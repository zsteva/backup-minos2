#ifndef STARTCONFIG_H
#define STARTCONFIG_H

#include <QDialog>

namespace Ui {
class StartConfig;
}

class StartConfig : public QDialog
{
    Q_OBJECT

public:
    explicit StartConfig(QWidget *parent, bool showAutoStart);
    ~StartConfig() override;

private:
    Ui::StartConfig *ui;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;
    virtual void changeEvent( QEvent* e ) override;
};

#endif // STARTCONFIG_H
