#ifndef ROTPRESETBUTTON_H
#define ROTPRESETBUTTON_H

#include <QShortcut>
#include <QToolButton>
#include <QMenu>


class RotControlFrame;
class RotatorMainWindow;


//class RotPreset;
class RotPresetButton : public QObject
{
    Q_OBJECT

public:
    explicit RotPresetButton(QToolButton *b, RotControlFrame *rcf, int num);
    explicit RotPresetButton(QToolButton *b, RotatorMainWindow *rmw, int num);

    ~RotPresetButton();

    RotControlFrame *rotControlFrame = nullptr;
    RotatorMainWindow *rotMainWindow = nullptr;
    QToolButton* presetButton;
    QMenu* presetMenu;
    QShortcut* shortKey;
    QShortcut* shiftShortKey;
    QAction* readAction;
    QAction* writeAction;
    QAction* editAction;
    QAction* clearAction;

    int presetNo;

private slots:
    void presetUpdate();

    void presetShortCutSelected();
    void readActionSelected();
    void editActionSelected();
    void writeActionSelected();
    void clearActionSelected();
signals:
    void clearActionSelected(int);
    void selectRadio(QString);

    initButton();


};



#endif // ROTPRESETBUTTON_H
