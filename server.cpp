#include "server.h"
#include "ui_server.h"
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QString>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);


    //新的连接
    tcpServer = new QTcpServer(this);

    this->setWindowTitle("服务端");

    //listen(QHostAddress::Any, 12345); // Listen to any address on port 12345
    tcpServer->listen(QHostAddress("10.6.22.1"), 9999);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(mNewConnection()));
}

Server::~Server()
{
    delete ui;
}

void Server::mNewConnection()
{
    //与客户端连接
    QTcpSocket *tmpTcpSocket = tcpServer->nextPendingConnection();

//    QTcpSocket *client = new QTcpSocket(this);
//    client->setSocketDescriptor(socketDescriptor);

    connect(tmpTcpSocket, &QTcpSocket::readyRead, this, &Server::receiveMessage);
    connect(tmpTcpSocket, &QTcpSocket::disconnected, this, &Server::disconnectClient);

    clients.append(tmpTcpSocket);

    qDebug() << "Client connected. Total clients: " << clients.count();
}




//void Server::incomingConnection(qintptr socketDescriptor)
//{
//    QTcpSocket *client = new QTcpSocket(this);
//    client->setSocketDescriptor(socketDescriptor);

//    connect(client, &QTcpSocket::readyRead, this, &Server::receiveMessage);
//    connect(client, &QTcpSocket::disconnected, this, &Server::disconnectClient);

//    clients.append(client);

//    qDebug() << "Client connected. Total clients: " << clients.count();
//}

void Server::receiveMessage()
{
    QTcpSocket *tmpTcpSocket = (QTcpSocket *)sender();


    QByteArray message = tmpTcpSocket->readAll();
    QString receivedString = QString::fromUtf8(message);

    if (message.startsWith("USERNAME:")) {
        QString newusername = QString::fromUtf8(message.mid(9)); // 跳过 "USERNAME:" 前缀
        QString ip = tmpTcpSocket->peerAddress().toString();
        quint16 port = tmpTcpSocket->peerPort();
        clientConnections[newusername] = qMakePair(ip, port);
        // 在这里处理用户名、IP地址和端口号

        ui->textBrowser->append("新连接：Username: "+ newusername + ", IP: " + ip + ", Port: " + QString::number(port));


        // 清空 QComboBox 中的所有项
        ui->comboBox->clear();
        //遍历QMap
        QMap<QString, QPair<QString, quint16>>::iterator it;
        /*把clients所有客户遍历出来*/
        for (it = clientConnections.begin(); it != clientConnections.end(); ++it)
        {
            QString username = it.key();
            QPair<QString, quint16> connectionInfo = it.value();
            QString ip = connectionInfo.first;
            quint16 port = connectionInfo.second;

//        // 在这里处理用户名、IP地址和端口号
            qDebug() << "服务器连接的Username: " << username << ", IP: " << ip << ", Port: " << port;

            ui->comboBox->addItem(username);

            // Forward the message to all other clients
            for (QTcpSocket *client : clients) {
                //QTcpSocket *client = clients[i];
                QString message = "Username: " + username + ", IP: " + ip + ", Port: " + QString::number(port);
                QByteArray byteArray = message.toUtf8();
                client->write(byteArray);
                client->flush();
                qDebug() << "服务器向客户端:"<< client->peerAddress().toString() <<"发送添加receiver的message:"<< byteArray <<endl;
            }
        }
    } else if (message.startsWith("Rereiver:")) {
        QStringList parts0 = receivedString.split(',');

        //分离出消息
        if (parts0.size() >= 2) {
            receiverMessage = parts0[1].trimmed(); // trimmed() 去除字符串前后的空白
        }

        // 分离出目标发送者的名字Username
        QString temReceiverName = parts0[0].trimmed();
        QStringList parts = temReceiverName.split(':');
        if (parts.size() >= 2) {
            receivername = parts[1].trimmed(); // trimmed() 去除字符串前后的空白
        }
        qDebug() << "receivername: " << receivername ;

        QString usernameToFind = receivername;
        QMap<QString, QPair<QString, quint16>>::iterator it = clientConnections.find(usernameToFind);

        if (it != clientConnections.end()) {
            // 找到了对应的条目
            QPair<QString, quint16> connectionInfo = it.value();
            receiverip = connectionInfo.first;
            receiverport = connectionInfo.second;
            // 现在你可以使用 ip 和 port 变量
            qDebug() << "User: " << usernameToFind << " IP: " << receiverip << " Port: " << receiverport;
            for (QTcpSocket *client : clients) {
                QString message = receiverMessage;
                if(client->peerAddress().toString() == receiverip) {
                    client->write(message.toUtf8());
                }
            }

        } else {
            // 没有找到对应的条目
            qDebug() << "User not found: " << usernameToFind;
        }

    }


//    // Forward the message to all other clients
//    for (QTcpSocket *client : clients) {
//        if (client != tmpTcpSocket) {
//            client->write(message);
//        }
//    }
}

void Server::disconnectClient()
{
    QTcpSocket *disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
    if (!disconnectedSocket)
        return;

    clients.removeAll(disconnectedSocket);



    QString removeUsrAddr = disconnectedSocket->peerAddress().toString();
    // 遍历 clientConnections 并移除拥有相同IP地址的 newusername 用户
    QMap<QString, QPair<QString, quint16>>::iterator it;
    for (it = clientConnections.begin(); it != clientConnections.end(); ) {
        removeName = it.key();
        QPair<QString, quint16> connectionInfo = it.value();
        QString ip = connectionInfo.first;

        // 检查IP地址是否与 removeUsrAddr 相同
        if (ip == removeUsrAddr) {
            // 移除匹配的键值对
            it = clientConnections.erase(it);
        } else {
            // 移动到下一个键值对
            ++it;
        }
    }

    // 清空 QComboBox 中的所有项
    ui->comboBox->clear();

    // Forward the message to all other clients
    for (QTcpSocket *client : clients) {
        QString ip = removeUsrAddr;
        quint16 port = disconnectedSocket->peerPort();

        QString message = "Removename: " + removeName + ", IP: " + ip + ", Port: " + QString::number(port);
        qDebug() << message <<endl;
        QByteArray byteArray = message.toUtf8();
        client->write(byteArray);
        client->flush();
        qDebug() << "服务器向客户端:"<< client->peerAddress().toString() <<"发送添加receiver的message:"<< byteArray <<endl;
    }

    //遍历QMap
    QMap<QString, QPair<QString, quint16>>::iterator it2;
    for (it2 = clientConnections.begin(); it2 != clientConnections.end(); ++it2)
    {
        QString username = it2.key();
        QPair<QString, quint16> connectionInfo = it2.value();
        QString ip = connectionInfo.first;
        quint16 port = connectionInfo.second;

        //        // 在这里处理用户名、IP地址和端口号
        qDebug() << "服务器连接的Username: " << username << ", IP: " << ip << ", Port: " << port;

        ui->comboBox->addItem(username);

    }



    disconnectedSocket->deleteLater();

    qDebug() << "Client disconnected. Total clients: " << clients.count();
}
