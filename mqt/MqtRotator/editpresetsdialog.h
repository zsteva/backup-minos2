/////////////////////////////////////////////////////////////////////////////
// $Id$
//
// PROJECT NAME 		Minos Amateur Radio Control and Logging System
//                      Rotator Control
// Copyright        (c) D. G. Balharrie M0DGB/G8FKH 2016
//
//
/////////////////////////////////////////////////////////////////////////////






#ifndef EDITPRESETSDIALOG_H
#define EDITPRESETSDIALOG_H

#include <QDialog>
#include<QLineEdit>

#define NUM_PRESETS 10

namespace Ui {
class EditPresetsDialog;
}

class EditPresetsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPresetsDialog(QWidget *parent = 0);
    ~EditPresetsDialog();


signals:
    void showEditPresetDialog();
    void updatePresetButtonLabels();

public slots:
    void loadPresetEditFieldsShow();

protected slots:
    void nameReturnPressed(int boxNumber);
    void bearingReturnPressed(int boxNumber);
    void cancelButtonPushed();
    void saveButtonPushed();
    void lineEditReturn();

protected:
 void keyPressEvent(QKeyEvent *);

private:
    Ui::EditPresetsDialog *ui;

    QLineEdit* presetNameLineEdit[NUM_PRESETS];
    QLineEdit* presetBearingLineEdit[NUM_PRESETS];
    QString presetName[NUM_PRESETS];
    bool presetNameUpdated[NUM_PRESETS];
    QString presetBearing[NUM_PRESETS];
    bool presetBearingUpdated[NUM_PRESETS];
    bool presetValueChanged;
    void getPresets();
    void savePresets();
    void clearPresets();
    void readPresets();
    void closePresets();
};

#endif // EDITPRESETSDIALOG_H
