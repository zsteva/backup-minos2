#ifndef TCONFIGFRAME_H
#define TCONFIGFRAME_H

#include <QFrame>

namespace Ui {
class TConfigFrame;
}

class TAboutBox;
class TConfigFrame : public QFrame
{
    Q_OBJECT

    TAboutBox * dialog;

public:
    explicit TConfigFrame(QWidget *parent = 0);
    ~TConfigFrame();
    void initialise(TAboutBox *b);
    void setup(bool started);

    void start();

private slots:
    void on_StartButton_clicked();

    void on_StopButton_clicked();

    void on_HideCheckBox_clicked();

    void on_SetButton_clicked();

    void on_ClearAllButton_clicked();

    void on_ModifyButton_clicked();

    void on_CancelButton_clicked();

private:
    Ui::TConfigFrame *ui;
};
#endif // TTCONFIGFRAME_H
