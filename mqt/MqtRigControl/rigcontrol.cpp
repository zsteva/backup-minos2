/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2017
//
//
// Hamlib Library
//
/////////////////////////////////////////////////////////////////////////////



#include <QList>
#include <QDebug>
#include <QStringList>
#include "rigcontrol.h"
#include <hamlib/rig.h>





QList<const rig_caps *> capsList;
bool riglistLoaded=false;





int collect(const rig_caps *caps,rig_ptr_t)
{
    capsList.append(caps);
    return 1;
}



RigControl::RigControl(QObject *parent) : QObject(parent)
{


    // set callback for debug messages
    // NB callback is the C function, not the class method.
    // user_data is used to point to our class.

    rig_set_debug_callback (::rig_message_cb, static_cast<rig_ptr_t>(this));

}



RigControl::~RigControl()
{
    rig_close(my_rig); /* close port */
    rig_cleanup(my_rig); /* if you care about memory */
}

int RigControl::init(scatParams currentRadio)
{
    int retcode;
    passBandState = hamlibData::NOR;
    QString comport = "\\\\.\\";
    comport.append(currentRadio.comport);

    my_rig = rig_init(currentRadio.radioModelNumber);
    if (!my_rig)
    {
        qDebug() << "Error init rig";
    }

    // load cat params

    if(QString(my_rig->caps->mfg_name) == "Icom")
    {
        if(!currentRadio.civAddress.isEmpty())
        {
            retcode = rig_set_conf(my_rig, rig_token_lookup(my_rig, "civaddr"),currentRadio.civAddress.toLatin1());
        }
    }
    strncpy(my_rig->state.rigport.pathname, comport.toLatin1().data(), FILPATHLEN);
    my_rig->state.rigport.parm.serial.rate = currentRadio.baudrate;
    my_rig->state.rigport.parm.serial.data_bits = currentRadio.databits;
    my_rig->state.rigport.parm.serial.stop_bits = currentRadio.stopbits;
    my_rig->state.rigport.parm.serial.parity = getSerialParityCode(currentRadio.parity);
    my_rig->state.rigport.parm.serial.handshake = getSerialHandshakeCode(currentRadio.handshake);


    retcode = rig_open(my_rig);
    if (retcode >= 0)
    {
        qDebug() << "rig opened ok";
        set_serialConnected(true);
    }
    else
    {
        qDebug() << "rig open error";
        set_serialConnected(false);
    }

    return retcode;

}


int RigControl::closeRig()
{
    int retcode;
    retcode = rig_close(my_rig);

    retcode = rig_cleanup(my_rig);
    set_serialConnected(false);
    return retcode;

}


int RigControl::getFrequency(vfo_t vfo, freq_t *frequency)
{
    return rig_get_freq(my_rig, vfo, frequency);
}


int RigControl::setFrequency(freq_t frequency, vfo_t vfo)
{
    return (rig_set_freq(my_rig, vfo, frequency));
}

/* ---------------------- Mode ------------------------------------ */

int RigControl::getMode(vfo_t vfo, rmode_t *mode, pbwidth_t *width)
{
    return rig_get_mode(my_rig, vfo, mode, width);
}

int RigControl::setMode(vfo_t vfo, rmode_t mode, pbwidth_t passBandwidth)
{
    return (rig_set_mode(my_rig, vfo, mode, passBandwidth));
}

// Hamlib conversion
QString RigControl::convertModeQstr(rmode_t mode)
{
    return QString::fromLatin1(rig_strrmode(mode));
}

rmode_t RigControl::convertQStrMode(QString mode)
{
    return rig_parse_mode(mode.toLatin1());

}

// rigControl conversion

int RigControl::rigConvertQStrMode(QString mode)
{
    for (int i = 0; i < hamlibData::supModeList.count(); i++)
    {
        if (mode == hamlibData::supModeList[i])
        {
            return i;
        }
    }
    return -1; //not found
}

/* ---------------------- VFO ------------------------------------ */
// Note not all radios support reading the VFO

int RigControl::getVfo(vfo_t *vfo)
{

    return rig_get_vfo(my_rig, vfo);
}



int RigControl::setVfo(vfo_t vfo)
{
    return rig_set_vfo(my_rig, vfo);
}




QString RigControl::convertVfoQStr(vfo_t vfo)
{
    return QString::fromLatin1(rig_strvfo(vfo));
}


/*************** Passband ********************************/





pbwidth_t RigControl::passbandNarrow(rmode_t mode)
{
    return rig_passband_narrow(my_rig, mode);
}

pbwidth_t RigControl::passbandNormal(rmode_t mode)
{
    return rig_passband_normal(my_rig, mode);
}

pbwidth_t RigControl::passbandWide(rmode_t mode)
{
    return rig_passband_wide(my_rig, mode);
}


void RigControl::buildPassBandTable()
{
    CW_PASSBAND_NAR = passbandNarrow(convertQStrMode("CW"));
    USB_PASSBAND_NAR = passbandNarrow(convertQStrMode("USB"));
    FM_PASSBAND_NAR = passbandNarrow(convertQStrMode("FM"));

    CW_PASSBAND_NOR = passbandNormal(convertQStrMode("CW"));
    USB_PASSBAND_NOR = passbandNormal(convertQStrMode("USB"));
    FM_PASSBAND_NOR = passbandNormal(convertQStrMode("FM"));

    CW_PASSBAND_WID = passbandWide(convertQStrMode("CW"));
    USB_PASSBAND_WID = passbandWide(convertQStrMode("USB"));
    FM_PASSBAND_WID = passbandWide(convertQStrMode("FM"));

    passBandWidth[0][0] = CW_PASSBAND_NAR;
    passBandWidth[0][1] = CW_PASSBAND_NOR;
    passBandWidth[0][2] = CW_PASSBAND_WID;

    passBandWidth[1][0] = USB_PASSBAND_NAR;
    passBandWidth[1][1] = USB_PASSBAND_NOR;
    passBandWidth[1][2] = USB_PASSBAND_WID;

    passBandWidth[2][0] = FM_PASSBAND_NAR;
    passBandWidth[2][1] = FM_PASSBAND_NOR;
    passBandWidth[2][2] = FM_PASSBAND_WID;

    passBandWidth[3][0] = MGM_PASSBAND_NAR;
    passBandWidth[3][1] = MGM_PASSBAND_NOR;
    passBandWidth[3][2] = MGM_PASSBAND_WID;


}


pbwidth_t RigControl::lookUpPassBand(QString mode, int modeState)
{
    int m = -1;
    for (int i=0; i < hamlibData::supModeList.count(); i++)
    {
        if (mode == hamlibData::supModeList[i])
        {
            m = i;
        }
    }
    if (m < 0)
    {
        return 0;
    }
    else
    {
       return passBandWidth[m][modeState];
    }
}


void RigControl::setPassBand(QString mode, int modeState)
{
    int imode = rigConvertQStrMode(mode);
    if (imode == -1)
    {
        return;
    }
    else
    {
        pbwidth = passBandWidth[imode][modeState];
    }

}

pbwidth_t RigControl::getPassBand()
{
    return pbwidth;
}

void RigControl::getRigList()
{
    if(!riglistLoaded)
    {
        capsList.clear();
        rig_load_all_backends();
        rig_list_foreach(collect,0);
        qSort(capsList.begin(),capsList.end(),model_Sort);
        riglistLoaded=true;
    }
}








bool RigControl::getRigList(QComboBox *cb)
{
    int i;
    if(capsList.count()==0) return false;
    QStringList sl;
    for (i=0;i<capsList.count();i++)
    {
        QString t;
        t= QString::number(capsList.at(i)->rig_model);
        t=t.rightJustified(5,' ')+" ";
        t+= capsList.at(i)->mfg_name;
        t+=",";
        t+=capsList.at(i)->model_name;
        sl << t;
    }
    cb->addItems(sl);
    return true;
}

int RigControl::getModelNumber(int idx)
{
    if(idx<0) return 0;
    return capsList.at(idx)->rig_model;
}

const char * RigControl::getMfg_Name(int idx)
{

    if(idx<0) return 0;
    return capsList.at(idx)->mfg_name;
}




const char * RigControl::getModel_Name(int idx)
{

    if(idx<0) return 0;
    return capsList.at(idx)->model_name;
}


int RigControl::getRigModelIndex()
{
    int i;
    QString t=catParams.radioModel;
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





void RigControl::set_serialConnected(bool connectFlag)
{
    serialConnected = connectFlag;
}




bool RigControl::get_serialConnected()
{
    return serialConnected;
}



 enum serial_parity_e RigControl::getSerialParityCode(int index)
 {

     return serialData::parityCodes[index];

 }

 enum serial_handshake_e RigControl::getSerialHandshakeCode(int index)
 {

     return serialData::handshakeCodes[index];
 }

 QStringList RigControl::getParityCodeNames()
 {
    return serialData::parityStr;
 }

 QStringList RigControl::getHandShakeNames()
 {
     return serialData::handshakeStr;
 }

 QStringList RigControl::getBaudRateNames()
 {


     return serialData::baudrateStr;
 }

 QStringList RigControl::getDataBitsNames()
 {
     return serialData::databitsStr;
 }

 QStringList RigControl::getStopBitsNames()
 {
     return serialData::stopbitsStr;
 }

QString RigControl::gethamlibErrorMsg(int errorCode)
{
    if (errorCode > serialData::hamlibErrorMsg.count())
    {
        return "hamlib Errorcode too large!";
    }
    return serialData::hamlibErrorMsg[errorCode];
}


QStringList RigControl::gethamlibErrorMsg()
{

    return serialData::hamlibErrorMsg;
}




bool model_Sort(const rig_caps *caps1,const rig_caps *caps2)
{
    if(caps1->mfg_name==caps2->mfg_name)
    {
        if (QString::compare(caps1->model_name,caps2->model_name)<0) return true;
        return false;
    }
    if (QString::compare(caps1->mfg_name,caps2->mfg_name)<0) return true;
    return false;
}


// which passes the call to this method
int RigControl::rig_message_cb(enum rig_debug_level_e /*debug_level*/, const char *fmt, va_list ap)
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
    RigControl *rt = static_cast<RigControl *>(user_data);
    return rt->rig_message_cb(debug_level, fmt, ap);

}
