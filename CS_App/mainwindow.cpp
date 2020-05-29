#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(QSqlDatabase::isDriverAvailable("QSQLITE")){
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(":memory:");

        db.open();
        query = QSqlQuery("CREATE TABLE dialog (id DateTime PRIMARY KEY, side TEXT, msg TEXT)");
    }


    model = new QSqlTableModel(this,db);

    model->setTable("dialog");
    ui->tableView->setModel(model);
    ui->tableView->show();

    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->setColumnWidth(0,110);
    ui->tableView->setColumnWidth(1,10);
    ui->tableView->setColumnWidth(2,350);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setShowGrid(false);
    model->select();

    InitServer(2323);
    InitClient();
}

MainWindow::~MainWindow()
{
    db.close();

    delete m_pTcpSocket;
    delete m_ptcpServer;
    delete model;
    delete ui;
}

void MainWindow::log(bool isServer, QString msg){
    query.prepare("INSERT INTO dialog(id,side,msg) VALUES(:datetime, :side,:message)");
    query.bindValue(":datetime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":side", (isServer)?" S:":" C:");
    query.bindValue(":message", msg);
    query.exec();

    model->select();
}

void MainWindow::on_pushButton_clicked()
{
    log(true,ui->textEdit->toPlainText());
    //slotSendToServer();

    sendMessage(ui->textEdit->toPlainText());
    ui->textEdit->clear();
}

// SERVER

void MainWindow::InitServer(int nPort)
{
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0,"Server Error","Unable to start the server:"+ m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void MainWindow::slotNewConnection()
{
    m_pTcpSocket = m_ptcpServer->nextPendingConnection();
    connect(m_pTcpSocket, SIGNAL(disconnected()),m_pTcpSocket, SLOT(deleteLater()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    qDebug() << "       new connection";

    ui->selectedIP->setText(m_pTcpSocket->localAddress().toString());

    sendMessage("[Server Response: Connected!]");
}

void MainWindow::sendMessage(const QString& str)
{
    qDebug() << "       send message";

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
}


// CLIENT

void MainWindow::InitClient()
{
    m_pTcpSocket = new QTcpSocket(this);

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
}

void MainWindow::slotConnected()
{
    log(false,"[Received the connected() signal]");
    sendMessage("[Server Response: Connected!]");
}

void MainWindow::slotReadyRead()
{
    qDebug() << "           ready read";

    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_2);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }
        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QString str;
        in >> str;

        log(false,str);

        m_nNextBlockSize = 0;

        sendMessage("[Server Response: Message is Received]");
    }
}

void MainWindow::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
    "Error: " + (err == QAbstractSocket::HostNotFoundError ?
    "The host was not found." :
    err == QAbstractSocket::RemoteHostClosedError ?
    "The remote host is closed." :
    err == QAbstractSocket::ConnectionRefusedError ?
    "The connection was refused." :
    QString(m_pTcpSocket->errorString()));
    //m_ptxtInfo->append(strError);

    log(true,'['+strError+']');
}

void MainWindow::on_pushButton_2_clicked()
{
    m_pTcpSocket->connectToHost(ui->selectedIP->toPlainText(), 2323);
}
