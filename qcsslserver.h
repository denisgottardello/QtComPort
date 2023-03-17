#ifndef QCSSLSERVER_H
#define QCSSLSERVER_H

#include "QSslKey"
#include "QSslSocket"
#include "QTcpServer"
#include "QFile"

class QcSSLServer : public QTcpServer
{
    Q_OBJECT

public:
    QcSSLServer(QObject *parent = nullptr);
    bool SslKeyCertificateEmbedded;
    QString SslKeyPrivate, SslCertificate;

signals:
    void GenericError(QString Description);

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // QCSSLSERVER_H
