/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rig Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2018
//
//
//
//
/////////////////////////////////////////////////////////////////////////////




#ifndef FREQPRESETDIALOG_H
#define FREQPRESETDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QLineEdit>
#include "BandList.h"
#include "rigcontrolcommonconstants.h"
#include "rigutils.h"



namespace Ui {
class FreqPresetDialog;
}

const int NUNPRESET = 10;
enum bandOffSet {_10M, _6M, _4M, _2M, _70CM, _23CM, _13CM, _9CM, _6CM, _3CM};


const QStringList bandFreq = { "28170000",
                               "50170000",
                               "70210000",
                               "144290000",
                               "432170000",
                               "1296210000",
                               "2320200000",  //13cm
                               "3400100000", // 9cm
                               "5760100000", // 6cm
                              "10368100000"  // 3cm
                              };


class FreqPresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FreqPresetDialog(QStringList& _presetFreq, const QVector<BandDetail*> _bands, bool& freqPresetChanged, QWidget *parent = 0);
    ~FreqPresetDialog();


    static void readSettings(QStringList &_presetFreq);




private slots:
    void b_10mSelected();
    void b_6mSelected();
    void b_4mSelected();
    void b_2mSelected();
    void b_70cmSelected();
    void b_23cmSelected();
    void b_13cmSelected();
    void b_9cmSelected();
    void b_6cmSelected();
    void b_3cmSelected();

    void saveSettings();
    void cancelSettings();



private:
    Ui::FreqPresetDialog *ui;
    QStringList presetFreq;
    QVector<BandDetail*> bands;
    bool freqChanged = false;
    bool freqPresetChanged = false;


    bool checkInBand(double freq, bandOffSet band);
    void getFreq(QLineEdit* f_box, bandOffSet band);

    void loadSettingsToDialog();
};

#endif // FREQPRESETDIALOG_H
