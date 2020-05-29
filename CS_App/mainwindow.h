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
    QSqlTableModel *model;

    quint16 m_nNextBlockSize = 0;

    void sendMessage(const QString& str);
    void slotSendToServer();

    void InitServer(int nPort);
    void InitClient();

    void log(bool isServer, QString msg);
private slots:
    void slotReadServer();
    void slotReadClient();
    void slotError (QAbstractSocket::SocketError);
    void slotConnected();

    void slotNewConnection();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
