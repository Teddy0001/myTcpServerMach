#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();



protected:
    //void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void receiveMessage();
    void disconnectClient();
    void mNewConnection();

private:
    QTcpServer *tcpServer;

    QList<QTcpSocket*> clients;

    QMap<QString, QPair<QString, quint16>> clientConnections;
    QString receivername;

    QString receiverip;
    quint16 receiverport;
    QString receiverMessage;

    QString removeName;

    Ui::Server *ui;

};
#endif // SERVER_H
