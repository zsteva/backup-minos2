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
#include <QThread>
#include "rotcontrol.h"
#include <hamlib/rotator.h>
#include "rotatorCommonConstants.h"



QList<const rot_caps *> capsList;
bool rotatorlistLoaded=false;


int collect(const rot_caps *caps,rig_ptr_t)
{
    capsList.append(caps);
    return 1;
}

RotControl::RotControl(QObject *parent) : QObject(parent)
{

//   getRotatorList();
   rot_azimuth = 0.0;
   rot_elevation = 0.0;

   // set callback for debug messages
   // NB callback is the C function, not the class method.
   // user_data is used to point to our class.

   rig_set_debug_callback (::rig_message_cb, static_cast<rig_ptr_t>(this));

}




RotControl::~RotControl()
{
    rot_close(my_rot); /* close port */
    rot_cleanup(my_rot); /* if you care about memory */
}


int RotControl::getSupportCwCcwCmd(int rotNumber, bool *flag)
{
    int retCode = 0;
    ROT *my_rot;
    my_rot = rot_init(rotNumber);
    if (!my_rot == 0)
    {
            if (my_rot->caps->move == 0)
            {
                *flag = false;
            }
            else
            {
                *flag = true;
            }

            return retCode;
    }

    retCode = -1;
    return retCode;
}

int RotControl::getMaxMinRotation(int rotNumber, int *maxRot, int *minRot)
{
    int retCode = 0;
    ROT *my_rot;
    my_rot = rot_init(rotNumber);
    if (!my_rot == 0)
    {
        *maxRot = int(my_rot->caps->max_az);
        *minRot = int(my_rot->caps->min_az);
    }
    else
    {
        *maxRot = COMPASS_MAX360;
        *minRot = COMPASS_MIN0;
        retCode = -1;
    }


    return retCode;
}


int RotControl::init(srotParams* selectedAntenna)
{
    int retcode;

#if defined Q_OS_WIN32
    QString comport = "\\\\.\\";
#elif defined Q_OS_LINUX
    QString comport = "/dev/";
#elif defined Q_OS_MAC 
    QString comport = "/dev/";
    
#endif

    comport.append(selectedAntenna->comport);

    my_rot = rot_init(selectedAntenna->rotatorModelNumber);
    if (!my_rot)
    {
        return retcode = -14;
    }

    /*   remove
    // get rotator parameters
    curRotParams.antennaName = selectedAntenna->antennaName;
    curRotParams.baudrate =  selectedAntenna->baudrate;
    curRotParams.antennaOffset = selectedAntenna->antennaOffset;
    //rotParams.comport =
    curRotParams.databits = selectedAntenna->databits;
    curRotParams.stopbits = selectedAntenna->stopbits;
    curRotParams.parity = getSerialParityCode(selectedAntenna->parity);
    curRotParams.handshake = getSerialHandshakeCode(selectedAntenna->handshake);
    curRotParams.serial_rate_max = my_rot->caps->serial_rate_max;
    curRotParams.serial_rate_min = my_rot->caps->serial_rate_min;
    */

    // load rotator params to open
    if (rig_port_e(selectedAntenna->portType) == RIG_PORT_SERIAL)
    {
        strncpy(my_rot->state.rotport.pathname, comport.toLatin1().data(), comport.length());
        my_rot->state.rotport.parm.serial.rate = selectedAntenna->baudrate;
        my_rot->state.rotport.parm.serial.data_bits = selectedAntenna->databits;
        my_rot->state.rotport.parm.serial.stop_bits = selectedAntenna->stopbits;
        my_rot->state.rotport.parm.serial.parity = getSerialParityCode(selectedAntenna->parity);
        my_rot->state.rotport.parm.serial.handshake = getSerialHandshakeCode(selectedAntenna->handshake);
    }
    else if (rig_port_e(selectedAntenna->portType) == RIG_PORT_NETWORK || rig_port_e(selectedAntenna->portType) == RIG_PORT_UDP_NETWORK)
    {
        strncpy(my_rot->state.rotport.pathname, QString(selectedAntenna->networkAdd + ":" + selectedAntenna->networkPort).toLatin1().data(), FILPATHLEN);
    }
    else if (rig_port_e(selectedAntenna->portType) == RIG_PORT_NONE)
    {
        strncpy(my_rot->state.rotport.pathname, QString("").toLatin1().data(), FILPATHLEN);
    }


    retcode = rot_open(my_rot);
    if (retcode >= 0)
    {

        set_serialConnected(true);

        /* remove
        // update rotator specific parameters
        curRotParams.max_azimuth = my_rot->caps->max_az;
        curRotParams.min_azimuth = my_rot->caps->min_az;
        curRotParams.serial_rate_max = my_rot->caps->serial_rate_max;
        curRotParams.serial_rate_min = my_rot->caps->serial_rate_min;
        */
    }
    else
    {

        set_serialConnected(false);
    }

    return retcode;

}


int RotControl::closeRotator()
{

    if (my_rot->caps->rot_model == ROT_MODEL_SPID_ROT1PROG || my_rot->caps->rot_model == ROT_MODEL_SPID_ROT2PROG)
    {
        // make sure we stop for a moment to let any last poll finish sending
        QThread::msleep(1000);
    }
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
    rig_port_e portType = RIG_PORT_NONE;

    if(capsList.count()==0) return false;
    QStringList sl;
    // add blank at beginning
    sl << "";
    for (i=0;i<capsList.count();i++)
    {

        QString t;
        t= QString::number(capsList.at(i)->rot_model);
        t=t.rightJustified(5,' ')+", ";
        t+= capsList.at(i)->mfg_name;
        t+=", ";
        t+=capsList.at(i)->model_name;
        if (getPortType(capsList.at(i)->rot_model, &portType) != -1)
        {
            //qDebug() << capsList.at(i)->rot_model << capsList.at(i)->model_name << portType;
            if (portType == RIG_PORT_SERIAL || portType == RIG_PORT_NETWORK || portType == RIG_PORT_UDP_NETWORK || portType == RIG_PORT_NONE)
            {
                sl << t;
            }
        }
    }
    std::sort(sl.begin(), sl.end());
    cb->addItems(sl);
    return true;
}


int RotControl::getPortType(int rotNumber, rig_port_e *portType)
{

    int retCode = 0;
    ROT *my_rot;
    my_rot = rot_init(rotNumber);
    if (!my_rot == 0)
    {
        *portType = my_rot->caps->port_type;
        return retCode;
    }

    retCode = -1;
    return retCode;

}


int RotControl::getModelInfo(QString rotModel, int *rotModelNumber, QString *rotMfgName, QString *rotModelName)
{
    bool ok;
    int number;
    QStringList modelInfo = rotModel.remove('\x20').split(',');
    if (modelInfo.length() == 3)
    {
        number = modelInfo[0].toInt(&ok);
        if (!ok)
        {
           return -1;
        }

        *rotModelNumber = number;

        //modelInfo = modelInfo[1].split(',');
        //if (modelInfo.length() > 0)

        *rotMfgName = modelInfo[1].trimmed();
        *rotModelName = modelInfo[2].trimmed();
        return 0;


    }

    return -1;

}




/*

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

int RotControl::getMaxBaudRate()
{
    return curRotParams.serial_rate_max;
}


int RotControl::getMinBaudRate()
{
    return curRotParams.serial_rate_min;
}
*/

// stop azimuth rotation


int RotControl::stop_rotation()
{
    int retCode = RIG_OK;
    retCode = rot_stop(my_rot);


    return retCode;
}

// request current bearing from controller



int RotControl::request_bearing()
{
    int retCode = RIG_OK;

    retCode = rot_get_position (my_rot, &rot_azimuth, &rot_elevation);

    if (retCode == RIG_OK)
    {
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
    if (my_rot->caps->rot_model == ROT_MODEL_SPID_ROT1PROG || my_rot->caps->rot_model == ROT_MODEL_SPID_ROT2PROG)
    {
        QThread::msleep(1000);
    }

    return retCode;
}

int RotControl::rotateCClockwise(int speed)
{
    int retCode = RIG_OK;
    retCode = rot_move(my_rot, ROT_MOVE_LEFT , speed);
    if (my_rot->caps->rot_model == ROT_MODEL_SPID_ROT1PROG || my_rot->caps->rot_model == ROT_MODEL_SPID_ROT2PROG)
    {
        QThread::msleep(1000);
    }

    return retCode;
}


int RotControl::rotate_to_bearing(int bearing)
{
    int retCode = RIG_OK;
    float rotbearing = bearing;

    retCode = rot_set_position(my_rot, rotbearing, 0.0);
    if (my_rot->caps->rot_model == ROT_MODEL_SPID_ROT1PROG || my_rot->caps->rot_model == ROT_MODEL_SPID_ROT2PROG)
    {
        QThread::msleep(1000);
    }

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




QString RotControl::gethamlibVersion()
{
    QString ver = hamlib_version;
    return ver;
}


// which passes the call to this method
int RotControl::rig_message_cb(enum rig_debug_level_e /*debug_level*/, const char *fmt, va_list ap)
{
    char buf[1024];
//    rig_debug_level_e dbl = debug_level;

    vsprintf (buf, fmt, ap);
    QString s = QString::fromLatin1(buf);
    emit debug_protocol(s);

    return RIG_OK;
}



int rig_message_cb(enum rig_debug_level_e debug_level, rig_ptr_t user_data, const char *fmt, va_list ap)
{
    RotControl *rt = static_cast<RotControl *>(user_data);
    return rt->rig_message_cb(debug_level, fmt, ap);

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


