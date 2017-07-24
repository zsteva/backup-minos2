#ifndef TABOUTBOX_H
#define TABOUTBOX_H

#include <QDialog>

namespace Ui {
class TAboutBox;
}

class TAboutBox : public QDialog
{
    Q_OBJECT

    Ui::TAboutBox *ui;
    explicit TAboutBox(QWidget *parent, bool onStartup );
    ~TAboutBox();

    int exec();

    void doCloseEvent();

public:
    static bool ShowAboutBox(QWidget *Owner, bool onStartup );
    bool doStartup;


public slots:
    virtual void reject() override;

private slots:
    virtual void accept() override;

    void on_ExitButton_clicked();

    void on_OKButton_clicked();

    void on_LoggerOnlyButton_clicked();

    void on_AboutMemo_linkActivated(const QString &link);
    void on_AppsButton_clicked();
};

#endif // TABOUTBOX_H
