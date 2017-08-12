#ifndef BANDMAPFRAME_H
#define BANDMAPFRAME_H

#include "logger_pch.h"

namespace Ui {
class BandMapFrame;
}


class BandMapFrame : public QFrame
{
    Q_OBJECT

public:
    explicit BandMapFrame(QWidget *parent);
    ~BandMapFrame();

    Ui::BandMapFrame *ui;

    void setBandMapLoaded();



private:

   bool bandMapLoaded;
   bool isBandMapLoaded();


};

#endif // BANDMAPFRAME_H
