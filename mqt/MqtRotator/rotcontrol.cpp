/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////






#include <QList>
#include <QDebug>
#include <QStringList>
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

int RotControl::init(srotParams currentAntenna)
{
    int retcode;
    QString comport = "\\\\.\\";
    comport.append(currentAntenna.comport);

    my_rot = rot_init(currentAntenna.rotatorModelNumber);
    if (!my_rot)
    {
        qDebug() << "Error init rotator";
    }


    // get rotator parameters
    curRotParams.antennaName = currentAntenna.antennaName;
    curRotParams.baudrate =  currentAntenna.baudrate;
    //rotParams.comport =
    curRotParams.databits = currentAntenna.databits;
    curRotParams.stopbits = currentAntenna.stopbits;
    curRotParams.parity = getSerialParityCode(currentAntenna.parity);
    curRotParams.handshake = getSerialHandshakeCode(currentAntenna.handshake);
    curRotParams.serial_rate_max = my_rot->caps->serial_rate_max;
    curRotParams.serial_rate_min = my_rot->caps->serial_rate_min;
    curRotParams.min_azimuth = my_rot->caps->min_az;
    curRotParams.max_azimuth = my_rot->caps->max_az;
    curRotParams.min_elevation = my_rot->caps->min_el;
    curRotParams.max_elevation = my_rot->caps->max_el;






    // load rotator params to open
    strncpy(my_rot->state.rotport.pathname, (const char*)comport.toLatin1(), FILPATHLEN);
    my_rot->state.rotport.parm.serial.rate = currentAntenna.baudrate;
    my_rot->state.rotport.parm.serial.data_bits = currentAntenna.databits;
    my_rot->state.rotport.parm.serial.stop_bits = currentAntenna.stopbits;
    my_rot->state.rotport.parm.serial.parity = getSerialParityCode(currentAntenna.parity);
    my_rot->state.rotport.parm.serial.handshake = getSerialHandshakeCode(currentAntenna.handshake);


    retcode = rot_open(my_rot);
    if (retcode >= 0)
    {
        qDebug() << "rotator opened ok";
        set_serialConnected(true);
    }
    else
    {
        qDebug() << "rotator open error";
        set_serialConnected(false);
    }

    return retcode;

}


int RotControl::closeRotator()
{
    int retcode;
    retcode = rot_close (my_rot);

    retcode = rot_cleanup (my_rot);
    set_serialConnected(false);
    return retcode;

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
    QString t=curRotParams.rotatorModel;
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


azimuth_t RotControl::getMinAzimuth()
{
    return curRotParams.min_azimuth;
}




azimuth_t RotControl::getMaxAzimuth()
{
    return curRotParams.max_azimuth;
}

elevation_t RotControl::getMinElevation()
{
    return curRotParams.min_elevation;
}


elevation_t RotControl::getMaxElevation()
{
    return curRotParams.max_elevation;
}



// stop azimuth rotation


int RotControl::stop_rotation()
{
    int retCode = RIG_OK;
    retCode = rot_stop(my_rot);

    qDebug() << "stop message" ;
    return retCode;
}

// request current bearing from controller



int RotControl::request_bearing()
{
    int retCode = RIG_OK;
//    QString bearing;
    retCode = rot_get_position (my_rot, &rot_azimuth, &rot_elevation);
    //qDebug() << "request error" << retCode;
    if (retCode == RIG_OK)
    {
//        bearing.setNum(rot_azimuth);

//        bearing.setNum(rot_azimuth);

        emit bearing_updated(rot_azimuth);
    }

    return retCode;

}


azimuth_t RotControl::getRotatorAzimuth()
{
    return rot_azimuth;
}



int RotControl::rotateClockwise(int speed)
{

    int retCode = RIG_OK;
    retCode = rot_move(my_rot, ROT_MOVE_RIGHT , speed);
    qDebug() << "rotate CW message";
    return retCode;
}

int RotControl::rotateCClockwise(int speed)
{
    int retCode = RIG_OK;
    retCode = rot_move(my_rot, ROT_MOVE_LEFT , speed);
    qDebug() << "rotate CCW message";
    return retCode;
}


int RotControl::rotate_to_bearing(int bearing)
{
    int retCode = RIG_OK;
    float rotbearing = bearing;
    //qDebug() << "got to rotate bearing";
    //qDebug() << "send rotate message to serial ";
    retCode = rot_set_position(my_rot, rotbearing, 0.0);
    //qDebug() << "error" << retCode;


    return retCode;

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

     return serialData::parityCodes[index];

 }

 enum serial_handshake_e RotControl::getSerialHandshakeCode(int index)
 {

     return serialData::handshakeCodes[index];
 }

 QStringList RotControl::getParityCodeNames()
 {
    return serialData::parityStr;
 }

 QStringList RotControl::getHandShakeNames()
 {
     return serialData::handshakeStr;
 }

 QStringList RotControl::getBaudRateNames()
 {


     return serialData::baudrateStr;
 }

 QStringList RotControl::getDataBitsNames()
 {
     return serialData::databitsStr;
 }

 QStringList RotControl::getStopBitsNames()
 {
     return serialData::stopbitsStr;
 }

QString RotControl::gethamlibErrorMsg(int errorCode)
{
    if (errorCode > serialData::hamlibErrorMsg.count())
    {
        return "hamlib Errorcode too large!";
    }
    return serialData::hamlibErrorMsg[errorCode];
}


QStringList RotControl::gethamlibErrorMsg()
{

    return serialData::hamlibErrorMsg;
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
