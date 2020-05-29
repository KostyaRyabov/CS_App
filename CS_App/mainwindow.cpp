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
    ui->tableView->setColumnWidth(2,450);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setShowGrid(false);
    model->select();

    InitServer(2323);
}

MainWindow::~MainWindow()
{
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
    //slotSendToServer();

    log(true,ui->textEdit->toPlainText());

    sendToClient(m_pTcpSocket, ui->textEdit->toPlainText());

    ui->textEdit->clear();
}

// SERVER

void MainWindow::InitServer(int nPort)
{
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + m_ptcpServer->errorString()
                             );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    /*
    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);
    */
}

void MainWindow::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    sendToClient(pClientSocket, "Server Response: Connected!");

    log(false, "[new connection]");
}

void MainWindow::slotReadClient()
{
    qDebug() << "slotReadClient";

    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_2);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime   time;
        QString str;
        in >> time >> str;

        QString strMessage =
            time.toString() + " " + "Client has sended - " + str;
        //m_ptxt->append(strMessage);

        log(false,strMessage);

        m_nNextBlockSize = 0;

        sendToClient(pClientSocket, "Server Response: Received \"" + str + "\"");

        log(true,"Server Response: Received \"" + str + "\"");
    }
}


void MainWindow::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}


// CLIENT

void MainWindow::InitClient(const QString& strHost, int nPort)
{
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(strHost, nPort);

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

    //m_ptxtInfo = new QTextEdit;
    //m_ptxtInput = new QLineEdit;

    //connect(m_ptxtInput, SIGNAL(returnPressed()), this, SLOT(slotSendToServer()));

    //m_ptxtInfo->setReadOnly(true);

    //QPushButton* pcmd = new QPushButton("&Send");

    //connect(this, SIGNAL(on_pushButton_clicked()), SLOT(slotSendToServer()));

    /*
    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;

    pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
    pvbxLayout->addWidget(m_ptxtInfo);
    pvbxLayout->addWidget(m_ptxtInput);
    pvbxLayout->addWidget(pcmd);

    setLayout(pvbxLayout);
    */
}

void MainWindow::slotReadyRead()
{
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
    QTime time;
    QString str;
    in >> time >> str;
    //m_ptxtInfo->append(time.toString() + " " + str);
    qDebug() << time.toString() + " " + str;

    log(false,str);

    m_nNextBlockSize = 0;
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

    qDebug() << strError;

    log(true,'['+strError+']');
}

void MainWindow::slotSendToServer()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);
    //out << quint16(0) << QTime::currentTime() << m_ptxtInput->text();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
    //m_ptxtInput->setText("");
}



void MainWindow::slotConnected()
{
   // m_ptxtInfo->append("Received the connected() signal");

    log(true,"[Received the connected() signal]");

    qDebug() << "Received the connected() signal";
}


// SQL



void MainWindow::on_pushButton_2_clicked()
{
    InitClient(ui->selectedIP->toPlainText(), 2323);
}
