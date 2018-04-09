#ifndef TMINOSBANDCHOOSER_H
#define TMINOSBANDCHOOSER_H

#include "base_pch.h"

namespace Ui {
class TMinosBandChooser;
}

class TMinosBandChooser : public QDialog
{
    Q_OBJECT

public:
    explicit TMinosBandChooser(QWidget *parent = nullptr);
    ~TMinosBandChooser();

private:
    Ui::TMinosBandChooser *ui;
};

#endif // TMINOSBANDCHOOSER_H
