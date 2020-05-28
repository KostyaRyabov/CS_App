#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>

class MyClient : public QObject
{
    Q_OBJECT
public:
    explicit MyClient(QObject *parent = nullptr);

signals:

};

#endif // MYCLIENT_H
