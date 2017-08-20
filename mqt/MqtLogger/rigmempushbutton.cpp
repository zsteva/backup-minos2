#include "rigmempushbutton.h"

RigMemPushButton::RigMemPushButton(QWidget *parent):
     QPushButton(parent)
{

}


void RigMemPushButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton)
        emit rightClicked();
}
