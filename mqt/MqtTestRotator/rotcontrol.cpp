#include <QList>
#include<QDebug>
#include <QString>
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
    rot_azimuth = 0.0;
    rot_elevation = 0.0;

}


RotControl::~RotControl()
{
    rot_close(my_rot); /* close port */
    rot_cleanup(my_rot); /* if you care about memory */
}

bool RotControl::init(srotParams currentAntenna)
{
    int retcode;
    QString comport = "\\\\.\\";
    comport.append(currentAntenna.comport);

    my_rot = rot_init(currentAntenna.rotatorModelNumber);
    if (!my_rot)
    {
        qDebug() << "Error init rotator";
    }

    // load rotator params
        strncpy(my_rot->state.rotport.pathname, (const char*)comport.toLatin1(), FILPATHLEN);
        my_rot->state.rotport.parm.serial.rate = currentAntenna.baudrate;
        my_rot->state.rotport.parm.serial.data_bits = currentAntenna.databits;
        my_rot->state.rotport.parm.serial.stop_bits = currentAntenna.stopbits;
        my_rot->state.rotport.parm.serial.parity = getSerialParityCode(currentAntenna.parity);
        my_rot->state.rotport.parm.serial.handshake = getSerialHandshakeCode(currentAntenna.handshake);

        retcode = rot_open(my_rot);
        if (retcode)
        {
            qDebug() << "rotator open error";
            set_serialConnected(false);
        }
        qDebug() << "rotator opened ok";
        set_serialConnected(true);

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

    qDebug() << "stop message" ;
    return retcode;
}

// request current bearing from controller
// send signal to serial write slot


void RotControl::request_bearing()
{
    int retcode;
    QString bearing;
    retcode = rot_get_position (my_rot, &rot_azimuth, &rot_elevation);
    if (retcode == RIG_OK)
    {
        bearing.setNum(rot_azimuth);
        emit bearing_updated(bearing);
    }

    //  Need error stuff here

}


azimuth_t RotControl::getRotatorAzimuth()
{
    return rot_azimuth;
}



int RotControl::rotateClockwise(int speed)
{

    int retcode;
    retcode = rot_move(my_rot, ROT_MOVE_CW , speed);
    qDebug() << "rotate CW message";
    return retcode;
}

int RotControl::rotateCClockwise(int speed)
{
    int retcode;
    retcode = rot_move(my_rot, ROT_MOVE_CCW , speed);
    qDebug() << "rotate CCW message";
    return retcode;
}


int RotControl::rotate_to_bearing(QString bearing)
{
    int retcode;
    qDebug() << "got to rotate bearing";

    rot_set_position(my_rot, (azimuth_t)bearing.toFloat(), 0.0);

    qDebug() << "rotate message to serial ";
    return retcode;

}

void RotControl::set_rotatorSpeed(int speed)
{
    rot_speed = speed;
}

int RotControl::get_rotatorSpeed()
{
    return rot_speed;
}


void RotControl::set_serialConnected(bool connectFlag)
{
    serialConnected = connectFlag;
}




bool RotControl::get_serialConnected()
{
    return serialConnected;
}



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
