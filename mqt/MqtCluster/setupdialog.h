#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>

namespace Ui {
class SetupDialog;
}

const QString CLUSTER_CONFIG_FILE = "./Configuration/cluster.ini";


class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(QWidget *parent = 0);
    ~SetupDialog();

private:
    Ui::SetupDialog *ui;
    void savePersonel();
    void readPersonel();

    QString callsign;
    QString name;
    QString locator;
    QString qth;



};

#endif // SETUPDIALOG_H
