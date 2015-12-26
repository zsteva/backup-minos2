#ifndef DXCCFRAME_H
#define DXCCFRAME_H

#include <QFrame>

namespace Ui {
class DXCCFrame;
}

class DXCCFrame : public QFrame
{
    Q_OBJECT

public:
    explicit DXCCFrame(QWidget *parent = 0);
    ~DXCCFrame();

    void reInitialiseCountries();
private:
    Ui::DXCCFrame *ui;
};

#endif // DXCCFRAME_H
