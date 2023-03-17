#include "qcsslserver.h"

QcSSLServer::QcSSLServer(QObject *parent) : QTcpServer(parent) {}

void QcSSLServer::incomingConnection(qintptr socketDescriptor) {
    QSslSocket *pQSslSocket= new QSslSocket(this);
    pQSslSocket->setProtocol(QSsl::TlsV1_2);
    QFile FileIn(SslKeyCertificateEmbedded ? ":/certs/certs/KeyPrivate.key" : SslKeyPrivate);
    if (FileIn.open(QIODevice::ReadOnly)) {
        QSslKey SslKey(FileIn.readAll(), QSsl::Rsa);
        FileIn.close();
        pQSslSocket->setPrivateKey(SslKey);
        FileIn.setFileName(SslKeyCertificateEmbedded ? ":/certs/certs/Certificate.crt" : SslCertificate);
        if (FileIn.open(QIODevice::ReadOnly)) {
            QSslCertificate SslLocalCertificate(FileIn.readAll());
            FileIn.close();
            pQSslSocket->setLocalCertificate(SslLocalCertificate);
            if (pQSslSocket->setSocketDescriptor(socketDescriptor)) {
                pQSslSocket->startServerEncryption();
                if (!pQSslSocket->isEncrypted()) {
                    if (pQSslSocket->waitForEncrypted(10000)) {
                        this->addPendingConnection(pQSslSocket);
                    } else {
                        emit GenericError(pQSslSocket->errorString());
                        delete pQSslSocket;
                    }
                }
            } else delete pQSslSocket;
        } else emit GenericError(FileIn.errorString());
    } else emit GenericError(FileIn.errorString());
}
