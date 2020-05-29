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
#include <QStandardItemModel>
#include <QNetworkInterface>

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

    QStandardItemModel* modelIP = Q_NULLPTR;
    QSqlTableModel *model;

    quint16 m_nNextBlockSize = 0;

    void sendToClient(QTcpSocket* pSocket, const QString& str);

    void UpdateIPTable();
    void InitServer(int nPort);
    void InitClient(const QString& strHost, int nPort);

    void log(bool isServer, QString msg);
private slots:
    void slotReadyRead ( );
    void slotError (QAbstractSocket::SocketError);
    void slotSendToServer( );
    void slotConnected ( );

    void slotNewConnection();
    void slotReadClient();
    void on_pushButton_clicked();

    void on_interfacesTableView_clicked(const QModelIndex &index);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
