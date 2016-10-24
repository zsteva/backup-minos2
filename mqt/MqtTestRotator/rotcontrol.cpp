#include <QList>

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

/*
rigControl::rigControl(int radioIndex)
{
    rigControlEnabled=false;
    rotParams.configLabel=QString("radio%1").arg(radioIndex);
    rig_set_debug(RIG_DEBUG_NONE);
    getRadioList();
    serialP=0;
    lastFrequency=0.0;
    xmlModes<<"USB"<<"LSB"<<"FM"<<"AM";
}

*/

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
