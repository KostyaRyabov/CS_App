#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>


#include <QTcpServer>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QObject>

#include <QTcpSocket>
#include <QDataStream>
#include <QTime>

#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QSqlQuery query;

    QTcpSocket* m_pTcpSocket;
    QTcpServer* m_ptcpServer;

    QSqlDatabase db;
    QSqlTableModel *model;

    quint16 m_nNextBlockSize = 0;

    void sendToClient(QTcpSocket* pSocket, const QString& str);

    void InitServer(int nPort);
    void InitClient(const QString& strHost, int nPort);
private slots:
    void slotReadyRead ( );
    void slotError (QAbstractSocket::SocketError);
    void slotSendToServer( );
    void slotConnected ( );

    void slotNewConnection();
    void slotReadClient();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
