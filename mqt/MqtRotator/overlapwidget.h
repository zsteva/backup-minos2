#ifndef OVERLAPWIDGET_H
#define OVERLAPWIDGET_H

#include <QWidget>

class OverLapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OverLapWidget(QWidget *parent = 0);
    void setText(QString text);
signals:

public slots:
    void overlapDisplayUpdate(bool);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


private:
    bool overlapOn;

};

#endif // OVERLAPWIDGET_H
