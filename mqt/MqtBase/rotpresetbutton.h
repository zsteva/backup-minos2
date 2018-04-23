#ifndef ROTPRESETBUTTON_H
#define ROTPRESETBUTTON_H

#include <QShortcut>
#include <QToolButton>
#include <QMenu>




//class RotPreset;
class RotPresetButton : public QObject
{
    Q_OBJECT

public:
    explicit RotPresetButton(QToolButton *b, int num);

    ~RotPresetButton();

    QToolButton* presetButton;

    void setText(QString t);
    QString getText();
signals:

    void presetReadAction();
    void presetEditAction();
    void presetWriteAction();
    void presetClearAction();

private:


    QMenu* presetMenu;
    QShortcut* shortKey;
    QShortcut* shiftShortKey;
    QAction* readAction;
    QAction* writeAction;
    QAction* editAction;
    QAction* clearAction;


    int presetNum;

private slots:
    //void presetUpdate();

    void presetShortCutSelected();
    void readActionSelected();
    void editActionSelected();
    void writeActionSelected();
    void clearActionSelected();






};



#endif // ROTPRESETBUTTON_H
