#ifndef KEYERABOUT_H
#define KEYERABOUT_H

#include <QDialog>

namespace Ui {
class keyerAbout;
}

class keyerAbout : public QDialog
{
    Q_OBJECT

public:
    explicit keyerAbout(QWidget *parent = 0);
    ~keyerAbout();

private slots:
    void on_pushButton_clicked();

    void on_AboutMemo_linkActivated(const QString &link);

private:
    Ui::keyerAbout *ui;
};

#endif // KEYERABOUT_H
