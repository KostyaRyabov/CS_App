#ifndef _MyClient_h_
#define _MyClient_h_

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>

class QTextEdit;
class QLineEdit;

class MyTcpClient : public QWidget {
    Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit* m_ptxtInfo;
    QLineEdit* m_ptxtInput;
    quint16 m_nNextBlockSize;
public:
    MyTcpClient(const QString& strHost, int nPort, QWidget* pwgt = 0) ;
private slots:
    void slotReadyRead ( );
    void slotError (QAbstractSocket::SocketError);
    void slotSendToServer( );
    void slotConnected ( );
};
#endif //_MyClient_h_
