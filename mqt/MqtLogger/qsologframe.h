#ifndef QSOLOGFRAME_H
#define QSOLOGFRAME_H

#include <QFrame>

namespace Ui {
class QSOLogFrame;
}

class QSOLogFrame : public QFrame
{
    Q_OBJECT

public:
    explicit QSOLogFrame(QWidget *parent = 0);
    ~QSOLogFrame();

private:
    Ui::QSOLogFrame *ui;
};

#endif // QSOLOGFRAME_H
