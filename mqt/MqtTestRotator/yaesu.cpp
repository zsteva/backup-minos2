#include "yaesu.h"

#include <QtSerialPort>
#include <QObject>
#include <QString>
#include <QByteArray>

#include <QtDebug>

Yaesu::Yaesu(QSerialPort *serialPort) :
    m_serialPort(serialPort)
{
    command_sent = "";
    old_bearing_from_controller = "";
    serialBuffer.clear();
    gotSOM = false;
}


Yaesu::~Yaesu()
{

}


// stop azimuth rotation


void Yaesu::stop_rotation()
{

    QString message = STOP_AZ_ROT;
    message.append(CR);
    // convert to 8 bit for transmission
    stop_message = message.toLatin1();
    qDebug() << "stop message" << stop_message;
    emit send_stop_msg(stop_message);

}

// request current bearing from controller
// send signal to serial write slot


void Yaesu::request_bearing()
{

    QString message = RET_CUR_AZ;
    message.append(CR);
    // convert to 8 bit for transmission
    get_bearing_message = message.toLatin1();

    emit send_bearing_msg(get_bearing_message);

}

void Yaesu::rotateClockwise()
{
    QString message = ROT_AZ_CW;
    message.append(CR);
    // convert to 8 bit for transmission
    cw_message = message.toLatin1();
    qDebug() << "rotate CW message" << cw_message;
    emit send_az_cw_msg(cw_message);
}

void Yaesu::rotateCClockwise()
{
    QString message = ROT_AZ_CCW;
    message.append(CR);
    // convert to 8 bit for transmission
    ccw_message = message.toLatin1();
    qDebug() << "rotate CCW message" << ccw_message;
    emit send_az_ccw_msg(ccw_message);
}

// handles data from controller
// following a request for current bearing

void Yaesu::received_bearing()
{

    int somPosition = -1;
    int eomPosition = -1;
    int messageSize = 0;
    serialData = m_serialPort->readAll();
    //qDebug() << "rawdata" << serialData;
    somPosition = serialData.indexOf(SOM,0);
    eomPosition = serialData.indexOf(LF,0);
    messageSize = serialData.size();
    if (!gotSOM)
    {
        if (somPosition > -1 && eomPosition == -1)
        {
            // only SOM in serialData
            //qDebug() << " only SOM ";
            //qDebug() << serialData;
            serialBuffer = serialData.mid(somPosition, messageSize - somPosition);
            gotSOM = true;
            //qDebug() << serialBuffer;
        }
    }
    else
    {
        if (eomPosition == -1 && somPosition == -1 && messageSize > 0)
        {
            // no SOM or EOM in serialData
            //qDebug() << " no SOM or EOM ";
            //qDebug() << serialData;
            //qDebug() << " mid " << serialData.mid(0, messageSize);
            serialBuffer = serialBuffer.append(serialData.mid(0, messageSize));
            //qDebug() << serialBuffer;
        }
        else if (eomPosition > -1 && somPosition == -1)
        {
            // only eom in serialData
            //qDebug() << " only EOM ";
            //qDebug() << serialData;
            //qDebug() << " mid " << serialData.mid(0, eomPosition);
            serialBuffer.append(serialData.mid(0, eomPosition));
            // process bearing to send to display
            serialBuffer = serialBuffer.mid(2, 3);
            qDebug() << "send to display " << serialBuffer;

            if (old_bearing_from_controller != serialBuffer)
            {
                Yaesu::send_to_bearing_display(serialBuffer);
                old_bearing_from_controller = serialBuffer;
            }

            gotSOM = false;
        }
        else if (eomPosition > -1 && somPosition > -1)
        {
            if (eomPosition < somPosition)
            {
                // eom and som in data
               //qDebug() << " both ";
                //qDebug() << serialData;
                //qDebug() << "mid " << serialData.mid(0,messageSize - eomPosition);
                serialBuffer.append(serialData.mid(0,messageSize - eomPosition));
                // process bearing to send to display
                serialBuffer = serialBuffer.mid(2, 3);

                if (old_bearing_from_controller != serialBuffer)
                {
                    Yaesu::send_to_bearing_display(serialBuffer);
                    old_bearing_from_controller = serialBuffer;
                }
                // get som
                serialBuffer.clear();
                serialBuffer.append(serialData.mid(somPosition,messageSize - somPosition));

            }
            else
            {
                // error
                gotSOM = false;
                serialBuffer.clear();
            }
        }
    }
}





void Yaesu::send_to_bearing_display(QByteArray bearing)
{
    bearing_from_controller = QString::fromStdString(bearing.toStdString());
    emit bearing_updated(bearing_from_controller);
}


void Yaesu::rotate_to_bearing(QString bearing)
{
    qDebug() << "got to rotate bearing";

    QString bearing_to_send = bearing;
    QString rotate_to_message = ROT_AZ_TO;
    int n = bearing_to_send.size();
    qDebug() << "size is" << n;
    if (n == 1)
    {
        rotate_to_message.append("00");
    }
    else if (n == 2)
    {
        rotate_to_message.append("0");
    }
    rotate_to_message.append(bearing_to_send);
    rotate_to_message.append(CR);
    // convert to 8 bit for transmission
    tx_rotate_to_message = rotate_to_message.toLatin1();
    qDebug() << "rotate message to serial " << tx_rotate_to_message;
    emit send_rotate_to_msg(tx_rotate_to_message);

}



void Yaesu::set_command_sent(QString command)
{
    command_sent = command;
}

QString Yaesu::get_command_sent()
{
    return command_sent;
}
