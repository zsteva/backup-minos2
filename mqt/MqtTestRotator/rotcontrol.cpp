#include <QList>
#include<QDebug>

#include "rotcontrol.h"
#include <hamlib/rotator.h>
//#include "rotctl_parse.h"


QList<const rot_caps *> capsList;
bool rotatorlistLoaded=false;


int collect(const rot_caps *caps,rig_ptr_t)
{
    capsList.append(caps);
    return 1;
}



RotControl::RotControl()
{


    getRotatorList();
//    my_model = atoi("601");
//    rot_file = "COM2";
//    serial_rate = atoi("9600");

//    my_rot = rot_init(my_model);

//    retcode = set_conf(my_rot, conf_parms);

//    if (rot_file)
//        strncpy(my_rot->state.rotport.pathname, rot_file, FILPATHLEN - 1);

    /* FIXME: bound checking and port type == serial */
//    if (serial_rate != 0)
//        my_rot->state.rotport.parm.serial.rate = serial_rate;

//    retcode = rot_open(my_rot);





}


RotControl::~RotControl()
{
    rot_close(my_rot); /* close port */
    rot_cleanup(my_rot); /* if you care about memory */
}

bool RotControl::init(srotParams currentAntenna)
{
    int retcode;

    my_rot = rot_init(currentAntenna.rotatorModelNumber);
    if (!my_rot)
    {
        qDebug() << "Error init rotator";
    }
    // load rotator params
        strncpy(my_rot->state.rotport.pathname, (const char*)currentAntenna.comport.toLatin1(), FILPATHLEN);
        my_rot->state.rotport.parm.serial.rate = currentAntenna.baudrate;
        my_rot->state.rotport.parm.serial.data_bits = currentAntenna.databits;
        my_rot->state.rotport.parm.serial.stop_bits = currentAntenna.stopbits;
        my_rot->state.rotport.parm.serial.parity = getSerialParityCode(currentAntenna.parity);
        my_rot->state.rotport.parm.serial.handshake = getSerialHandshakeCode(currentAntenna.handshake);

        retcode = rot_open(my_rot);
        if (retcode)
        {
            qDebug() << "rotator open error";
        }
        qDebug() << "rotator opened ok";

        return true;

}

void RotControl::getRotatorList()
{
    if(!rotatorlistLoaded)
    {
        capsList.clear();
        rot_load_all_backends();
        rot_list_foreach(collect,0);
        qSort(capsList.begin(),capsList.end(),model_Sort);
        rotatorlistLoaded=true;
    }
}

bool RotControl::getRotatorList(QComboBox *cb)
{
    int i;
    if(capsList.count()==0) return false;
    QStringList sl;
    for (i=0;i<capsList.count();i++)
    {
        QString t;
        t= QString::number(capsList.at(i)->rot_model);
        t=t.rightJustified(5,' ')+" ";
        t+= capsList.at(i)->mfg_name;
        t+=",";
        t+=capsList.at(i)->model_name;
        sl << t;
    }
    cb->addItems(sl);
    return true;
}

int RotControl::getModelNumber(int idx)
{
    if(idx<0) return 0;
    return capsList.at(idx)->rot_model;
}

const char * RotControl::getMfg_Name(int idx)
{

    if(idx<0) return 0;
    return capsList.at(idx)->mfg_name;
}




const char * RotControl::getModel_Name(int idx)
{

    if(idx<0) return 0;
    return capsList.at(idx)->model_name;
}


int RotControl::getRotatorModelIndex()
{
    int i;
    QString t=rotParams.rotatorModel;
    t=t.remove(0,5);
    t=t.simplified();
    QStringList sl=t.split(",");
    if(sl.count()==1) sl.append("");
    for(i=0;i<capsList.count();i++)
    {
        if((capsList.at(i)->mfg_name==sl.at(0)) && (capsList.at(i)->model_name==sl.at(1)))
        {
            return i;
        }
    }
    return -1;
}


// stop azimuth rotation


int RotControl::stop_rotation()
{
    int retcode;
    rot_stop (my_rot);

    qDebug() << "stop message" << stop_message;
    return retcode;
}

// request current bearing from controller
// send signal to serial write slot


int RotControl::request_bearing()
{
    int retcode;
    rot_get_position (my_rot, &rot_azimuth, &rot_elevation);

    return retcode;

}


azimuth_t RotControl::getRotatorAzimuth()
{
    return rot_azimuth;
}



int RotControl::rotateClockwise(int speed)
{

    int retcode;
    retcode = rot_move(my_rot, ROT_MOVE_CW , speed);
    qDebug() << "rotate CW message" << cw_message;
    return retcode;
}

int RotControl::rotateCClockwise(int speed)
{
    int retcode;
    retcode = rot_move(my_rot, ROT_MOVE_CCW , speed);
    qDebug() << "rotate CCW message" << ccw_message;
    return retcode;
}


int RotControl::rotate_to_bearing(QString bearing)
{
    int retcode;
    qDebug() << "got to rotate bearing";

    rot_set_position(my_rot, (azimuth_t)bearing, 0.0);

    qDebug() << "rotate message to serial " << tx_rotate_to_message;
    return retcode;

}


int rotate_to_bearing(QString bearing);
int rotateCClockwise(int speed);
int rotateClockwise(int speed);
azimuth_t getRotatorAzimuth();
int request_bearing();
int stop_rotation();

 enum serial_parity_e RotControl::getSerialParityCode(int index)
 {
     enum serial_parity_e parity_codes[5] = {RIG_PARITY_NONE, RIG_PARITY_ODD, RIG_PARITY_EVEN, RIG_PARITY_MARK, RIG_PARITY_SPACE};

     return parity_codes[index];

 }

 enum serial_handshake_e RotControl::getSerialHandshakeCode(int index)
 {
     enum serial_handshake_e handshake_codes[3] = {RIG_HANDSHAKE_NONE, RIG_HANDSHAKE_XONXOFF, RIG_HANDSHAKE_HARDWARE};

     return handshake_codes[index];
 }

bool model_Sort(const rot_caps *caps1,const rot_caps *caps2)
{
    if(caps1->mfg_name==caps2->mfg_name)
    {
        if (QString::compare(caps1->model_name,caps2->model_name)<0) return true;
        return false;
    }
    if (QString::compare(caps1->mfg_name,caps2->mfg_name)<0) return true;
    return false;
}
