#include "qcsslserver.h"

QcSSLServer::QcSSLServer(QObject *parent) : QTcpServer(parent) {}

void QcSSLServer::incomingConnection(qintptr socketDescriptor) {
    QSslSocket *pQSslSocket= new QSslSocket(this);
    pQSslSocket->setProtocol(QSsl::TlsV1_2);
    QFile FileIn(":/certs/certs/sslserver.key");
    if (FileIn.open(QIODevice::ReadOnly)) {
        QSslKey SslKey(FileIn.readAll(), QSsl::Rsa);
        FileIn.close();
        pQSslSocket->setPrivateKey(SslKey);
        FileIn.setFileName(":/certs/certs/sslserver.pem");
        if (FileIn.open(QIODevice::ReadOnly)) {
            QSslCertificate SslLocalCertificate(FileIn.readAll());
            FileIn.close();
            pQSslSocket->setLocalCertificate(SslLocalCertificate);
            if (pQSslSocket->setSocketDescriptor(socketDescriptor)) {
                pQSslSocket->startServerEncryption();
                if (!pQSslSocket->isEncrypted()) {
                    if (pQSslSocket->waitForEncrypted()) {
                        this->addPendingConnection(pQSslSocket);
                    } else delete pQSslSocket;
                }
            } else delete pQSslSocket;
        } else {
            qDebug() << FileIn.errorString();
        }
    } else {
        qDebug() << FileIn.errorString();
    }
}
