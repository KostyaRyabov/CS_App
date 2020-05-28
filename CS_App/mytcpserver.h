#ifndef _MyServer_h_
#define _MyServer_h_

#include <QWidget>

#include <QTcpServer>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QObject>

#include <QTcpSocket>
#include <QDataStream>
#include <QTime>

class QTcpServer;
class QTextEdit;
class QTcpSocket;

class MyTcpServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    MyTcpServer(int nPort, QWidget* pwgt = 0);

public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
};
#endif  //_MyServer_h_
