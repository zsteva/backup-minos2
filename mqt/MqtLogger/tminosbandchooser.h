#ifndef TMINOSBANDCHOOSER_H
#define TMINOSBANDCHOOSER_H

#include <QDialog>

namespace Ui {
class TMinosBandChooser;
}

class TMinosBandChooser : public QDialog
{
    Q_OBJECT

public:
    explicit TMinosBandChooser(QWidget *parent = 0);
    ~TMinosBandChooser();

private:
    Ui::TMinosBandChooser *ui;
};

#endif // TMINOSBANDCHOOSER_H
