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
    this->IsNewConnection= IsNewConnection;
    QdTerminal= NULL;
    IsCTSEnabled= IsDCDEnabled= IsDSREnabled= false;
    DirectoryPath= "";
    ui->QLCTS->setStyleSheet("background-color: green; color: black");
    ui->QLDCD->setStyleSheet("background-color: green; color: black");
    ui->QLDSR->setStyleSheet("background-color: green; color: black");
    QFont(ui->QPTELog->font()).setFixedPitch(true);
    Tcp= new QTcpSocket(this);
    QDTLastByteIn= QDateTime::currentDateTime();
    RowCount= 0;
}

QDTerminal::~QDTerminal() {
    delete ui;
}

bool QDTerminal::eventFilter(QObject *object, QEvent *event) {
    if (object== ui->QPTELog && event->type()== QEvent::KeyPress) {
        QKeyEvent *keyEvent= (QKeyEvent*)event;
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
        case MODERS232: SerialPort.write(KeyPressed.toLatin1());
        case MODETCPIP: Tcp->write(KeyPressed.toLatin1()); break;
        }
        /*switch(keyEvent->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
                switch(Mode) {
                case MODERS232: QcSerialComPort.SendChar('\r');
                case MODETCPIP: Tcp->write(QString("\r").toLatin1()); break;
                }
                break;
            }
        case Qt::Key_F1: {

            }
        default: {
                switch(Mode) {
                case MODERS232: QcSerialComPort.SendString(keyEvent->->text()->text()); break;
                case MODETCPIP: Tcp->write(QString(keyEvent->text()).toLatin1()); break;
                }
            }
        }*/
        return true;
    } else return QObject::eventFilter(object, event);
}

void QDTerminal::Connected() {
    ui->QPTELog->setPlainText(ui->QPTELog->toPlainText()+ "Connected");
    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
}

void QDTerminal::Disconnected() {
    ui->QPBClose->click();
}

void QDTerminal::Error(QAbstractSocket::SocketError ) {
    ui->QPTELog->setPlainText(ui->QPTELog->toPlainText()+ Tcp->errorString());
    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
}

void QDTerminal::OnTimeout() {
    QTControl->stop();
    if ((SerialPort.pinoutSignals() & QSerialPort::ClearToSendSignal)!= IsCTSEnabled) {
        if (SerialPort.pinoutSignals() & QSerialPort::ClearToSendSignal) ui->QLCTS->setStyleSheet("background-color: red; color: black");
        else ui->QLCTS->setStyleSheet("background-color: green; color: black");
        IsCTSEnabled= SerialPort.pinoutSignals() & QSerialPort::ClearToSendSignal;
    }
    if ((SerialPort.pinoutSignals() & QSerialPort::DataCarrierDetectSignal)!= IsDCDEnabled) {
        if (SerialPort.pinoutSignals() & QSerialPort::DataCarrierDetectSignal) ui->QLDCD->setStyleSheet("background-color: red; color: black");
        else ui->QLDCD->setStyleSheet("background-color: green; color: black");
        IsDCDEnabled= SerialPort.pinoutSignals() & QSerialPort::DataCarrierDetectSignal;
    }
    if ((SerialPort.pinoutSignals() & QSerialPort::DataSetReadySignal)!= IsDSREnabled) {
        if (SerialPort.pinoutSignals() & QSerialPort::DataSetReadySignal) ui->QLDSR->setStyleSheet("background-color: red; color: black");
        else ui->QLDSR->setStyleSheet("background-color: green; color: black");
        IsDSREnabled= SerialPort.pinoutSignals() & QSerialPort::DataSetReadySignal;
    }
    QByteArray QBABufferIn= SerialPort.readAll();
    if (QBABufferIn.size()> 0) {
        if (QdTerminal!= NULL) QdTerminal->SendByteArray(QBABufferIn);
        ShowBufferIn(QBABufferIn);
        QBAByteIn.append(QBABufferIn);
    }
    QTControl->start(10);
}

void QDTerminal::on_QCBRowCount_toggled(bool ) {
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
        case MODERS232: {
            if (SerialPort.write(QString(ui->QLESend->text()+ "\r\n").toLatin1())) ui->QLESend->setText("");
            break;
        }
        case MODETCPIP: {
            Tcp->write(ui->QLESend->text().toLatin1());
            Tcp->write(QString("\r\n").toLatin1());
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
    QDFontDialog QdFontDialog;
    QdFontDialog.SetCurrentFont(ui->QPTELog->font());
    if (QdFontDialog.exec()== QDialog::Accepted) {
        ui->QPTELog->setFont(QdFontDialog.GetCurrentFont());
        ui->QPBSaveProfile->setEnabled(true);
    }
}

void QDTerminal::on_QPBClear_clicked() {
    ui->QPTELog->clear();
    QBAByteIn.clear();
    RowCount= 1;
}

void QDTerminal::on_QPBClose_clicked() {
    ui->QPTELog->removeEventFilter(this);
    ui->QLConnection->setText("");
    switch(Mode) {
    case MODERS232: {
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
    case MODETCPIP: {
            disconnect(Tcp, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
            disconnect(Tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
            disconnect(Tcp, SIGNAL(connected()), this, SLOT(Connected()));
            disconnect(Tcp, SIGNAL(disconnected()), this, SLOT(Disconnected()));
            Tcp->disconnectFromHost();
            break;
        }
    }
    ui->QPBOpen->setEnabled(true);
    ui->QPBClose->setEnabled(false);
    ui->QPBChangeFont->setEnabled(false);
    ui->QPBColors->setEnabled(false);
    ui->QPBModify->setEnabled(true);
    ui->QPBSendFile->setEnabled(false);
}

void QDTerminal::on_QPBDTR_clicked() {
    if (ui->QPBDTR->isChecked()) ui->QPBDTR->setStyleSheet("background-color: red; color: black");
    else ui->QPBDTR->setStyleSheet("background-color: green; color: black");
    SerialPort.setDataTerminalReady(ui->QPBDTR->isChecked());
}

void QDTerminal::on_QPBModify_clicked() {
    QDOpenComPort QdOpenComPort;
    foreach (const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts()) QdOpenComPort.ui->QCBComPort->addItem(SerialPortInfo.portName());
    for (int count= 0; count< QdOpenComPort.ui->QCBComPort->count(); count++) {
        if (QdOpenComPort.ui->QCBComPort->itemText(count).compare(ComPort, Qt::CaseInsensitive)== 0) {
            QdOpenComPort.ui->QCBComPort->setCurrentIndex(count);
            break;
        }
    }
    for (int count= 0; count< QdOpenComPort.ui->QCBBaudRate->count(); count++) {
        if (QdOpenComPort.ui->QCBBaudRate->itemText(count).compare(QString::number(BaudRate), Qt::CaseInsensitive)== 0) {
            QdOpenComPort.ui->QCBBaudRate->setCurrentIndex(count);
            break;
        }
    }
    switch(Parity) {
    case 'N': QdOpenComPort.ui->QCBParity->setCurrentIndex(0); break;
    case 'E': QdOpenComPort.ui->QCBParity->setCurrentIndex(1); break;
    case 'O': QdOpenComPort.ui->QCBParity->setCurrentIndex(2); break;
    case 'M': QdOpenComPort.ui->QCBParity->setCurrentIndex(3); break;
    case 'S': QdOpenComPort.ui->QCBParity->setCurrentIndex(4); break;
    }
    switch(ByteSize) {
    case 5: QdOpenComPort.ui->QCBDataBits->setCurrentIndex(0); break;
    case 6: QdOpenComPort.ui->QCBDataBits->setCurrentIndex(1); break;
    case 7: QdOpenComPort.ui->QCBDataBits->setCurrentIndex(2); break;
    case 8: QdOpenComPort.ui->QCBDataBits->setCurrentIndex(3); break;
    }
    switch(StopBits) {
    case 0: QdOpenComPort.ui->QCBStopBits->setCurrentIndex(0); break;
    case 1: QdOpenComPort.ui->QCBStopBits->setCurrentIndex(1); break;
    case 2: QdOpenComPort.ui->QCBStopBits->setCurrentIndex(2); break;
    }
    QdOpenComPort.ui->QCBFlowControl->setCurrentIndex(FlowControl);
    QdOpenComPort.ui->QCBSendBreak->setChecked(SendBreak);
    QdOpenComPort.ui->QLEServer->setText(Server);
    QdOpenComPort.ui->QSBSocket->setValue(Socket);
    switch(Mode) {
    case MODERS232: QdOpenComPort.ui->QRBRS232->setChecked(true); break;
    case MODETCPIP: {
            QdOpenComPort.ui->QRBTCPIP->setChecked(true);
            QdOpenComPort.ui->QRBTCPIP->click();
            break;
        }
    }
    if (QdOpenComPort.exec()== QDialog::Accepted) {
        switch(QdOpenComPort.ui->QCBParity->currentIndex()) {
        case 0: Parity= 'N'; break;
        case 1: Parity= 'E'; break;
        case 2: Parity= 'O'; break;
        case 3: Parity= 'M'; break;
        case 4: Parity= 'S'; break;
        }
        switch(QdOpenComPort.ui->QCBDataBits->currentIndex()) {
        case 0: ByteSize= 5; break;
        case 1: ByteSize= 6; break;
        case 2: ByteSize= 7; break;
        case 3: ByteSize= 8; break;
        }
        switch(QdOpenComPort.ui->QCBStopBits->currentIndex()) {
        case 0: StopBits= 0; break;
        case 1: StopBits= 1; break;
        case 2: StopBits= 2; break;
        }
        ComPort= QdOpenComPort.ui->QCBComPort->currentText();
        BaudRate= QdOpenComPort.ui->QCBBaudRate->currentText().toInt();
        FlowControl= QdOpenComPort.ui->QCBFlowControl->currentIndex();
        SendBreak= QdOpenComPort.ui->QCBSendBreak->isChecked();
        Server= QdOpenComPort.ui->QLEServer->text();
        Socket= QdOpenComPort.ui->QSBSocket->value();
        if (QdOpenComPort.ui->QRBRS232->isChecked()) Mode= MODERS232;
        else Mode= MODETCPIP;
        if (ConnectionPath.length()> 0) ui->QPBSaveProfile->setEnabled(true);
    }
}

void QDTerminal::on_QPBOpen_clicked() {
    switch(Mode) {
    case MODERS232: OpenComPort(); break;
    case MODETCPIP: OpenTcpPort(); break;
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
    QString Path= QFileDialog::getSaveFileName(this, "Save As", QDir::currentPath(), "QtComPort (*.qcp)");
    if (Path!= NULL) {
        if (!Path.endsWith(".qcp", Qt::CaseInsensitive)!= 0) Path+= ".qcp";
        ConnectionPath= Path;
        QTBTerminal->setTabText(TabNumber, ConnectionPath);
        SaveProfile(ConnectionPath);
        ui->QPBSaveProfile->setEnabled(false);
        IsNewConnection= false;
    }
}

void QDTerminal::on_QPBSaveToFile_clicked() {
    QString Path= QFileDialog::getSaveFileName(this, "Save to file", DirectoryPath);
    if (Path!= NULL) {
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
        case MODERS232: {
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
        case MODETCPIP: {
            Tcp->write(a.toLocal8Bit());
            Tcp->waitForBytesWritten(2000);
            break;
        }
    }
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
                case MODERS232: {
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
                case MODETCPIP: {
                    Tcp->write(QBABufferIn, QBABufferIn.size());
                    Tcp->waitForBytesWritten(2000);
                    break;
                }
            }
            FileIn.close();
        }
    }
}

void QDTerminal::on_QPBColors_clicked() {
    QDColors Colors;
    Colors.ui->QLEPreview->setPalette(ui->QPTELog->palette());
    if (Colors.exec()== QDialog::Accepted) {
        ui->QPTELog->setPalette(Colors.ui->QLEPreview->palette());
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

void QDTerminal::on_QRBSym_clicked() {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QSBNewLineAfterMs_valueChanged(int ) {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QPBSelectAll_clicked() {
    ui->QPTELog->selectAll();
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
            ui->QPBChangeFont->setEnabled(true);
            ui->QPBColors->setEnabled(true);
            ui->QPBRTS->setStyleSheet("background-color: green; color: black");
            ui->QPBDTR->setStyleSheet("background-color: green; color: black");
            SerialPort.setDataTerminalReady(false);
            SerialPort.setRequestToSend(false);
            QTControl= new QTimer(this);
            connect(QTControl, SIGNAL(timeout()), this, SLOT(OnTimeout()));
            QTControl->start(10);
            ui->QLESend->setEnabled(true);
            ui->QPBSend->setEnabled(true);
            ui->QPBClose->setEnabled(true);
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
            ui->QPTELog->appendPlainText(tr("The ComPort is already in use!"));
            ui->QPBOpen->setEnabled(true);
            ui->QPBModify->setEnabled(true);
        }
    }
}

void QDTerminal::OpenTcpPort() {
    ui->QPBOpen->setEnabled(false);
    ui->QPBModify->setEnabled(false);
    ui->QPBClose->setEnabled(true);
    ui->QPBChangeFont->setEnabled(true);
    ui->QPBColors->setEnabled(true);
    ui->QLESend->setEnabled(true);
    ui->QPBSend->setEnabled(true);
    ui->QPTELog->installEventFilter(this);
    ui->QPBSaveProfileAs->setEnabled(true);
    ui->QLConnection->setText("TCPIP: "+ Server+ ":"+ QString::number(Socket));
    ui->QPBSendFile->setEnabled(true);
    connect(Tcp, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
    connect(Tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
    connect(Tcp, SIGNAL(connected()), this, SLOT(Connected()));
    connect(Tcp, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    Tcp->connectToHost(Server, Socket);
}

void QDTerminal::ReadyRead() {
    QByteArray QBABufferIn= Tcp->readAll();
    if (QBABufferIn.length()> 0) {
        if (QdTerminal!= NULL) QdTerminal->SendByteArray(QBABufferIn);
        ShowBufferIn(QBABufferIn);
        QBAByteIn.append(QBABufferIn);
    }
}

void QDTerminal::SaveProfile(QString ConnectionPath) {
    QSettings *Settings= new QSettings(ConnectionPath, QSettings::IniFormat); {
        Settings->beginGroup("Main");
        Settings->setValue("ComPort", ComPort);
        Settings->setValue("BaudRate", BaudRate);
        Settings->setValue("Parity", QString(Parity));
        Settings->setValue("ByteSize", ByteSize);
        Settings->setValue("StopBits", StopBits);
        Settings->setValue("FlowControl", FlowControl);
        Settings->setValue("Font", ui->QPTELog->font().toString());
        if (ui->QRBPrintableOnly->isChecked()) Settings->setValue("ViewMode", 0);
        else if (ui->QRBSym->isChecked()) Settings->setValue("ViewMode", 1);
        else if (ui->QRBDec->isChecked()) Settings->setValue("ViewMode", 2);
        else if (ui->QRBHex->isChecked()) Settings->setValue("ViewMode", 3);
        else if (ui->QRBBin->isChecked()) Settings->setValue("ViewMode", 4);
        int r, g, b;
        ui->QPTELog->palette().color(QPalette::Text).getRgb(&r, &g, &b);
        Settings->setValue("FontColor", QString::number(r)+ ","+ QString::number(g)+ ","+ QString::number(b));
        ui->QPTELog->palette().color(QPalette::Base).getRgb(&r, &g, &b);
        Settings->setValue("BackgroundColor", QString::number(r)+ ","+ QString::number(g)+ ","+ QString::number(b));
        Settings->setValue("Mode", Mode);
        Settings->setValue("NewLineAfter", ui->QCBNewLineAfter->isChecked());
        Settings->setValue("NewLineAfterMs", ui->QSBNewLineAfterMs->value());
        Settings->setValue("RowCount", ui->QCBRowCount->isChecked());
        Settings->setValue("SendBreak", SendBreak);
        Settings->setValue("Server", Server);
        Settings->setValue("Socket", Socket);
        Settings->setValue("Timestamp", ui->QCBTimestamp->isChecked());
    }{
        delete Settings;
    }
}

void QDTerminal::ShowBufferIn(QByteArray &QBABufferIn) {
    QString BufferIn;
    for (int count= 0; count< QBABufferIn.size(); count++) {
        if (ui->QRBPrintableOnly->isChecked()) {
            switch(QBABufferIn.at(count)) {
            case 7: QApplication::beep(); break;
            case 10: BufferIn+= "\n"; break;
            case 13: break;
            default: {
                    if (int(QBABufferIn.at(count))>= 32 && int(QBABufferIn.at(count))<= 126) BufferIn+= char(QBABufferIn.at(count));
                    else BufferIn+= "[?]";
                    break;
                }
            }
        } else if (ui->QRBSym->isChecked()) {
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
            case 10: BufferIn+= "[LF]\n"; break;
            case 11: BufferIn+= "[VT]"; break;
            case 12: BufferIn+= "[FF]"; break;
            case 13: BufferIn+= "[CR]\r"; break;
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
        } else if (ui->QRBDec->isChecked()) BufferIn+= "["+ QString::number(uchar(QBABufferIn.at(count))).rightJustified(3, '0')+ "]";
        else if (ui->QRBBin->isChecked()) BufferIn+= "["+ QString::number(uchar(QBABufferIn.at(count)), 2).rightJustified(8, '0')+ "]";
        else BufferIn+= "[0x"+ QString::number(uchar(QBABufferIn.at(count)), 16).rightJustified(2, '0').toUpper()+ "]";
    }
    if (ui->QCBNewLineAfter->isChecked()) {
        if (ui->QPTELog->toPlainText().length()> 0) {
            if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                if (ui->QCBRowCount->isChecked()) {
                    BufferIn= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-"+ BufferIn;
                    RowCount++;
                }
                if (ui->QCBTimestamp->isChecked()) BufferIn= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-"+ BufferIn;
                ui->QPTELog->setPlainText(ui->QPTELog->toPlainText()+ '\n'+ BufferIn);
            } else ui->QPTELog->setPlainText(ui->QPTELog->toPlainText()+ BufferIn);
        } else {
            if (ui->QCBRowCount->isChecked()) {
                BufferIn= "->"+ QString::number(RowCount).rightJustified(3, '0')+ "<-"+ BufferIn;
                RowCount++;
            }
            if (ui->QCBTimestamp->isChecked()) BufferIn= "->"+ QDateTime::currentDateTime().toString("hh:mm:ss.zzz")+ "<-"+ BufferIn;
            ui->QPTELog->setPlainText(ui->QPTELog->toPlainText()+ BufferIn);
        }
        QDTLastByteIn= QDateTime::currentDateTime();
    } else ui->QPTELog->setPlainText(ui->QPTELog->toPlainText()+ BufferIn);
    ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
}

void QDTerminal::SendByteArray(QByteArray QBABufferIn) {
    switch(Mode) {
        case MODERS232: {
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
        case MODETCPIP: {
            Tcp->write(QBABufferIn, QBABufferIn.size());
            Tcp->waitForBytesWritten(2000);
            break;
        }
    }
}

void QDTerminal::ReadConfigurationFile() {
    QSettings *Settings= new QSettings(ConnectionPath, QSettings::IniFormat); {
        Settings->beginGroup("Main");
        ComPort= Settings->value("ComPort", "/dev/ttyS0").toString();
        BaudRate= Settings->value("BaudRate", 9600).toInt();
        if (Settings->value("Parity", 'N').toString().length()> 0) Parity= Settings->value("Parity", 'N').toString().toLatin1()[0];
        ByteSize= Settings->value("ByteSize", 8).toInt();
        StopBits= Settings->value("StopBits", 0).toInt();
        FlowControl= Settings->value("FlowControl", 2).toInt();
        QFont font;
        if (Settings->contains("Font")) font.fromString(Settings->value("Font").toString());
        else font.fromString("DejaVu Sans,9,-1,0,50,0,0,0,0,0");
        ui->QPTELog->setFont(font);
        if (Settings->value("ViewMode").toInt()== 0) ui->QRBPrintableOnly->setChecked(true);
        else if (Settings->value("ViewMode").toInt()== 1) ui->QRBSym->setChecked(true);
        else if (Settings->value("ViewMode").toInt()== 2) ui->QRBDec->setChecked(true);
        else if (Settings->value("ViewMode").toInt()== 3) ui->QRBHex->setChecked(true);
        else if (Settings->value("ViewMode").toInt()== 4) ui->QRBBin->setChecked(true);
        QPalette Palette= ui->QPTELog->palette();
        int r, g, b;
        QString Color= Settings->value("FontColor").toString();
        r= Color.left(Color.indexOf(",")).toInt();
        Color= Color.right(Color.length()- Color.indexOf(",")- 1);
        g= Color.left(Color.indexOf(",")).toInt();
        Color= Color.right(Color.length()- Color.indexOf(",")- 1);
        b= Color.toInt();
        Palette.setColor(QPalette::Text, QColor::fromRgb(r, g, b));
        Color= Settings->value("BackgroundColor").toString();
        r= Color.left(Color.indexOf(",")).toInt();
        Color= Color.right(Color.length()- Color.indexOf(",")- 1);
        g= Color.left(Color.indexOf(",")).toInt();
        Color= Color.right(Color.length()- Color.indexOf(",")- 1);
        b= Color.toInt();
        Palette.setColor(QPalette::Base, QColor::fromRgb(r, g, b));
        ui->QPTELog->setPalette(Palette);
        Mode= Settings->value("Mode", MODERS232).toInt();
        ui->QCBNewLineAfter->setChecked(Settings->value("NewLineAfter", false).toBool());
        ui->QSBNewLineAfterMs->setValue(Settings->value("NewLineAfterMs", 1000).toInt());
        ui->QCBRowCount->setChecked(Settings->value("RowCount", false).toBool());
        ui->QCBTimestamp->setChecked(Settings->value("Timestamp", false).toBool());
        SendBreak= Settings->value("SendBreak", false).toBool();
        Server= Settings->value("Server").toString();
        Socket= Settings->value("Socket").toInt();
        switch(Mode) {
        case MODERS232: OpenComPort(); break;
        case MODETCPIP: OpenTcpPort(); break;
        }
    }{
        delete Settings;
    }
}
