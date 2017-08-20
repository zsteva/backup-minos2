#ifndef RIGMEMPUSHBUTTON_H
#define RIGMEMPUSHBUTTON_H

#include <QPushButton>
#include <QMouseEvent>


class RigMemPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit RigMemPushButton(QWidget *parent = 0);

private slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void rightClicked();

};



#endif // RIGMEMPUSHBUTTON_H
