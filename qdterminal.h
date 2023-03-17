/***************************************************************************
*   Copyright (C) 2011-2017 by Denis Gottardello                          *
*   info@denisgottardello.it                                              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef QDTERMINAL_H
#define QDTERMINAL_H

#include "QDateTime"
#include "QFileDialog"
#include "QScrollBar"
#include "QSerialPortInfo"
#include "QSettings"
#include "QTimer"
#include "qcsslserver.h"
#include "qdcolors.h"
#include "qdfontdialog.h"
#include "qdopencomport.h"
#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include "ui_qdterminal.h"
#ifndef QDTERMINALLOGFORMATS_H
    class QDTerminal;
    #include "qdterminallogformats.h"
#endif

enum Modes {
    MODE_RS232= 0,
    MODE_TCP_CLIENT= 1,
    MODE_TCP_CLIENT_SSL= 2,
    MODE_TCP_SERVER= 3,
    MODE_TCP_SERVER_SSL= 4,
};

enum TerminalLogFormats {
    TERMINAL_LOG_FORMAT_COMPACT= 1,
    TERMINAL_LOG_FORMAT_FULL= 0,
    TERMINAL_LOG_FORMAT_SEPARATOR= 2,
};

namespace Ui {
    class QDTerminal;
}

class QDTerminal : public QDialog
{
    Q_OBJECT

public:
    explicit QDTerminal(QWidget *parent = nullptr, QString ConnectionPath= "");
    ~QDTerminal();
    Ui::QDTerminal *ui;
    bool SendBreak, SslKeyCertificateEmbedded;
    char Parity;
    int TabNumber, BaudRate, ByteSize, StopBits, FlowControl, MaxClients;
    Modes Mode;
    QDTerminal *pQDTerminal= nullptr;
    QString ComPort, Server, SslKeyPrivate, SslCertificate;
    QTabWidget *pQTBTerminal= nullptr;
    QVector<QDTerminal*> *pQVTerminals= nullptr;
    quint16 Socket;
    void ReadConfigurationFile();
    void SendByteArray(QByteArray QBABufferIn);

private:
    bool IsNewConnection, TimestampPrint= false;
    int RowCount;
    QByteArray QBAByteIn;
    QcSSLServer *pQcSSLServer= nullptr;
    QDateTime QDTLastByteIn;
    QSerialPort SerialPort;
    QSerialPort::PinoutSignals pinoutSignals;
    QString ConnectionPath, DirectoryPath, FontColor, FontColorWarnings;
    QStringList QSLHistory;
    QSslSocket *pQSslSocketClient= nullptr;
    QTcpServer *pQTcpServer= nullptr;
    QTcpSocket *pQTcpSocketClient= nullptr;
    QTimer QTControl;
    QVector<QTcpSocket*> QVTcpSocketsServer;
    QVector<QSslSocket*> QVSslSocketsServer;
    TerminalLogFormats TerminalLogFormat= TERMINAL_LOG_FORMAT_FULL;
    bool eventFilter(QObject *object, QEvent *event);
    void OpenComPort();
    void OpenTcpClientPort();
    void OpenTcpClientSslPort();
    void OpenTcpServerPort();
    void OpenTcpServerSslPort();
    void SaveProfile(QString ConnectionPath);
    void ShowBufferIn(QByteArray &QBABufferIn);
    void TextCursorSet();
    void TimestampPrintEvaluation(QString &BufferIn);

private slots:
    void AcceptError(QAbstractSocket::SocketError socketError);
    void Connected();
    void Disconnected();
    void Error(QAbstractSocket::SocketError socketError);
    void GenericError(QString Description);
    void on_QCBAutoScroll_toggled(bool checked);
    void on_QCBRowCount_toggled(bool checked);
    void on_QCBSpecialCharacters_toggled(bool checked);
    void on_QCBTimestampAfterCR_toggled(bool checked);
    void on_QCBTimestampAfterLF_toggled(bool checked);
    void on_QCBTimestampAuto_toggled(bool checked);
    void on_QGBNewLineAfter_toggled(bool arg1);
    void on_QGBTimestamp_toggled(bool arg1);
    void on_QLESend_returnPressed();
    void on_QPBChangeFont_clicked();
    void on_QPBClear_clicked();
    void on_QPBClose_clicked();
    void on_QPBCloseProfile_clicked();
    void on_QPBColors_clicked();
    void on_QPBCopy_clicked();
    void on_QPBDTR_clicked();
    void on_QPBModify_clicked();
    void on_QPBOpen_clicked();
    void on_QPBRTS_clicked();
    void on_QRBBin_clicked();
    void on_QRBDec_clicked();
    void on_QRBHex_clicked();
    void on_QRBPrintableOnly_clicked();
    void on_QRBPrintableOnly_toggled(bool checked);
    void on_QPBSaveProfile_clicked();
    void on_QPBSaveProfileAs_clicked();
    void on_QPBSaveToFile_clicked();
    void on_QPBSelectAll_clicked();
    void on_QPBSend_clicked();
    void on_QPBSendFile_clicked();
    void on_QPBSendHistory_clicked();
    void on_QRBSym_clicked();
    void on_QSBNewLineAfterMs_valueChanged(int arg1);
    void on_QTBTerminalLogFormats_clicked();
    void OnNewConnection();
    void OnTimeout();
    void PeerVerifyError(const QSslError &error);
    void ReadyRead();
    void SslErrors(const QList<QSslError> &errors);
};

#endif // QDTERMINAL_H
