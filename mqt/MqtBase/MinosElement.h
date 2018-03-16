#ifndef MINOSELEMENT_H
#define MINOSELEMENT_H
#include <QString>

class MinosElement
{
    QString _server;
    QString _appName;
    QString _element;

public:
    MinosElement();
    MinosElement(QString s);
    MinosElement(QString ser, QString an, QString ele):_server(ser), _appName(an), _element(ele){}
    ~MinosElement();
    QString server() const;
    QString appName() const;
    QString element() const;
};

#endif // MINOSELEMENT_H
