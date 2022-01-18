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

#include "qdterminal.h"
#include "ui_qdterminal.h"

QDTerminal::QDTerminal(QWidget *parent, QString ConnectionPath) : QDialog(parent), ui(new Ui::QDTerminal) {
    ui->setupUi(this);
    this->ConnectionPath= ConnectionPath;
    DirectoryPath= "";
    ui->QLCTS->setStyleSheet("background-color: green; color: black");
    ui->QLDCD->setStyleSheet("background-color: green; color: black");
    ui->QLDSR->setStyleSheet("background-color: green; color: black");
    FontColor= "#ffffff";
    FontColorWarnings= "#ffff00";
    QPalette Palette= ui->QPTELog->palette();
    Palette.setColor(QPalette::Base, QColor(0, 0, 0));
    Palette.setColor(QPalette::Text, QColor(FontColor));
    ui->QPTELog->setPalette(Palette);
    QFont Font("DejaVu Sans Mono", 8);
    Font.setFixedPitch(true);
    ui->QPTELog->setFont(Font);
    QFontMetrics FontMetrics(ui->QPTELog->font());
    ui->QPTELog->setTabStopDistance(2* FontMetrics.horizontalAdvance(' '));
    SslSocketClient= new QSslSocket();
    TcpServer= new QTcpServer();
    TcpSocketClient= new QTcpSocket();
    connect(TcpServer, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
    QDTLastByteIn= QDateTime::currentDateTime();
    RowCount= 0;
    on_QRBPrintableOnly_toggled(ui->QRBPrintableOnly->isChecked());
}

QDTerminal::~QDTerminal() {
    delete SslSocketClient;
    for (int count= 0; count< QVTcpSocketsServer.length(); count++) delete QVTcpSocketsServer.at(count);
    QVTcpSocketsServer.clear();
    delete TcpServer;
    delete TcpSocketClient;
    delete ui;
}

bool QDTerminal::eventFilter(QObject *object, QEvent *event) {
    if (object== ui->QPTELog && event->type()== QEvent::KeyPress) {
        QKeyEvent *keyEvent= static_cast<QKeyEvent*>(event);
        QString KeyPressed;
        switch(keyEvent->key()) {
            case Qt::Key_F1: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x3b); break; }
            case Qt::Key_F2: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x3c); break; }
            case Qt::Key_F3: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x3d); break; }
            case Qt::Key_F4: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x3e); break; }
            case Qt::Key_F5: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x3f); break; }
            case Qt::Key_F6: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x40); break; }
            case Qt::Key_F7: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x41); break; }
            case Qt::Key_F8: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x42); break; }
            case Qt::Key_F9: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x43); break; }
            case Qt::Key_F10: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x44); break; }
            case Qt::Key_F11: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x85); break; }
            case Qt::Key_F12: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x86); break; }
            case Qt::Key_Pause: { KeyPressed= QChar(0x0); KeyPressed+= QChar(0x0); break; }
            case Qt::Key_Return:
            case Qt::Key_Enter: KeyPressed= '\r'; break;
            default: KeyPressed= keyEvent->text(); break;
        }
        switch(Mode) {
            case MODE_RS232: SerialPort.write(KeyPressed.toLatin1()); break;
            case MODE_TCP_CLIENT: {
                if (TcpSocketClient) TcpSocketClient->write(KeyPressed.toLatin1());
                break;
            }
            case MODE_TCP_CLIENT_SSL: SslSocketClient->write(KeyPressed.toLatin1()); break;
            case MODE_TCP_SERVER: {
                for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(KeyPressed.toLatin1());
                break;
            }
        }
        return true;
    } else return QObject::eventFilter(object, event);
}

void QDTerminal::Connected() {
    TextCursorSet();
    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connected")+ "<br></font>");
    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
}

void QDTerminal::Disconnected() {
    switch(Mode) {
        case MODE_TCP_CLIENT:
        case MODE_TCP_CLIENT_SSL: {
            ui->QPBClose->click();
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) {
                if (QVTcpSocketsServer.at(count)== static_cast<QTcpSocket *>(QObject::sender())) {
                    TextCursorSet();
                    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Disconnected")+ "<br></font>");
                    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
                    QVTcpSocketsServer.removeAt(count);
                    break;
                }
            }
            break;
        }
        default: break;
    }
}

void QDTerminal::Error(QAbstractSocket::SocketError ) {
    TextCursorSet();
    switch(Mode) {
        case MODE_TCP_CLIENT: {
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ TcpSocketClient->errorString()+ "<br></font>");
            ui->QPBClose->click();
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ SslSocketClient->errorString()+ "<br></font>");
            ui->QPBClose->click();
            break;
        }
        case MODE_TCP_SERVER: {
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ static_cast<QTcpSocket *>(QObject::sender())->errorString()+ "<br></font>");
            break;
        }
        default: break;
    }
    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
}

void QDTerminal::OnNewConnection() {
    QVTcpSocketsServer.append(TcpServer->nextPendingConnection());
    TextCursorSet();
    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connection from")+ " "+ QVTcpSocketsServer.last()->peerAddress().toString()+ ":"+ QString::number(QVTcpSocketsServer.last()->peerPort())+ "<br></font>");
    connect(QVTcpSocketsServer.last(), SIGNAL(readyRead()), this, SLOT(ReadyRead()));
    connect(QVTcpSocketsServer.last(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
    connect(QVTcpSocketsServer.last(), SIGNAL(disconnected()), this, SLOT(Disconnected()));
    TextCursorSet();
    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connected")+ "<br></font>");
    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
}

void QDTerminal::OnTimeout() {
    QTControl->stop();
    if ((SerialPort.pinoutSignals() & QSerialPort::ClearToSendSignal)!= (pinoutSignals & QSerialPort::ClearToSendSignal)) {
        if (SerialPort.pinoutSignals() & QSerialPort::ClearToSendSignal) ui->QLCTS->setStyleSheet("background-color: red; color: black");
        else ui->QLCTS->setStyleSheet("background-color: green; color: black");
        pinoutSignals= SerialPort.pinoutSignals();
    }
    if ((SerialPort.pinoutSignals() & QSerialPort::DataCarrierDetectSignal)!= (pinoutSignals & QSerialPort::DataCarrierDetectSignal)) {
        if (SerialPort.pinoutSignals() & QSerialPort::DataCarrierDetectSignal) ui->QLDCD->setStyleSheet("background-color: red; color: black");
        else ui->QLDCD->setStyleSheet("background-color: green; color: black");
        pinoutSignals= SerialPort.pinoutSignals();
    }
    if ((SerialPort.pinoutSignals() & QSerialPort::DataSetReadySignal)!= (pinoutSignals & QSerialPort::DataSetReadySignal)) {
        if (SerialPort.pinoutSignals() & QSerialPort::DataSetReadySignal) ui->QLDSR->setStyleSheet("background-color: red; color: black");
        else ui->QLDSR->setStyleSheet("background-color: green; color: black");
        pinoutSignals= SerialPort.pinoutSignals();
    }
    QByteArray QBABufferIn= SerialPort.readAll();
    if (QBABufferIn.size()> 0) {
        if (pQDTerminal) pQDTerminal->SendByteArray(QBABufferIn);
        ShowBufferIn(QBABufferIn);
        QBAByteIn.append(QBABufferIn);
    }
    QTControl->start(10);
}

void QDTerminal::on_QCBRowCount_toggled(bool ) {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QCBSpecialCharacters_clicked() {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QCBTimestamp_toggled(bool ) {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QCBNewLineAfter_clicked() {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QLESend_returnPressed() {
    switch(Mode) {
        case MODE_RS232: {
            if (SerialPort.write(QString(ui->QLESend->text()+ "\r\n").toLatin1())) ui->QLESend->setText("");
            break;
        }
        case MODE_TCP_CLIENT: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            TcpSocketClient->write(ui->QLESend->text().toLatin1());
            TcpSocketClient->write(QString("\r\n").toLatin1());
            ui->QLESend->setText("");
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            SslSocketClient->write(ui->QLESend->text().toLatin1());
            SslSocketClient->write(QString("\r\n").toLatin1());
            ui->QLESend->setText("");
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(ui->QLESend->text().toLatin1());
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(QString("\r\n").toLatin1());
            ui->QLESend->setText("");
            break;
        }
    }
}

void QDTerminal::on_QPBCloseProfile_clicked() {
    for (int count= 0; count< QVTerminals->count(); count++) {
        if (QVTerminals->value(count)== this) {
            QVTerminals->remove(count);
            break;
        }
    }
    QTBTerminal->removeTab(TabNumber);
    delete this;
}

void QDTerminal::on_QPBChangeFont_clicked() {
    QDFontDialog QdFontDialog(this);
    QdFontDialog.SetCurrentFont(ui->QPTELog->font());
    if (QdFontDialog.exec()== QDialog::Accepted) {
        ui->QPTELog->setFont(QdFontDialog.GetCurrentFont());
        ui->QPBSaveProfile->setEnabled(true);
        QFontMetrics FontMetrics(ui->QPTELog->font());
        ui->QPTELog->setTabStopDistance(2* FontMetrics.horizontalAdvance(' '));
    }
}

void QDTerminal::on_QPBClear_clicked() {
    ui->QPTELog->clear();
    QBAByteIn.clear();
    RowCount= 1;
}

void QDTerminal::on_QPBClose_clicked() {
    ui->QPBClose->setEnabled(false);
    ui->QPBSendFile->setEnabled(false);
    ui->QPTELog->removeEventFilter(this);
    ui->QLConnection->setText("");
    switch(Mode) {
        case MODE_RS232: {
            disconnect(QTControl, SIGNAL(timeout()), this, SLOT(OnTimeout()));
            delete QTControl;
            ui->QPBRTS->setEnabled(false);
            ui->QPBDTR->setEnabled(false);
            SerialPort.close();
            ui->QPBRTS->setStyleSheet("");
            ui->QPBDTR->setStyleSheet("");
            ui->QLCTS->setStyleSheet("");
            ui->QLDCD->setStyleSheet("");
            ui->QLDSR->setStyleSheet("");
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) delete QVTcpSocketsServer.at(count);
            QVTcpSocketsServer.clear();
            TcpServer->close();
            break;
        }
        case MODE_TCP_CLIENT: {
            disconnect(TcpSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
            disconnect(TcpSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
            disconnect(TcpSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
            disconnect(TcpSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
            TcpSocketClient->disconnectFromHost();
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            disconnect(SslSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
            disconnect(SslSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
            disconnect(SslSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
            disconnect(SslSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
            SslSocketClient->disconnectFromHost();
            break;
        }
    }
    ui->QPBOpen->setEnabled(true);
    ui->QPBModify->setEnabled(true);
}

void QDTerminal::on_QPBDTR_clicked() {
    if (ui->QPBDTR->isChecked()) ui->QPBDTR->setStyleSheet("background-color: red; color: black");
    else ui->QPBDTR->setStyleSheet("background-color: green; color: black");
    SerialPort.setDataTerminalReady(ui->QPBDTR->isChecked());
}

void QDTerminal::on_QPBModify_clicked() {
    QDOpenComPort _OpenComPort(this);
    foreach (const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts()) _OpenComPort.ui->QCBComPort->addItem(SerialPortInfo.portName());
    for (int count= 0; count< _OpenComPort.ui->QCBComPort->count(); count++) {
        if (_OpenComPort.ui->QCBComPort->itemText(count).compare(ComPort, Qt::CaseInsensitive)== 0) {
            _OpenComPort.ui->QCBComPort->setCurrentIndex(count);
            break;
        }
    }
    for (int count= 0; count< _OpenComPort.ui->QCBBaudRate->count(); count++) {
        if (_OpenComPort.ui->QCBBaudRate->itemText(count).compare(QString::number(BaudRate), Qt::CaseInsensitive)== 0) {
            _OpenComPort.ui->QCBBaudRate->setCurrentIndex(count);
            break;
        }
    }
    switch(Parity) {
        case 'N': _OpenComPort.ui->QCBParity->setCurrentIndex(0); break;
        case 'E': _OpenComPort.ui->QCBParity->setCurrentIndex(1); break;
        case 'O': _OpenComPort.ui->QCBParity->setCurrentIndex(2); break;
        case 'M': _OpenComPort.ui->QCBParity->setCurrentIndex(3); break;
        case 'S': _OpenComPort.ui->QCBParity->setCurrentIndex(4); break;
    }
    switch(ByteSize) {
        case 5: _OpenComPort.ui->QCBDataBits->setCurrentIndex(0); break;
        case 6: _OpenComPort.ui->QCBDataBits->setCurrentIndex(1); break;
        case 7: _OpenComPort.ui->QCBDataBits->setCurrentIndex(2); break;
        case 8: _OpenComPort.ui->QCBDataBits->setCurrentIndex(3); break;
    }
    switch(StopBits) {
        case 0: _OpenComPort.ui->QCBStopBits->setCurrentIndex(0); break;
        case 1: _OpenComPort.ui->QCBStopBits->setCurrentIndex(1); break;
        case 2: _OpenComPort.ui->QCBStopBits->setCurrentIndex(2); break;
    }
    _OpenComPort.ui->QCBFlowControl->setCurrentIndex(FlowControl);
    _OpenComPort.ui->QCBSendBreak->setChecked(SendBreak);
    _OpenComPort.ui->QLEServer->setText(Server);
    _OpenComPort.ui->QSBSocket->setValue(Socket);
    _OpenComPort.ui->QSBTCPServerMaxClients->setValue(MaxClients);
    switch(Mode) {
        case MODE_RS232: _OpenComPort.ui->QRBRS232->setChecked(true); break;
        case MODE_TCP_SERVER: _OpenComPort.ui->QRBTCPServer->setChecked(true); break;
        case MODE_TCP_CLIENT: _OpenComPort.ui->QRBTCPClient->setChecked(true); break;
        case MODE_TCP_CLIENT_SSL: _OpenComPort.ui->QRBTCPSsl->setChecked(true); break;
    }
    if (_OpenComPort.exec()== QDialog::Accepted) {
        switch(_OpenComPort.ui->QCBParity->currentIndex()) {
            case 0: Parity= 'N'; break;
            case 1: Parity= 'E'; break;
            case 2: Parity= 'O'; break;
            case 3: Parity= 'M'; break;
            case 4: Parity= 'S'; break;
        }
        switch(_OpenComPort.ui->QCBDataBits->currentIndex()) {
            case 0: ByteSize= 5; break;
            case 1: ByteSize= 6; break;
            case 2: ByteSize= 7; break;
            case 3: ByteSize= 8; break;
        }
        switch(_OpenComPort.ui->QCBStopBits->currentIndex()) {
            case 0: StopBits= 0; break;
            case 1: StopBits= 1; break;
            case 2: StopBits= 2; break;
        }
        ComPort= _OpenComPort.ui->QCBComPort->currentText();
        BaudRate= _OpenComPort.ui->QCBBaudRate->currentText().toInt();
        FlowControl= _OpenComPort.ui->QCBFlowControl->currentIndex();
        MaxClients= _OpenComPort.ui->QSBTCPServerMaxClients->value();
        SendBreak= _OpenComPort.ui->QCBSendBreak->isChecked();
        Server= _OpenComPort.ui->QLEServer->text();
        Socket= static_cast<quint16>(_OpenComPort.ui->QSBSocket->value());
        if (_OpenComPort.ui->QRBRS232->isChecked()) Mode= MODE_RS232;
        else if (_OpenComPort.ui->QRBTCPClient->isChecked()) Mode= MODE_TCP_CLIENT;
        else Mode= MODE_TCP_CLIENT_SSL;
        if (ConnectionPath.length()> 0) ui->QPBSaveProfile->setEnabled(true);
    }
}

void QDTerminal::on_QPBOpen_clicked() {
    switch(Mode) {
        case MODE_RS232: OpenComPort(); break;
        case MODE_TCP_SERVER: OpenTcpServerPort(); break;
        case MODE_TCP_CLIENT: OpenTcpClientPort(); break;
        case MODE_TCP_CLIENT_SSL: OpenTcpSslPort(); break;
    }
    RowCount= 1;
}

void QDTerminal::on_QPBRTS_clicked() {
    if (ui->QPBRTS->isChecked()) ui->QPBRTS->setStyleSheet("background-color: red; color: black");
    else ui->QPBRTS->setStyleSheet("background-color: green; color: black");
    SerialPort.setRequestToSend(ui->QPBRTS->isChecked());
}

void QDTerminal::on_QPBSaveProfile_clicked() {
    ui->QPBSaveProfile->setEnabled(false);
    SaveProfile(ConnectionPath);
}

void QDTerminal::on_QPBSaveProfileAs_clicked() {
    QString Path= QFileDialog::getSaveFileName(this, tr("Save As"), QDir::currentPath(), "QtComPort (*.qcp)");
    if (Path.length()> 0) {
        if (!Path.endsWith(".qcp", Qt::CaseInsensitive)!= 0) Path+= ".qcp";
        ConnectionPath= Path;
        QTBTerminal->setTabText(TabNumber, ConnectionPath);
        SaveProfile(ConnectionPath);
        ui->QPBSaveProfile->setEnabled(false);
        IsNewConnection= false;
    }
}

void QDTerminal::on_QPBSaveToFile_clicked() {
    QString Path= QFileDialog::getSaveFileName(this, tr("Save to file"), DirectoryPath);
    if (Path.length()> 0) {
        DirectoryPath= QFileInfo(Path).dir().absolutePath();
        QFile FileOut(Path);
        FileOut.open(QIODevice::WriteOnly);
        if (FileOut.isOpen()) {
            FileOut.write(QBAByteIn);
            FileOut.close();
        }
    }
}

void QDTerminal::on_QPBSend_clicked() {
    QString a("**");
    a+= char(7);
    a+= "#";
    switch(Mode) {
        case MODE_RS232: {
            if (SerialPort.isOpen()) {
                if (SendBreak) {
                    SerialPort.setBreakEnabled(true);
                    SerialPort.setBreakEnabled(false);
                }
                SerialPort.write(a.toLocal8Bit());
                SerialPort.waitForBytesWritten(2000);
            }
            break;
        }
        case MODE_TCP_CLIENT: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            TcpSocketClient->write(a.toLocal8Bit());
            TcpSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            SslSocketClient->write(a.toLocal8Bit());
            SslSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(a.toLocal8Bit());
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->waitForBytesWritten(2000);
            break;
        }
    }
}

void QDTerminal::on_QPBSelectAll_clicked() {
    ui->QPTELog->selectAll();
}

void QDTerminal::on_QPBSendFile_clicked() {
    QFileDialog FileDialog;
    FileDialog.setDirectory(DirectoryPath);
    if (FileDialog.exec()== QDialog::Accepted) {
        DirectoryPath= FileDialog.directory().absolutePath();
        QFile FileIn(FileDialog.selectedFiles().at(0));
        if (FileIn.open(QIODevice::ReadOnly)) {
            QByteArray QBABufferIn= FileIn.readAll();
            switch(Mode) {
                case MODE_RS232: {
                    if (SerialPort.isOpen()) {
                        if (SendBreak) {
                            SerialPort.setBreakEnabled(true);
                            SerialPort.setBreakEnabled(false);
                        }
                        SerialPort.write(QBABufferIn, QBABufferIn.size());
                        SerialPort.waitForBytesWritten(2000);
                    }
                    break;
                }
                case MODE_TCP_CLIENT: {
                    if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
                    TcpSocketClient->write(QBABufferIn, QBABufferIn.size());
                    TcpSocketClient->waitForBytesWritten(2000);
                    break;
                }
                case MODE_TCP_CLIENT_SSL: {
                    if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
                    SslSocketClient->write(QBABufferIn, QBABufferIn.size());
                    SslSocketClient->waitForBytesWritten(2000);
                    break;
                }
                case MODE_TCP_SERVER: {
                    for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(QBABufferIn, QBABufferIn.size());
                    for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->waitForBytesWritten(2000);
                    break;
                }
            }
            FileIn.close();
        }
    }
}

void QDTerminal::on_QPBColors_clicked() {
    QDColors Colors(this);
    Colors.ui->QLEPreview->setPalette(ui->QPTELog->palette());
    Colors.ui->QLEPreviewWarnings->setPalette(ui->QPTELog->palette());
    QPalette Palette= Colors.ui->QLEPreviewWarnings->palette();
    Palette.setColor(QPalette::Text, QColor(FontColorWarnings));
    Colors.ui->QLEPreviewWarnings->setPalette(Palette);
    if (Colors.exec()== QDialog::Accepted) {
        FontColor= Colors.ui->QLEPreview->palette().color(QPalette::Text).name();
        FontColorWarnings= Colors.ui->QLEPreviewWarnings->palette().color(QPalette::Text).name();
        //ui->QPTELog->setPalette(Colors.ui->QLEPreview->palette());
        ui->QPBSaveProfile->setEnabled(true);
    }
}

void QDTerminal::on_QPBCopy_clicked() {
    ui->QPTELog->copy();
}

void QDTerminal::on_QRBBin_clicked() {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QRBDec_clicked() {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QRBHex_clicked() {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QRBPrintableOnly_clicked() {
    ui->QPBSaveProfile->setEnabled(true);
}

void QDTerminal::on_QRBPrintableOnly_toggled(bool checked) {
    ui->QCBSpecialCharacters->setEnabled(checked);
}

void QDTerminal::on_QRBSym_clicked() {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QSBNewLineAfterMs_valueChanged(int ) {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QTBTerminalLogFormats_clicked() {
    QDTerminalLogFormats TerminalLogFormats(this);
    switch (TerminalLogFormat) {
        case TERMINAL_LOG_FORMAT_COMPACT: TerminalLogFormats.ui->QRBCompact->setChecked(true); break;
        case TERMINAL_LOG_FORMAT_FULL: TerminalLogFormats.ui->QRBFull->setChecked(true); break;
        case TERMINAL_LOG_FORMAT_SEPARATOR: TerminalLogFormats.ui->QRBSeparator->setChecked(true); break;
    }
    if (TerminalLogFormats.exec()== QDialog::Accepted) {
        if (TerminalLogFormats.ui->QRBCompact->isChecked()) TerminalLogFormat= TERMINAL_LOG_FORMAT_COMPACT;
        else if (TerminalLogFormats.ui->QRBFull->isChecked()) TerminalLogFormat= TERMINAL_LOG_FORMAT_FULL;
        else TerminalLogFormat= TERMINAL_LOG_FORMAT_SEPARATOR;
        on_QRBPrintableOnly_clicked();
    }
}

void QDTerminal::OpenComPort() {
    SerialPort.setPortName(ComPort);
    if (SerialPort.open(QIODevice::ReadWrite)) {
        bool setData, setParity, setStopBits, setFlowControl= false;
        setData= setParity= setStopBits= setFlowControl= false;
        SerialPort.setBaudRate(BaudRate);
        switch(ByteSize) {
            case 5: setData= SerialPort.setDataBits(QSerialPort::Data5); break;
            case 6: setData= SerialPort.setDataBits(QSerialPort::Data6); break;
            case 7: setData= SerialPort.setDataBits(QSerialPort::Data7); break;
            case 8: setData= SerialPort.setDataBits(QSerialPort::Data8); break;
        }
        if (Parity== 'N') setParity= SerialPort.setParity(QSerialPort::NoParity);
        else if (Parity== 'E') setParity= SerialPort.setParity(QSerialPort::EvenParity);
        else if (Parity== 'O') setParity= SerialPort.setParity(QSerialPort::OddParity);
        else if (Parity== 'M') setParity= SerialPort.setParity(QSerialPort::MarkParity);
        else if (Parity== 'S') setParity= SerialPort.setParity(QSerialPort::SpaceParity);
        switch(StopBits) {
            case 0: setStopBits= SerialPort.setStopBits(QSerialPort::OneStop); break;
            case 1: setStopBits= SerialPort.setStopBits(QSerialPort::OneAndHalfStop); break;
            case 2: setStopBits= SerialPort.setStopBits(QSerialPort::TwoStop); break;
        }
        switch(FlowControl) {
            case 0: setFlowControl= SerialPort.setFlowControl(QSerialPort::NoFlowControl); break;
            case 1: setFlowControl= SerialPort.setFlowControl(QSerialPort::HardwareControl); break;
            case 2: setFlowControl= SerialPort.setFlowControl(QSerialPort::SoftwareControl); break;
        }
        if (setData && setParity && setStopBits && setFlowControl) {
            ui->QPBOpen->setEnabled(false);
            ui->QPBModify->setEnabled(false);
            ui->QPBRTS->setEnabled(true);
            ui->QPBDTR->setEnabled(true);
            ui->QPBClose->setEnabled(true);
            ui->QPBRTS->setStyleSheet("background-color: green; color: black");
            ui->QPBDTR->setStyleSheet("background-color: green; color: black");
            SerialPort.setDataTerminalReady(false);
            SerialPort.setRequestToSend(false);
            QTControl= new QTimer(this);
            connect(QTControl, SIGNAL(timeout()), this, SLOT(OnTimeout()));
            QTControl->start(10);
            ui->QLESend->setEnabled(true);
            ui->QPBSend->setEnabled(true);
            ui->QPTELog->installEventFilter(this);
            ui->QPBSaveProfileAs->setEnabled(true);
            ui->QLConnection->setText("ComPort: "+ ComPort+ ", BaudRate: "+ QString::number(BaudRate)+ ", Parity: "+ Parity+ ", ByteSize: "+ QString::number(ByteSize));
            switch(StopBits) {
                case 0: ui->QLConnection->setText(ui->QLConnection->text()+ ", StopBits: 1"); break;
                case 1: ui->QLConnection->setText(ui->QLConnection->text()+ ", StopBits: 1.5"); break;
                case 2: ui->QLConnection->setText(ui->QLConnection->text()+ ", StopBits: 2"); break;
            }
            switch(FlowControl) {
                case 0: ui->QLConnection->setText(ui->QLConnection->text()+ ", FlowControl: none"); break;
                case 1: ui->QLConnection->setText(ui->QLConnection->text()+ ", FlowControl: hardware"); break;
                case 2: ui->QLConnection->setText(ui->QLConnection->text()+ ", FlowControl: handshake off"); break;
            }
            ui->QPBSendFile->setEnabled(true);
        } else {
            TextCursorSet();
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("The ComPort is already in use!")+ "<br></font>");
            ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
            ui->QPBOpen->setEnabled(true);
            ui->QPBModify->setEnabled(true);
        }
    }
}

void QDTerminal::OpenTcpClientPort() {
    ui->QPBOpen->setEnabled(false);
    ui->QPBModify->setEnabled(false);
    ui->QPBClose->setEnabled(true);
    ui->QLESend->setEnabled(true);
    ui->QPBSend->setEnabled(true);
    ui->QPTELog->installEventFilter(this);
    ui->QPBSaveProfileAs->setEnabled(true);
    ui->QLConnection->setText("TCP Client: "+ Server+ ":"+ QString::number(Socket));
    ui->QPBSendFile->setEnabled(true);
    connect(TcpSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
    connect(TcpSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
    connect(TcpSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
    connect(TcpSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    TcpSocketClient->connectToHost(Server, Socket);
    TcpSocketClient->waitForConnected();
}

void QDTerminal::OpenTcpServerPort() {
    TcpServer->setMaxPendingConnections(1);
    if (TcpServer->listen(QHostAddress::Any, Socket)) {
        ui->QPBOpen->setEnabled(false);
        ui->QPBModify->setEnabled(false);
        ui->QPBClose->setEnabled(true);
        ui->QLESend->setEnabled(true);
        ui->QPBSend->setEnabled(true);
        ui->QPTELog->installEventFilter(this);
        ui->QPBSaveProfileAs->setEnabled(true);
        ui->QLConnection->setText("TCP Server: "+ QString::number(Socket));
        ui->QPBSendFile->setEnabled(true);
    }
}

void QDTerminal::OpenTcpSslPort() {
    ui->QPBOpen->setEnabled(false);
    ui->QPBModify->setEnabled(false);
    ui->QPBClose->setEnabled(true);
    ui->QLESend->setEnabled(true);
    ui->QPBSend->setEnabled(true);
    ui->QPTELog->installEventFilter(this);
    ui->QPBSaveProfileAs->setEnabled(true);
    ui->QLConnection->setText("SSL: "+ Server+ ":"+ QString::number(Socket));
    ui->QPBSendFile->setEnabled(true);
    connect(SslSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
    connect(SslSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
    connect(SslSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
    connect(SslSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    SslSocketClient->setPeerVerifyMode(QSslSocket::VerifyNone);
    SslSocketClient->connectToHostEncrypted(Server, Socket);
    if (SslSocketClient->waitForConnected()) {
        SslSocketClient->waitForEncrypted();
    }
}

void QDTerminal::ReadConfigurationFile() {
    QSettings Settings(ConnectionPath, QSettings::IniFormat);
    Settings.beginGroup("Main");
    ComPort= Settings.value("ComPort", "/dev/ttyS0").toString();
    BaudRate= Settings.value("BaudRate", 9600).toInt();
    ByteSize= Settings.value("ByteSize", 8).toInt();
    FlowControl= Settings.value("FlowControl", 2).toInt();
    FontColor= Settings.value("FontColor", "#ffffff").toString();
    FontColorWarnings= Settings.value("FontColorWarnings", "#ffff00").toString();
    MaxClients= Settings.value("MaxClients", 1).toInt();
    Mode= static_cast<Modes>(Settings.value("Mode", MODE_RS232).toInt());
    SendBreak= Settings.value("SendBreak", false).toBool();
    Server= Settings.value("Server").toString();
    Socket= static_cast<quint16>(Settings.value("Socket").toInt());
    StopBits= Settings.value("StopBits", 0).toInt();
    TerminalLogFormat= static_cast<TerminalLogFormats>(Settings.value("TerminalLogFormat", TERMINAL_LOG_FORMAT_FULL).toInt());
    if (Settings.value("Parity", 'N').toString().length()> 0) Parity= Settings.value("Parity", 'N').toString().toLatin1()[0];
    QPalette Palette= ui->QPTELog->palette();
    Palette.setColor(QPalette::Base, QColor(Settings.value("BackgroundColor", "#000000").toString()));
    Palette.setColor(QPalette::Text, QColor(FontColor));
    ui->QPTELog->setPalette(Palette);
    QFont Font(Settings.value("Font", "DejaVu Sans Mono").toString(), 8);
    Font.setFixedPitch(true);
    ui->QPTELog->setFont(Font);
    QFontMetrics FontMetrics(ui->QPTELog->font());
    ui->QPTELog->setTabStopDistance(2* FontMetrics.horizontalAdvance(' '));
    if (Settings.value("ViewMode").toInt()== 0) ui->QRBPrintableOnly->setChecked(true);
    else if (Settings.value("ViewMode").toInt()== 1) ui->QRBSym->setChecked(true);
    else if (Settings.value("ViewMode").toInt()== 2) ui->QRBDec->setChecked(true);
    else if (Settings.value("ViewMode").toInt()== 3) ui->QRBHex->setChecked(true);
    else if (Settings.value("ViewMode").toInt()== 4) ui->QRBBin->setChecked(true);
    ui->QCBNewLineAfter->setChecked(Settings.value("NewLineAfter", false).toBool());
    ui->QSBNewLineAfterMs->setValue(Settings.value("NewLineAfterMs", 1000).toInt());
    ui->QCBRowCount->setChecked(Settings.value("RowCount", false).toBool());
    ui->QCBSpecialCharacters->setChecked(Settings.value("SpecialCharacters", false).toBool());
    ui->QCBTimestamp->setChecked(Settings.value("Timestamp", false).toBool());
    ui->QRBCR->setChecked(Settings.value("NewLineWidth", "lf").toString().compare("cr")== 0);
    ui->QRBLF->setChecked(Settings.value("NewLineWidth", "lf").toString().compare("lf")== 0);
    switch(Mode) {
        case MODE_RS232: OpenComPort(); break;
        case MODE_TCP_SERVER: OpenTcpServerPort(); break;
        case MODE_TCP_CLIENT: OpenTcpClientPort(); break;
        case MODE_TCP_CLIENT_SSL: OpenTcpSslPort(); break;
    }
}

void QDTerminal::ReadyRead() {
    QByteArray QBABufferIn;
    switch(Mode) {
        case MODE_TCP_CLIENT: {
            QBABufferIn= TcpSocketClient->readAll();
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            QBABufferIn= SslSocketClient->readAll();
            break;
        }
        case MODE_TCP_SERVER: {
            QBABufferIn= static_cast<QTcpSocket *>(QObject::sender())->readAll();
            break;
        }
        default:
            break;
    }
    if (QBABufferIn.length()> 0) {
        if (pQDTerminal) pQDTerminal->SendByteArray(QBABufferIn);
        ShowBufferIn(QBABufferIn);
        QBAByteIn.append(QBABufferIn);
    }
}

void QDTerminal::SaveProfile(QString ConnectionPath) {
    QSettings Settings(ConnectionPath, QSettings::IniFormat);
    Settings.beginGroup("Main");
    Settings.setValue("ComPort", ComPort);
    Settings.setValue("BaudRate", BaudRate);
    Settings.setValue("BackgroundColor", ui->QPTELog->palette().color(QPalette::Base).name());
    Settings.setValue("ByteSize", ByteSize);
    Settings.setValue("StopBits", StopBits);
    Settings.setValue("FlowControl", FlowControl);
    Settings.setValue("Font", ui->QPTELog->font().toString());
    Settings.setValue("FontColor", ui->QPTELog->palette().color(QPalette::Text).name());
    Settings.setValue("FontColorWarnings", FontColorWarnings);
    Settings.setValue("MaxClients", MaxClients);
    Settings.setValue("Mode", Mode);
    Settings.setValue("NewLineAfter", ui->QCBNewLineAfter->isChecked());
    Settings.setValue("NewLineAfterMs", ui->QSBNewLineAfterMs->value());
    Settings.setValue("NewLineWidth", ui->QRBCR->isChecked() ? "cr" : "lf");
    Settings.setValue("Parity", QString(Parity));
    Settings.setValue("RowCount", ui->QCBRowCount->isChecked());
    Settings.setValue("SendBreak", SendBreak);
    Settings.setValue("Server", Server);
    Settings.setValue("Socket", Socket);
    Settings.setValue("SpecialCharacters", ui->QCBSpecialCharacters->isChecked());
    Settings.setValue("TerminalLogFormat", TerminalLogFormat);
    Settings.setValue("Timestamp", ui->QCBTimestamp->isChecked());
    if (ui->QRBPrintableOnly->isChecked()) Settings.setValue("ViewMode", 0);
    else if (ui->QRBSym->isChecked()) Settings.setValue("ViewMode", 1);
    else if (ui->QRBDec->isChecked()) Settings.setValue("ViewMode", 2);
    else if (ui->QRBHex->isChecked()) Settings.setValue("ViewMode", 3);
    else if (ui->QRBBin->isChecked()) Settings.setValue("ViewMode", 4);
}

void QDTerminal::ShowBufferIn(QByteArray &QBABufferIn) {
    QString BufferIn;
    if (ui->QRBPrintableOnly->isChecked()) {
        if (ui->QCBNewLineAfter->isChecked()) {
            if (ui->QPTELog->toPlainText().length()> 0) {
                if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                    BufferIn+= "\n";
                }
            }
        }
        if (ui->QCBRowCount->isChecked()) {
            BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
            RowCount++;
        }
        if (ui->QCBTimestamp->isChecked()) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
        for (int count= 0; count< QBABufferIn.size(); count++) {
            switch(QBABufferIn.at(count)) {
                case 1: if (ui->QCBSpecialCharacters->isChecked()) BufferIn+= "[SOH]"; break;
                case 2: if (ui->QCBSpecialCharacters->isChecked()) BufferIn+= "[STX]"; break;
                case 3: if (ui->QCBSpecialCharacters->isChecked()) BufferIn+= "[ETX]"; break;
                case 4: if (ui->QCBSpecialCharacters->isChecked()) BufferIn+= "[EOT]"; break;
                case 6: if (ui->QCBSpecialCharacters->isChecked()) BufferIn+= "[ACK]"; break;
                case 7: QApplication::beep(); break;
                case 9: BufferIn+= "\t"; break;
                case 10: {
                    if (ui->QRBLF->isChecked()) {
                        BufferIn+= "\n";
                        if (ui->QCBRowCount->isChecked() && count< QBABufferIn.size()- 1) {
                            BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
                            RowCount++;
                        }
                        if (ui->QCBTimestamp->isChecked() && count< QBABufferIn.size()- 1) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
                    }
                    break;
                }
                case 13: {
                    if (ui->QRBCR->isChecked()) {
                        BufferIn+= "\n";
                        if (ui->QCBRowCount->isChecked() && count< QBABufferIn.size()- 1) {
                            BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
                            RowCount++;
                        }
                        if (ui->QCBTimestamp->isChecked() && count< QBABufferIn.size()- 1) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
                    }
                    break;
                }
                case 21: if (ui->QCBSpecialCharacters->isChecked()) BufferIn+= "[NACK]"; break;
                default: {
                    if ((uchar(QBABufferIn.at(count))>= 32 && uchar(QBABufferIn.at(count))<= 126) ||
                            (uchar(QBABufferIn.at(count))>= 128 && uchar(QBABufferIn.at(count))<= 254)) BufferIn+= char(QBABufferIn.at(count));
                    else BufferIn+= "[?]";
                    break;
                }
            }
        }
    } else if (ui->QRBSym->isChecked()) {
        if (ui->QCBNewLineAfter->isChecked()) {
            if (ui->QPTELog->toPlainText().length()> 0) {
                if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                    BufferIn+= "\n";
                }
            }
        }
        if (ui->QCBRowCount->isChecked()) {
            BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
            RowCount++;
        }
        if (ui->QCBTimestamp->isChecked()) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
        for (int count= 0; count< QBABufferIn.size(); count++) {
            switch(QBABufferIn.at(count)) {
                case 0: BufferIn+= "[NUL]"; break;
                case 1: BufferIn+= "[SOH]"; break;
                case 2: BufferIn+= "[STX]"; break;
                case 3: BufferIn+= "[ETX]"; break;
                case 4: BufferIn+= "[EOT]"; break;
                case 5: BufferIn+= "[ENQ]"; break;
                case 6: BufferIn+= "[ACQ]"; break;
                case 7: BufferIn+= "[BEL]"; break;
                case 8: BufferIn+= "[BS]"; break;
                case 9: BufferIn+= "[TAB]"; break;
                case 10: {
                    BufferIn+= "[LF]\n";
                    if (ui->QCBRowCount->isChecked() && count< QBABufferIn.size()- 1) {
                        BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
                        RowCount++;
                    }
                    if (ui->QCBTimestamp->isChecked() && count< QBABufferIn.size()- 1) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
                    break;
                }
                case 11: BufferIn+= "[VT]"; break;
                case 12: BufferIn+= "[FF]"; break;
                case 13: {
                    BufferIn+= "[CR]\r";
                    if (ui->QCBRowCount->isChecked() && count< QBABufferIn.size()- 1) {
                        BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
                        RowCount++;
                    }
                    if (ui->QCBTimestamp->isChecked() && count< QBABufferIn.size()- 1) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
                    break;
                }
                case 14: BufferIn+= "[SO]"; break;
                case 15: BufferIn+= "[SI]"; break;
                case 16: BufferIn+= "[DLE]"; break;
                case 17: BufferIn+= "[DC1]"; break;
                case 18: BufferIn+= "[DC2]"; break;
                case 19: BufferIn+= "[DC3]"; break;
                case 20: BufferIn+= "[DC4]"; break;
                case 21: BufferIn+= "[NAK]"; break;
                case 22: BufferIn+= "[SYN]"; break;
                case 23: BufferIn+= "[ETB]"; break;
                case 24: BufferIn+= "[CAN]"; break;
                case 25: BufferIn+= "[EM]"; break;
                case 26: BufferIn+= "[SUB]"; break;
                case 27: BufferIn+= "[ESC]"; break;
                case 28: BufferIn+= "[FS]"; break;
                case 29: BufferIn+= "[GS]"; break;
                case 30: BufferIn+= "[RS]"; break;
                case 31: BufferIn+= "[US]"; break;
                default: BufferIn+= QString(char(QBABufferIn.at(count))); break;
            }
        }

    } else if (ui->QRBDec->isChecked()) {
        if (ui->QCBNewLineAfter->isChecked()) {
            if (ui->QPTELog->toPlainText().length()> 0) {
                if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                    BufferIn+= "\n";
                }
            }
        }
        if (ui->QCBRowCount->isChecked()) {
            BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
            RowCount++;
        }
        if (ui->QCBTimestamp->isChecked()) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
        for (int count= 0; count< QBABufferIn.size(); count++) {
            switch (TerminalLogFormat) {
                case TERMINAL_LOG_FORMAT_COMPACT: BufferIn+= QString::number(uchar(QBABufferIn.at(count))).rightJustified(3, '0'); break;
                case TERMINAL_LOG_FORMAT_FULL: BufferIn+= "["+ QString::number(uchar(QBABufferIn.at(count))).rightJustified(3, '0')+ "]"; break;
                case TERMINAL_LOG_FORMAT_SEPARATOR: BufferIn+= QString::number(uchar(QBABufferIn.at(count))).rightJustified(3, '0')+ ","; break;
            }

        }
    } else if (ui->QRBBin->isChecked()) {
        if (ui->QCBNewLineAfter->isChecked()) {
            if (ui->QPTELog->toPlainText().length()> 0) {
                if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                    BufferIn+= "\n";
                }
            }
        }
        if (ui->QCBRowCount->isChecked()) {
            BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
            RowCount++;
        }
        if (ui->QCBTimestamp->isChecked()) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
        for (int count= 0; count< QBABufferIn.size(); count++) {
            switch (TerminalLogFormat) {
                case TERMINAL_LOG_FORMAT_COMPACT: BufferIn+= QString::number(uchar(QBABufferIn.at(count)), 2).rightJustified(8, '0'); break;
                case TERMINAL_LOG_FORMAT_FULL: BufferIn+= "["+ QString::number(uchar(QBABufferIn.at(count)), 2).rightJustified(8, '0')+ "]"; break;
                case TERMINAL_LOG_FORMAT_SEPARATOR: BufferIn+= QString::number(uchar(QBABufferIn.at(count)), 2).rightJustified(8, '0')+ ","; break;
            }
        }
    } else {
        if (ui->QCBNewLineAfter->isChecked()) {
            if (ui->QPTELog->toPlainText().length()> 0) {
                if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                    BufferIn+= "\n";
                }
            }
        }
        if (ui->QCBRowCount->isChecked()) {
            BufferIn+= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-";
            RowCount++;
        }
        if (ui->QCBTimestamp->isChecked()) BufferIn+= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-";
        for (int count= 0; count< QBABufferIn.size(); count++) {
            switch (TerminalLogFormat) {
                case TERMINAL_LOG_FORMAT_COMPACT: BufferIn+= QString::number(uchar(QBABufferIn.at(count)), 16).rightJustified(2, '0').toUpper(); break;
                case TERMINAL_LOG_FORMAT_FULL: BufferIn+= "["+ QString::number(uchar(QBABufferIn.at(count)), 16).rightJustified(2, '0').toUpper()+ "]"; break;
                case TERMINAL_LOG_FORMAT_SEPARATOR: BufferIn+= QString::number(uchar(QBABufferIn.at(count)), 16).rightJustified(2, '0').toUpper()+ ","; break;
            }
        }
    }
    TextCursorSet();
    QTextCharFormat TextCharFormat= ui->QPTELog->currentCharFormat();
    TextCharFormat.setForeground(QBrush(QColor(FontColor)));
    ui->QPTELog->setCurrentCharFormat(TextCharFormat);
    ui->QPTELog->insertPlainText(BufferIn);
    //ui->QPTELog->textCursor().insertHtml("<font color="+ FontColor+ ">"+ BufferIn+ "</font>");
    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
    QDTLastByteIn= QDateTime::currentDateTime();
}

void QDTerminal::SendByteArray(QByteArray QBABufferIn) {
    switch(Mode) {
        case MODE_RS232: {
            if (SerialPort.isOpen()) {
                if (SendBreak) {
                    SerialPort.setBreakEnabled(true);
                    SerialPort.setBreakEnabled(false);
                }
                SerialPort.write(QBABufferIn, QBABufferIn.size());
                SerialPort.waitForBytesWritten(2000);
            }
            break;
        }
        case MODE_TCP_CLIENT: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            TcpSocketClient->write(QBABufferIn, QBABufferIn.size());
            TcpSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            SslSocketClient->write(QBABufferIn, QBABufferIn.size());
            SslSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(QBABufferIn, QBABufferIn.size());
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->waitForBytesWritten(2000);
            break;
        }
    }
}

void QDTerminal::TextCursorSet() {
    QTextCursor Cursor= QTextCursor(ui->QPTELog->document());
    Cursor.movePosition(QTextCursor::End);
    ui->QPTELog->setTextCursor(Cursor);
}
