#ifndef ROTATORLOGIC_H
#define ROTATORLOGIC_H

#include "base_pch.h"

class RotatorMainWindow;

class RotatorLogic : public QObject
{
    Q_OBJECT

    bool connected;
    bool subscribed;
    QTimer ConnectTimer;

    RotatorMainWindow *parent;


    void notifyCallback( bool err, MinosRPCObj *mro, const QString &from );
    void rotatorClientCallback( bool err, MinosRPCObj *mro, const QString &from );
    void rotatorServerCallback( bool err, MinosRPCObj *mro, const QString &from );

    static void makeRPCObjects();
public:
    explicit RotatorLogic(RotatorMainWindow *parent);

    void publishState( const QString &state );

signals:
    void setRotation(int direction, int angle);

private slots:
    void ConnectTimerTimer( );
};

#endif // ROTATORLOGIC_H
