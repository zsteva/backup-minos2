#ifndef TSESSIONMANAGER_H
#define TSESSIONMANAGER_H

#include <QDialog>

namespace Ui {
class TSessionManager;
}

class TSessionManager : public QDialog
{
    Q_OBJECT

public:
    explicit TSessionManager(QWidget *parent = 0);
    ~TSessionManager();
    QString currSession;

private slots:

    void on_OKButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::TSessionManager *ui;
    void doCloseEvent();

public Q_SLOTS:
    virtual void accept() override;
    virtual void reject() override;
};

#endif // TSESSIONMANAGER_H
