#ifndef TBUNDLEFRAME_H
#define TBUNDLEFRAME_H

#include <QFrame>

namespace Ui {
class TBundleFrame;
}

class TBundleFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TBundleFrame(QWidget *parent = 0);
    ~TBundleFrame();

private:
    Ui::TBundleFrame *ui;
};

#endif // TBUNDLEFRAME_H
