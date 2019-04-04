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

#include <QDialog>
#include "QSettings"
#include "QScrollBar"
#include "QTimer"
#include "QFileDialog"
#include "qdfontdialog.h"
#include "qdopencomport.h"
#include "ui_qdterminal.h"
#include "qdcolors.h"
#include "QTcpSocket"
#include <QtSerialPort/QSerialPort>
#include "QSerialPortInfo"
#include "QDateTime"

const static int MODERS232= 0;
const static int MODETCPIP= 1;

namespace Ui {
    class QDTerminal;
}

class QDTerminal : public QDialog
{
    Q_OBJECT

public:
    explicit QDTerminal(QWidget *parent = 0, QString ConnectionPath= "");
    ~QDTerminal();
    Ui::QDTerminal *ui;
    bool SendBreak;
    int TabNumber, BaudRate, ByteSize, StopBits, FlowControl, Mode, Socket;
    char Parity;
    QDTerminal *QdTerminal;
    QString ComPort, Server;
    QTabWidget *QTBTerminal;
    QVector <QDTerminal*>*QVTerminals;
    void ReadConfigurationFile();
    void SendByteArray(QByteArray QBABufferIn);

private:
    bool IsNewConnection;
    int PinoutSignals, RowCount;
    QByteArray QBAByteIn;
    QDateTime QDTLastByteIn;
    QSerialPort SerialPort;
    QString ConnectionPath, DirectoryPath;
    QTcpSocket *Tcp;
    QTimer *QTControl;
    bool eventFilter(QObject *object, QEvent *event);
    void OpenComPort();
    void OpenTcpPort();
    void SaveProfile(QString ConnectionPath);
    void ShowBufferIn(QByteArray &QBABufferIn);

private slots:
    void Connected();
    void Disconnected();
    void Error(QAbstractSocket::SocketError socketError);
    void on_QCBNewLineAfter_clicked();
    void on_QCBRowCount_toggled(bool checked);
    void on_QCBTimestamp_toggled(bool checked);
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
    void on_QPBSaveProfile_clicked();
    void on_QPBSaveProfileAs_clicked();
    void on_QPBSaveToFile_clicked();
    void on_QPBSend_clicked();
    void on_QPBSendFile_clicked();
    void on_QRBBin_clicked();
    void on_QRBDec_clicked();
    void on_QRBHex_clicked();
    void on_QRBPrintableOnly_clicked();
    void on_QPBSelectAll_clicked();
    void on_QRBSym_clicked();
    void on_QSBNewLineAfterMs_valueChanged(int arg1);
    void OnTimeout();
    void ReadyRead();

};

#endif // QDTERMINAL_H
