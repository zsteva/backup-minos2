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
public:
    explicit TAboutBox(QWidget *parent, bool onStartup );
    ~TAboutBox();

    static bool ShowAboutBox(QWidget *Owner, bool onStartup );
    int exec();

    bool doStartup;
private slots:
    void on_ExitButton_clicked();

    void on_OKButton_clicked();

    void on_LoggerOnlyButton_clicked();

    void on_AboutMemo_linkActivated(const QString &link);
};

#endif // TABOUTBOX_H
