#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>
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

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

private:
    QTcpServer *tcpServer;

    QList<QTcpSocket*> clients;

    QMap<QString, QPair<QString, quint16>> clientConnections;
    QString receivername;

    QString receiverip;
    quint16 receiverport;
    QString receiverMessage;

    QString removeName;


    QString serverIpaddr;
    QString serverPort;

    Ui::Server *ui;

};
#endif // SERVER_H
