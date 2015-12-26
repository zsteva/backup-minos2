#ifndef DISTRICTFRAME_H
#define DISTRICTFRAME_H

#include <QFrame>

namespace Ui {
class DistrictFrame;
}

class DistrictFrame : public QFrame
{
    Q_OBJECT

public:
    explicit DistrictFrame(QWidget *parent = 0);
    ~DistrictFrame();

    void reInitialiseDistricts();

private:
    Ui::DistrictFrame *ui;
};

#endif // DISTRICTFRAME_H
