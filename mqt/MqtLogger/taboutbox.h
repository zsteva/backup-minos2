#ifndef TABOUTBOX_H
#define TABOUTBOX_H

#include <QDialog>

namespace Ui {
class TAboutBox;
}

class TAboutBox : public QDialog
{
    Q_OBJECT

public:
    explicit TAboutBox(QWidget *parent, bool onStartup );
    ~TAboutBox();

    static bool ShowAboutBox( QWidget *Owner, bool onStartup );

private slots:
    void on_ExitButton_clicked();

    void on_OKButton_clicked();

    void on_LoggerOnlyButton_clicked();

private:
    Ui::TAboutBox *ui;

    bool doStartup;
};

#endif // TABOUTBOX_H
