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
    pQcSSLServer= new QcSSLServer(this);
    pQSslSocketClient= new QSslSocket();
    pQTcpServer= new QTcpServer();
    pQTcpSocketClient= new QTcpSocket();
    connect(pQcSSLServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(AcceptError(QAbstractSocket::SocketError)));
    connect(pQcSSLServer, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
    connect(pQcSSLServer, SIGNAL(GenericError(QString)), this, SLOT(GenericError(QString)), Qt::QueuedConnection);
    connect(pQTcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(AcceptError(QAbstractSocket::SocketError)));
    connect(pQTcpServer, SIGNAL(newConnection()), this, SLOT(OnNewConnection()));
    QDTLastByteIn= QDateTime::currentDateTime();
    on_QRBPrintableOnly_toggled(ui->QRBPrintableOnly->isChecked());
}

QDTerminal::~QDTerminal() {
    if (pQDCommandsSequence) delete pQDCommandsSequence;
    if (pQLowEnergyService) delete pQLowEnergyService;
    if (pQLowEnergyController) delete pQLowEnergyController;
    if (pQBluetoothDeviceDiscoveryAgent) delete pQBluetoothDeviceDiscoveryAgent;
    delete pQcSSLServer;
    delete pQSslSocketClient;
    for (int count= 0; count< QVTcpSocketsServer.length(); count++) delete QVTcpSocketsServer.at(count);
    QVTcpSocketsServer.clear();
    delete pQTcpServer;
    delete pQTcpSocketClient;
    delete ui;
}

void QDTerminal::AcceptError(QAbstractSocket::SocketError ) {
    switch(Mode) {
        case MODE_TCP_SERVER: {
            int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
            TextCursorSet();
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ pQTcpServer->errorString()+ "<br></font>");
            if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
            else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
            TextCursorSet();
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ pQcSSLServer->errorString()+ "<br></font>");
            if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
            else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
            break;
        }
        default:
            break;
    }
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
            case MODE_BLUETOOTH_LOW_ENERGY: {
                if (LowEnergyCharacteristicWrite.isValid()) {
                    pQLowEnergyService->writeCharacteristic(LowEnergyCharacteristicWrite, KeyPressed.toLocal8Bit(), LowEnergyServiceWriteMode);
                }
                break;
            }
            case MODE_RS232: SerialPort.write(KeyPressed.toLatin1()); break;
            case MODE_TCP_CLIENT: {
                if (pQTcpSocketClient) pQTcpSocketClient->write(KeyPressed.toLatin1());
                break;
            }
            case MODE_TCP_CLIENT_SSL: pQSslSocketClient->write(KeyPressed.toLatin1()); break;
            case MODE_TCP_SERVER: {
                for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(KeyPressed.toLatin1());
                break;
            }
            case MODE_TCP_SERVER_SSL: {
                for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->write(KeyPressed.toLatin1());
                break;
            }
        }
        return true;
    } else return QObject::eventFilter(object, event);
}

void QDTerminal::BluetoothLowEnergyCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue) {
    qDebug() << characteristic.uuid().toString() << newValue << pQLowEnergyService->state() << characteristic.name();
    if (pQLowEnergyService->state()== QLowEnergyService::ServiceDiscovered) {
        QByteArray QBATemp(newValue);
        if (pQDTerminal) pQDTerminal->SendByteArray(QBATemp);
        ShowBufferIn(QBATemp);
    } else if (pQLowEnergyService->state()== QLowEnergyService::DiscoveringServices) {
        ui->QPTELog->textCursor().insertHtml("<font color="+ FontColorWarnings+ ">Characteristic: "+ characteristic.uuid().toString()+ "<br></font>");
    }
}

void QDTerminal::BluetoothLowEnergyCharacteristicRead(const QLowEnergyCharacteristic &, const QByteArray &value) {
    qDebug() << "BluetoothLowEnergyCharacteristicRead()" << value;
}

void QDTerminal::BluetoothLowEnergyCharacteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &newValue) {
    qDebug() << "BluetoothLowEnergyCharacteristicWritten()" << newValue;
}

void QDTerminal::BluetoothLowEnergyConnected() {
    pQLowEnergyController->discoverServices();
}

void QDTerminal::BluetoothLowEnergyDisconnected() {
    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Bluetooth low energy disconnected")+ "<br></font>");
    ui->QPBClose->click();
}

void QDTerminal::BluetoothLowEnergyDiscoveryFinished() {
    QLBluetoothUuids= pQLowEnergyController->services();
    foreach (auto BluetoothUuid, QLBluetoothUuids) {
        ui->QPTELog->textCursor().insertHtml("<font color="+ FontColorWarnings+ ">"+ tr("Service ")+ BluetoothUuid.toString()+ "<br></font>");
    }
    ui->QPTELog->textCursor().insertHtml("<font color="+ FontColorWarnings+ ">"+ tr("Services discovery finished")+ "<br></font>");
    foreach (auto BluetoothUuid, QLBluetoothUuids) {
        if (pQLowEnergyService) delete pQLowEnergyService;
        pQLowEnergyService= pQLowEnergyController->createServiceObject(BluetoothUuid);
        if (pQLowEnergyService) {
            connect(pQLowEnergyService, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)), this, SLOT(BluetoothLowEnergyCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)));
            connect(pQLowEnergyService, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)), this, SLOT(BluetoothLowEnergyCharacteristicRead(QLowEnergyCharacteristic,QByteArray)));
            connect(pQLowEnergyService, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)), this, SLOT(BluetoothLowEnergyCharacteristicWritten(QLowEnergyCharacteristic,QByteArray)));
            connect(pQLowEnergyService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(BluetoothLowEnergyStateChanged(QLowEnergyService::ServiceState)));
            if (pQLowEnergyService->state()== QLowEnergyService::DiscoveryRequired) pQLowEnergyService->discoverDetails();
        }
    }
}

void QDTerminal::BluetoothLowEnergyFinished() {
    ui->QPBSaveProfile->setEnabled(true);
    ui->QPBSaveProfileAs->setEnabled(true);
    QList<QBluetoothDeviceInfo> QLBluetoothDevices= pQBluetoothDeviceDiscoveryAgent->discoveredDevices();
    for (int count= 0; count< QLBluetoothDevices.length(); count++) {
        if (QLBluetoothDevices.at(count).coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
            if (QLBluetoothDevices.at(count).name().compare(BluetoothLowEnergyDevice)== 0) {
                ui->QPTELog->textCursor().insertHtml("<font color="+ FontColorWarnings+ ">"+ tr("Device found, trying to connect and discover services...")+ "<br></font>");
                if (pQLowEnergyController) delete pQLowEnergyController;
                pQLowEnergyController= QLowEnergyController::createCentral(QLBluetoothDevices.at(count));
                connect(pQLowEnergyController, SIGNAL(connected()), this, SLOT(BluetoothLowEnergyConnected()));
                connect(pQLowEnergyController, SIGNAL(disconnected()), this, SLOT(BluetoothLowEnergyDisconnected()));
                connect(pQLowEnergyController, SIGNAL(discoveryFinished()), this, SLOT(BluetoothLowEnergyDiscoveryFinished()));
                pQLowEnergyController->setRemoteAddressType(QLowEnergyController::PublicAddress);
                pQLowEnergyController->connectToDevice();
                ui->QPBClose->setEnabled(true);
                return;
            }
        }
    }
    ui->QPTELog->textCursor().insertHtml("<font color="+ FontColorWarnings+ ">"+ tr("Device not found")+ "<br></font>");
    ui->QPBOpen->setEnabled(true);
}

void QDTerminal::BluetoothLowEnergyRead() {
    qDebug() << "BluetoothLowEnergyRead()";
    if (pQLowEnergyService && LowEnergyCharacteristicRead.isValid()) {
        pQLowEnergyService->readCharacteristic(LowEnergyCharacteristicRead);
        qDebug() << "BluetoothLowEnergyRead() valid";
    }
}

void QDTerminal::BluetoothLowEnergyStateChanged(QLowEnergyService::ServiceState newState) {
    qDebug() << "BluetoothLowEnergyStateChanged()" << newState;
    if (newState== QLowEnergyService::ServiceDiscovered) {
        foreach (QLowEnergyCharacteristic LowEnergyCharacteristic, pQLowEnergyService->characteristics()) {
            if (LowEnergyCharacteristic.isValid()) {
                if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::WriteNoResponse || LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Write) {
                    if (!LowEnergyCharacteristicWrite.isValid()) {
                        LowEnergyCharacteristicWrite= LowEnergyCharacteristic;
                        if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::WriteNoResponse) LowEnergyServiceWriteMode= QLowEnergyService::WriteWithoutResponse;
                        else LowEnergyServiceWriteMode= QLowEnergyService::WriteWithResponse;
                        ui->QPTELog->textCursor().insertHtml("<font color="+ FontColorWarnings+ ">"+ tr("Write characteristic found (WriteWithoutResponse)")+ "<br></font>");
                        ui->QLESend->setEnabled(true);
                        ui->QPBSend->setEnabled(true);
                    }
                }
                if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Read) {
                    if (!LowEnergyCharacteristicRead.isValid()) {
                        LowEnergyCharacteristicRead= LowEnergyCharacteristic;
                        ui->QPTELog->textCursor().insertHtml("<font color="+ FontColorWarnings+ ">"+ tr("Read characteristic found")+ "<br></font>");
                    }
                }
                QLowEnergyDescriptor LowEnergyDescriptor= LowEnergyCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
                if (LowEnergyDescriptor.isValid()) {
                    pQLowEnergyService->writeDescriptor(LowEnergyDescriptor, QByteArray::fromHex("0100"));
                }
            }
        }
    }
}

void QDTerminal::Connected() {
    int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
    TextCursorSet();
    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connected")+ "<br></font>");
    if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
    else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
}

void QDTerminal::Disconnected() {
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: break;
        case MODE_RS232: break;
        case MODE_TCP_CLIENT:
        case MODE_TCP_CLIENT_SSL: {
            ui->QPBClose->click();
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) {
                if (QVTcpSocketsServer.at(count)== static_cast<QTcpSocket *>(QObject::sender())) {
                    int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
                    TextCursorSet();
                    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Disconnected")+ "<br></font>");
                    if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
                    else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
                    QVTcpSocketsServer.removeAt(count);
                    break;
                }
            }
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            for (int count= 0; count< QVSslSocketsServer.length(); count++) {
                if (QVSslSocketsServer.at(count)== static_cast<QSslSocket *>(QObject::sender())) {
                    int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
                    TextCursorSet();
                    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Disconnected")+ "<br></font>");
                    if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
                    else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
                    QVSslSocketsServer.removeAt(count);
                    break;
                }
            }
            break;
        }
    }
}

void QDTerminal::Error(QAbstractSocket::SocketError ) {
    int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
    TextCursorSet();
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: break;
        case MODE_RS232: break;
        case MODE_TCP_CLIENT: {
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ pQTcpSocketClient->errorString()+ "<br></font>");
            ui->QPBClose->click();
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ pQSslSocketClient->errorString()+ "<br></font>");
            ui->QPBClose->click();
            break;
        }
        case MODE_TCP_SERVER: {
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ static_cast<QTcpSocket *>(QObject::sender())->errorString()+ "<br></font>");
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ static_cast<QSslSocket *>(QObject::sender())->errorString()+ "<br></font>");
            break;
        }
    }
    if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
    else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
}

void QDTerminal::GenericError(QString Description) {
    int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
    TextCursorSet();
    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ Description+ "<br></font>");
    if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
    else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
}

void QDTerminal::OnNewConnection() {
    switch(Mode) {
        case MODE_TCP_SERVER: {
            if (pQTcpServer->hasPendingConnections()) {
                QVTcpSocketsServer.append(pQTcpServer->nextPendingConnection());
                int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
                TextCursorSet();
                ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connection from")+ " "+ QVTcpSocketsServer.last()->peerAddress().toString()+ ":"+ QString::number(QVTcpSocketsServer.last()->peerPort())+ "<br></font>");
                connect(QVTcpSocketsServer.last(), SIGNAL(readyRead()), this, SLOT(ReadyRead()));
                connect(QVTcpSocketsServer.last(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
                connect(QVTcpSocketsServer.last(), SIGNAL(disconnected()), this, SLOT(Disconnected()));
                TextCursorSet();
                ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connected")+ "<br></font>");
                if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
                else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
            }
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            if (pQcSSLServer->hasPendingConnections()) {
                QVSslSocketsServer.append(dynamic_cast<QSslSocket*>(pQcSSLServer->nextPendingConnection()));
                int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
                TextCursorSet();
                ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connection from")+ " "+ QVSslSocketsServer.last()->peerAddress().toString()+ ":"+ QString::number(QVSslSocketsServer.last()->peerPort())+ "<br></font>");
                connect(QVSslSocketsServer.last(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
                connect(QVSslSocketsServer.last(), SIGNAL(disconnected()), this, SLOT(Disconnected()));
                connect(QVSslSocketsServer.last(), SIGNAL(peerVerifyError(QSslError)), this, SLOT(PeerVerifyError(QSslError)));
                connect(QVSslSocketsServer.last(), SIGNAL(readyRead()), this, SLOT(ReadyRead()));
                connect(QVSslSocketsServer.last(), SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(SslErrors(QList<QSslError>)));
                TextCursorSet();
                ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("Connected")+ "<br></font>");
                if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
                else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
            }
            break;
        }
        default:
            break;
    }
}

void QDTerminal::OnTimeout() {
    QTControl.stop();
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
        QBAByteIn.append(QBABufferIn);
        ShowBufferIn(QBABufferIn);
    }
    QTControl.start(10);
}

void QDTerminal::on_QCBAutoScroll_toggled(bool ) {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QCBRowCount_toggled(bool ) {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QCBSpecialCharacters_toggled(bool ) {
    on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QGBNewLineAfter_toggled(bool ) {
    ui->QSBNewLineAfterMs->setEnabled(ui->QGBNewLineAfter->isChecked());
    //on_QRBPrintableOnly_clicked();
}

void QDTerminal::on_QLESend_returnPressed() {
    QSLHistory.insert(0, ui->QLESend->text());
    if (QSLHistory.length()> 1) {
        if (QSLHistory.at(0).compare(QSLHistory.at(1))== 0) QSLHistory.removeAt(0);
    }
    while (QSLHistory.length()> 32) QSLHistory.removeLast();
    ui->QCBHistory->clear();
    ui->QCBHistory->addItems(QSLHistory);
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: {
            if (LowEnergyCharacteristicWrite.isValid()) {
                pQLowEnergyService->writeCharacteristic(LowEnergyCharacteristicWrite, ui->QLESend->text().toLatin1(), LowEnergyServiceWriteMode);
                pQLowEnergyService->writeCharacteristic(LowEnergyCharacteristicWrite, QString("\r\n").toLatin1(), LowEnergyServiceWriteMode);
                ui->QLESend->setText("");
            }
            break;
        }
        case MODE_RS232: {
            if (SerialPort.write(QString(ui->QLESend->text()+ "\r\n").toLatin1())) ui->QLESend->setText("");
            break;
        }
        case MODE_TCP_CLIENT: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            pQTcpSocketClient->write(ui->QLESend->text().toLatin1());
            pQTcpSocketClient->write(QString("\r\n").toLatin1());
            ui->QLESend->setText("");
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            pQSslSocketClient->write(ui->QLESend->text().toLatin1());
            pQSslSocketClient->write(QString("\r\n").toLatin1());
            ui->QLESend->setText("");
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(ui->QLESend->text().toLatin1());
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(QString("\r\n").toLatin1());
            ui->QLESend->setText("");
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->write(ui->QLESend->text().toLatin1());
            for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->write(QString("\r\n").toLatin1());
            ui->QLESend->setText("");
            break;
        }
    }
}

void QDTerminal::on_QPBCloseProfile_clicked() {
    for (int count= 0; count< pQVTerminals->count(); count++) {
        if (pQVTerminals->value(count)== this) {
            pQVTerminals->remove(count);
            break;
        }
    }
    pQTBTerminal->removeTab(TabNumber);
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
    QVRows.clear();
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
        case MODE_BLUETOOTH_LOW_ENERGY: {
            QTBluetoothLowEnergyRead.stop();
            pQLowEnergyController->disconnectFromDevice();
            break;
        }
        case MODE_RS232: {
            disconnect(&QTControl, SIGNAL(timeout()), this, SLOT(OnTimeout()));
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
        case MODE_TCP_CLIENT: {
            disconnect(pQTcpSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
            disconnect(pQTcpSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
            disconnect(pQTcpSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
            disconnect(pQTcpSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
            pQTcpSocketClient->disconnectFromHost();
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            disconnect(pQSslSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
            disconnect(pQSslSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
            disconnect(pQSslSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
            disconnect(pQSslSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
            pQSslSocketClient->disconnectFromHost();
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) delete QVTcpSocketsServer.at(count);
            QVTcpSocketsServer.clear();
            pQTcpServer->close();
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            for (int count= 0; count< QVSslSocketsServer.length(); count++) delete QVSslSocketsServer.at(count);
            QVSslSocketsServer.clear();
            pQcSSLServer->close();
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
    _OpenComPort.ui->QLESslCertificate->setText(SslCertificate);
    _OpenComPort.ui->QLESslKeyPrivate->setText(SslKeyPrivate);
    if (SslKeyCertificateEmbedded) _OpenComPort.ui->QRBSslKeyCertificateEmbedded->setChecked(SslKeyCertificateEmbedded);
    else {
        _OpenComPort.ui->QRBSslKeyCertificateCustom->setChecked(true);
        _OpenComPort.ui->QRBSslKeyCertificateEmbedded->setChecked(false);
    }
    _OpenComPort.ui->QSBSocket->setValue(Socket);
    _OpenComPort.ui->QSBTCPServerMaxClients->setValue(MaxClients);
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: _OpenComPort.ui->QRBBluetoothLowEnergy->setChecked(true); break;
        case MODE_RS232: _OpenComPort.ui->QRBRS232->setChecked(true); break;
        case MODE_TCP_CLIENT: _OpenComPort.ui->QRBTCPClient->setChecked(true); break;
        case MODE_TCP_CLIENT_SSL: _OpenComPort.ui->QRBTCPClientSsl->setChecked(true); break;
        case MODE_TCP_SERVER: _OpenComPort.ui->QRBTCPServer->setChecked(true); break;
        case MODE_TCP_SERVER_SSL: _OpenComPort.ui->QRBTCPServerSsl->setChecked(true); break;
    }
    _OpenComPort.ui->QSBBluetoothLowEnergyReadPolling->setValue(BluetoothLowEnergyReadPolling);
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
        BluetoothLowEnergyDevice= _OpenComPort.ui->QCBBluetoothLowEnergyDevices->currentText();
        BluetoothLowEnergyReadPolling= _OpenComPort.ui->QSBBluetoothLowEnergyReadPolling->value();
        ComPort= _OpenComPort.ui->QCBComPort->currentText();
        BaudRate= _OpenComPort.ui->QCBBaudRate->currentText().toInt();
        FlowControl= _OpenComPort.ui->QCBFlowControl->currentIndex();
        MaxClients= _OpenComPort.ui->QSBTCPServerMaxClients->value();
        SendBreak= _OpenComPort.ui->QCBSendBreak->isChecked();
        Server= _OpenComPort.ui->QLEServer->text();
        Socket= static_cast<quint16>(_OpenComPort.ui->QSBSocket->value());
        SslCertificate= _OpenComPort.ui->QLESslCertificate->text();
        SslKeyPrivate= _OpenComPort.ui->QLESslKeyPrivate->text();
        SslKeyCertificateEmbedded= _OpenComPort.ui->QRBSslKeyCertificateEmbedded->isChecked();
        if (_OpenComPort.ui->QRBRS232->isChecked()) Mode= MODE_RS232;
        else if (_OpenComPort.ui->QRBTCPClient->isChecked()) Mode= MODE_TCP_CLIENT;
        else if (_OpenComPort.ui->QRBTCPClientSsl->isChecked()) Mode= MODE_TCP_CLIENT_SSL;
        else if (_OpenComPort.ui->QRBTCPServer->isChecked()) Mode= MODE_TCP_SERVER;
        else if (_OpenComPort.ui->QRBTCPServerSsl->isChecked()) Mode= MODE_TCP_SERVER_SSL;
        if (ConnectionPath.length()> 0) ui->QPBSaveProfile->setEnabled(true);
    }
}

void QDTerminal::on_QPBOpen_clicked() {
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: OpenBluetoothLowEnergy(); break;
        case MODE_RS232: OpenComPort(); break;
        case MODE_TCP_CLIENT: OpenTcpClientPort(); break;
        case MODE_TCP_CLIENT_SSL: OpenTcpClientSslPort(); break;
        case MODE_TCP_SERVER: OpenTcpServerPort(); break;
        case MODE_TCP_SERVER_SSL: OpenTcpServerSslPort(); break;
    }
    RowCount= 1;
}

void QDTerminal::on_QPBRTS_clicked() {
    if (ui->QPBRTS->isChecked()) ui->QPBRTS->setStyleSheet("background-color: red; color: black");
    else ui->QPBRTS->setStyleSheet("background-color: green; color: black");
    SerialPort.setRequestToSend(ui->QPBRTS->isChecked());
}

void QDTerminal::on_QPBSaveProfile_clicked() {
    SaveProfile(ConnectionPath);
}

void QDTerminal::on_QPBSaveProfileAs_clicked() {
    QString Path= QFileDialog::getSaveFileName(this, tr("Save As"), QDir::currentPath(), "QtComPort (*.qcp)");
    if (Path.length()> 0) {
        if (!Path.endsWith(".qcp", Qt::CaseInsensitive)!= 0) Path+= ".qcp";
        ConnectionPath= Path;
        pQTBTerminal->setTabText(TabNumber, ConnectionPath);
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
        case MODE_BLUETOOTH_LOW_ENERGY: {
            if (LowEnergyCharacteristicWrite.isValid()) {
                pQLowEnergyService->writeCharacteristic(LowEnergyCharacteristicWrite, a.toLocal8Bit(), LowEnergyServiceWriteMode);
            }
            break;
        }
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
            pQTcpSocketClient->write(a.toLocal8Bit());
            pQTcpSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            pQSslSocketClient->write(a.toLocal8Bit());
            pQSslSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(a.toLocal8Bit());
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->write(a.toLocal8Bit());
            for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->waitForBytesWritten(2000);
            break;
        }
    }
}

void QDTerminal::on_QPBSendHistory_clicked() {
    if (ui->QCBHistory->currentIndex()> -1) {
        ui->QLESend->setText(ui->QCBHistory->currentText());
        on_QLESend_returnPressed();
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
                case MODE_BLUETOOTH_LOW_ENERGY: {
                    if (LowEnergyCharacteristicWrite.isValid()) {
                        pQLowEnergyService->writeCharacteristic(LowEnergyCharacteristicWrite, QBABufferIn, LowEnergyServiceWriteMode);
                    }
                    break;
                }
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
                    pQTcpSocketClient->write(QBABufferIn, QBABufferIn.size());
                    pQTcpSocketClient->waitForBytesWritten(2000);
                    break;
                }
                case MODE_TCP_CLIENT_SSL: {
                    if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
                    pQSslSocketClient->write(QBABufferIn, QBABufferIn.size());
                    pQSslSocketClient->waitForBytesWritten(2000);
                    break;
                }
                case MODE_TCP_SERVER: {
                    for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(QBABufferIn, QBABufferIn.size());
                    for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->waitForBytesWritten(2000);
                    break;
                }
                case MODE_TCP_SERVER_SSL: {
                    for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->write(QBABufferIn, QBABufferIn.size());
                    for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->waitForBytesWritten(2000);
                    break;
                }
            }
            FileIn.close();
        }
    }
}

void QDTerminal::on_QPBColors_clicked() {
    QDColors Colors(this);
    QPalette Palette= Colors.ui->QLEPreview->palette();
    QPalette PaletteWarnings= Colors.ui->QLEPreviewWarnings->palette();
    Palette.setColor(QPalette::Base, ui->QPTELog->palette().color(QPalette::Base).name());
    Palette.setColor(QPalette::Text, QColor(FontColor));
    PaletteWarnings.setColor(QPalette::Text, QColor(FontColorWarnings));
    PaletteWarnings.setColor(QPalette::Base, ui->QPTELog->palette().color(QPalette::Base).name());
    Colors.ui->QLEPreview->setPalette(Palette);
    Colors.ui->QLEPreviewWarnings->setPalette(PaletteWarnings);
    if (Colors.exec()== QDialog::Accepted) {
        FontColor= Colors.ui->QLEPreview->palette().color(QPalette::Text).name();
        FontColorWarnings= Colors.ui->QLEPreviewWarnings->palette().color(QPalette::Text).name();
        ui->QPBSaveProfile->setEnabled(true);
    }
}

void QDTerminal::on_QPBCommandsSequence_clicked() {
    if (ui->QPBCommandsSequence->isChecked()) pQDCommandsSequence->show();
    else pQDCommandsSequence->hide();
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

void QDTerminal::OpenBluetoothLowEnergy() {
    ui->QPBOpen->setEnabled(false);
    ui->QLConnection->setText(tr("Bluetooth low energy, device: ")+ BluetoothLowEnergyDevice);
    ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ BluetoothLowEnergyDevice+ tr(" device discovery...")+ "<br></font>");
    if (pQBluetoothDeviceDiscoveryAgent) delete pQBluetoothDeviceDiscoveryAgent;
    pQBluetoothDeviceDiscoveryAgent= new QBluetoothDeviceDiscoveryAgent();
    pQBluetoothDeviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    connect(pQBluetoothDeviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(BluetoothLowEnergyFinished()));
    pQBluetoothDeviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
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
            connect(&QTControl, SIGNAL(timeout()), this, SLOT(OnTimeout()));
            QTControl.start(10);
            ui->QLESend->setEnabled(true);
            ui->QPBSend->setEnabled(true);
            ui->QPTELog->installEventFilter(this);
            ui->QPBSaveProfile->setEnabled(true);
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
            int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
            TextCursorSet();
            ui->QPTELog->textCursor().insertHtml("<br><font color="+ FontColorWarnings+ ">"+ tr("The ComPort is already in use!")+ "<br></font>");
            if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
            else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
            ui->QPBOpen->setEnabled(true);
            ui->QPBModify->setEnabled(true);
        }
    } else {
        ui->QPBOpen->setEnabled(true);
        ui->QPBModify->setEnabled(true);
    }
}

void QDTerminal::OpenTcpClientPort() {
    ui->QPBOpen->setEnabled(false);
    ui->QPBModify->setEnabled(false);
    ui->QPBClose->setEnabled(true);
    ui->QLESend->setEnabled(true);
    ui->QPBSend->setEnabled(true);
    ui->QPTELog->installEventFilter(this);
    ui->QPBSaveProfile->setEnabled(true);
    ui->QPBSaveProfileAs->setEnabled(true);
    ui->QLConnection->setText("TCP Client: "+ Server+ ":"+ QString::number(Socket));
    ui->QPBSendFile->setEnabled(true);
    connect(pQTcpSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
    connect(pQTcpSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
    connect(pQTcpSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
    connect(pQTcpSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    pQTcpSocketClient->connectToHost(Server, Socket);
    pQTcpSocketClient->waitForConnected();
}

void QDTerminal::OpenTcpClientSslPort() {
    ui->QPBOpen->setEnabled(false);
    ui->QLConnection->setText("TCP SSL Client: "+ Server+ ":"+ QString::number(Socket));
    ui->QPBModify->setEnabled(false);
    ui->QPBClose->setEnabled(true);
    ui->QLESend->setEnabled(true);
    ui->QPBSend->setEnabled(true);
    ui->QPTELog->installEventFilter(this);
    ui->QPBSaveProfile->setEnabled(true);
    ui->QPBSaveProfileAs->setEnabled(true);
    ui->QLConnection->setText("TCP SSL Client: "+ Server+ ":"+ QString::number(Socket));
    ui->QPBSendFile->setEnabled(true);
    connect(pQSslSocketClient, SIGNAL(connected()), this, SLOT(Connected()));
    connect(pQSslSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Error(QAbstractSocket::SocketError)));
    connect(pQSslSocketClient, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    connect(pQSslSocketClient, SIGNAL(peerVerifyError(QSslError)), this, SLOT(PeerVerifyError(QSslError)));
    connect(pQSslSocketClient, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
    connect(pQSslSocketClient, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(SslErrors(QList<QSslError>)));
    pQSslSocketClient->setPeerVerifyMode(QSslSocket::VerifyPeer);
    pQSslSocketClient->setProtocol(QSsl::AnyProtocol);
    QFile FileIn(SslKeyCertificateEmbedded ? ":/certs/certs/KeyPrivate.key" : SslKeyPrivate);
    if (FileIn.open(QIODevice::ReadOnly)) {
        QSslKey SslKey(FileIn.readAll(), QSsl::Rsa);
        FileIn.close();
        pQSslSocketClient->setPrivateKey(SslKey);
        FileIn.setFileName(SslKeyCertificateEmbedded ? ":/certs/certs/Certificate.crt" : SslCertificate);
        if (FileIn.open(QIODevice::ReadOnly)) {
            QSslCertificate SslLocalCertificate(FileIn.readAll());
            FileIn.close();
            pQSslSocketClient->setLocalCertificate(SslLocalCertificate);
            QList<QSslError> QSLQSslErrors;
            QSLQSslErrors.append(QSslError(QSslError::SelfSignedCertificate, SslLocalCertificate));
            pQSslSocketClient->ignoreSslErrors(QSLQSslErrors);
        } else GenericError(FileIn.errorString());
    } else GenericError(FileIn.errorString());
    pQSslSocketClient->connectToHostEncrypted(Server, Socket);
    if (pQSslSocketClient->waitForConnected()) {
        pQSslSocketClient->waitForEncrypted();
    }
}

void QDTerminal::OpenTcpServerPort() {
    pQTcpServer->setMaxPendingConnections(1);
    if (pQTcpServer->listen(QHostAddress::Any, Socket)) {
        ui->QPBOpen->setEnabled(false);
        ui->QPBModify->setEnabled(false);
        ui->QPBClose->setEnabled(true);
        ui->QLESend->setEnabled(true);
        ui->QPBSend->setEnabled(true);
        ui->QPTELog->installEventFilter(this);
        ui->QPBSaveProfile->setEnabled(true);
        ui->QPBSaveProfileAs->setEnabled(true);
        ui->QLConnection->setText("TCP Server: "+ QString::number(Socket));
        ui->QPBSendFile->setEnabled(true);
    }
}

void QDTerminal::OpenTcpServerSslPort() {
    pQcSSLServer->setMaxPendingConnections(1);
    pQcSSLServer->SslCertificate= SslCertificate;
    pQcSSLServer->SslKeyCertificateEmbedded= SslKeyCertificateEmbedded;
    pQcSSLServer->SslKeyPrivate= SslKeyPrivate;
    if (pQcSSLServer->listen(QHostAddress::Any, Socket)) {
        ui->QPBOpen->setEnabled(false);
        ui->QPBModify->setEnabled(false);
        ui->QPBClose->setEnabled(true);
        ui->QLESend->setEnabled(true);
        ui->QPBSend->setEnabled(true);
        ui->QPTELog->installEventFilter(this);
        ui->QPBSaveProfile->setEnabled(true);
        ui->QPBSaveProfileAs->setEnabled(true);
        ui->QLConnection->setText("TCP SSL Server: "+ QString::number(Socket));
        ui->QPBSendFile->setEnabled(true);
    }
}

void QDTerminal::OnCloseCommandsSequence() {
    ui->QPBCommandsSequence->setChecked(false);
}

void QDTerminal::PeerVerifyError(const QSslError &error) {
    GenericError(error.errorString());
}

void QDTerminal::ReadConfigurationFile() {
    QSettings Settings(ConnectionPath, QSettings::IniFormat);
    Settings.beginGroup("Main");
    BluetoothLowEnergyDevice= Settings.value("BluetoothLowEnergyDevice", "").toString();
    BluetoothLowEnergyReadPolling= Settings.value("BluetoothLowEnergyReadPolling", "100").toInt();
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
    SslCertificate= Settings.value("SslCertificate", "").toString();
    SslKeyPrivate= Settings.value("SslKeyPrivate", "").toString();
    SslKeyCertificateEmbedded= Settings.value("SslKeyCertificateEmbedded", true).toBool();
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
    ui->QCBAutoScroll->setChecked(Settings.value("AutoScroll", true).toBool());
    ui->QGBNewLineAfter->setChecked(Settings.value("NewLineAfter", false).toBool());
    ui->QSBNewLineAfterMs->setValue(Settings.value("NewLineAfterMs", 1000).toInt());
    ui->QCBTimestamp->setChecked(Settings.value("Timestamp", false).toBool());
    ui->QCBRowCount->setChecked(Settings.value("RowCount", false).toBool());
    ui->QCBSpecialCharacters->setChecked(Settings.value("SpecialCharacters", false).toBool());
    ui->QRBCR->setChecked(Settings.value("NewLineWidth", "lf").toString().compare("cr")== 0);
    ui->QRBLF->setChecked(Settings.value("NewLineWidth", "lf").toString().compare("lf")== 0);
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: OpenBluetoothLowEnergy(); break;
        case MODE_RS232: OpenComPort(); break;
        case MODE_TCP_CLIENT: OpenTcpClientPort(); break;
        case MODE_TCP_CLIENT_SSL: OpenTcpClientSslPort(); break;
        case MODE_TCP_SERVER: OpenTcpServerPort(); break;
        case MODE_TCP_SERVER_SSL: OpenTcpServerSslPort(); break;
    }
}

void QDTerminal::ReadyRead() {
    QByteArray QBABufferIn;
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: break;
        case MODE_RS232: break;
        case MODE_TCP_CLIENT: QBABufferIn= pQTcpSocketClient->readAll(); break;
        case MODE_TCP_CLIENT_SSL: QBABufferIn= pQSslSocketClient->readAll(); break;
        case MODE_TCP_SERVER: QBABufferIn= static_cast<QTcpSocket *>(QObject::sender())->readAll(); break;
        case MODE_TCP_SERVER_SSL: QBABufferIn= static_cast<QSslSocket *>(QObject::sender())->readAll(); break;
    }
    if (QBABufferIn.length()> 0) {
        if (pQDTerminal) pQDTerminal->SendByteArray(QBABufferIn);
        QBAByteIn.append(QBABufferIn);
        ShowBufferIn(QBABufferIn);
    }
}

void QDTerminal::SaveProfile(QString ConnectionPath) {
    QSettings Settings(ConnectionPath, QSettings::IniFormat);
    Settings.beginGroup("Main");
    Settings.setValue("AutoScroll", ui->QCBAutoScroll->isChecked());
    Settings.setValue("BluetoothLowEnergyDevice", BluetoothLowEnergyDevice);
    Settings.setValue("BluetoothLowEnergyReadPolling", BluetoothLowEnergyReadPolling);
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
    Settings.setValue("NewLineAfter", ui->QGBNewLineAfter->isChecked());
    Settings.setValue("NewLineAfterMs", ui->QSBNewLineAfterMs->value());
    Settings.setValue("NewLineWidth", ui->QRBCR->isChecked() ? "cr" : "lf");
    Settings.setValue("Parity", QString(Parity));
    Settings.setValue("RowCount", ui->QCBRowCount->isChecked());
    Settings.setValue("SendBreak", SendBreak);
    Settings.setValue("Server", Server);
    Settings.setValue("Socket", Socket);
    Settings.setValue("SpecialCharacters", ui->QCBSpecialCharacters->isChecked());
    Settings.setValue("SslCertificate", SslCertificate);
    Settings.setValue("SslKeyPrivate", SslKeyPrivate);
    Settings.setValue("SslKeyCertificateEmbedded", SslKeyCertificateEmbedded);
    Settings.setValue("TerminalLogFormat", TerminalLogFormat);
    Settings.setValue("Timestamp", ui->QCBTimestamp->isChecked());
    if (ui->QRBPrintableOnly->isChecked()) Settings.setValue("ViewMode", 0);
    else if (ui->QRBSym->isChecked()) Settings.setValue("ViewMode", 1);
    else if (ui->QRBDec->isChecked()) Settings.setValue("ViewMode", 2);
    else if (ui->QRBHex->isChecked()) Settings.setValue("ViewMode", 3);
    else if (ui->QRBBin->isChecked()) Settings.setValue("ViewMode", 4);
    Settings.endGroup();
    Settings.beginGroup("CommandsSequence");
    int Index= 0;
    for (int count= 0; count< pQDCommandsSequence->ui->QTWCommands->rowCount(); count++) {
        if (((QLineEdit*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(count, 2)))->text().length()) {
            Settings.setValue("Command"+ QString::number(Index), ((QLineEdit*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(count, 2)))->text());
            Settings.setValue("CommandType"+ QString::number(Index), ((QCheckBox*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(count, 1)))->isChecked() ? 1 : 0);
            Settings.setValue("Delay"+ QString::number(Index), ((QSpinBox*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(count, 3)))->value());
            Settings.setValue("Enabled"+ QString::number(Index), ((QCheckBox*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(count, 0)))->isChecked());
            Index++;
        }
    }
}

void QDTerminal::ShowBufferIn(QByteArray &QBABufferIn) {
    int VerticalScrollBarValue= ui->QPTELog->verticalScrollBar()->value();
    TextCursorSet();
    if (QVRows.length()== 0) {
        Row row;
        QVRows.append(row);
    }
    Row *pRow= &QVRows.last();
    if (ui->QRBPrintableOnly->isChecked() || ui->QRBSym->isChecked()) {
        QStringList QSLBufferIn= QString(QBABufferIn).split(ui->QRBLF->isChecked() ? '\n' : '\r');
        for (int count= 0; count< QSLBufferIn.length(); count++) {
            if (count> 0) {
                Row row;
                QVRows.append(row);
                pRow= &QVRows.last();
            }
            if (ui->QGBNewLineAfter->isChecked()) {
                if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                    if (pRow->Value.length()> 0) {
                        Row row;
                        QVRows.append(row);
                        pRow= &QVRows.last();
                    }
                }
            }
            pRow->Value+= QSLBufferIn.at(count);
            if (pRow->Value.length()> 0 && pRow->QDTRow.isNull()) pRow->QDTRow= QDateTime::currentDateTime();
        }
    } else {
        if (ui->QGBNewLineAfter->isChecked()) {
            if (QDTLastByteIn.msecsTo(QDateTime::currentDateTime())>= ui->QSBNewLineAfterMs->value()) {
                if (pRow->QBAValue.length()> 0) {
                    Row row;
                    QVRows.append(row);
                    pRow= &QVRows.last();
                }
            }
        }
        for (int count= 0; count< QBABufferIn.length(); count++) {
            pRow->QBAValue.append(QBABufferIn.at(count));
            if (pRow->QBAValue.length()> 0 && pRow->QDTRow.isNull()) pRow->QDTRow= QDateTime::currentDateTime();
            if (QBABufferIn.at(count)== (ui->QRBLF->isChecked() ? '\n' : '\r')) {
                Row row;
                QVRows.append(row);
                pRow= &QVRows.last();
            }
        }
    }
    while (QVRows.length()> 2048) QVRows.removeFirst();
    QString RowsOut;
    for (int count= 0; count< QVRows.length(); count++) {
        if (QVRows.at(count).RowCount== 0) {
            QVRows[count].RowCount= RowCount;
            RowCount++;
        }
        QString Prefix;
        if (ui->QCBRowCount->isChecked()) Prefix+= "->"+ QString::number(QVRows.at(count).RowCount).rightJustified(4, '0')+ "<-";
        if (ui->QCBTimestamp->isChecked()) Prefix+= "->"+ QVRows.at(count).QDTRow.toString("hh:mm:ss.zzz")+ "<-";
        if (ui->QRBBin->isChecked()) {
            QString Value;
            for (int count_2= 0; count_2< QVRows.at(count).Value.length(); count_2++) {
                switch (TerminalLogFormat) {
                    case TERMINAL_LOG_FORMAT_COMPACT: Value+= QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 2).rightJustified(8, '0'); break;
                    case TERMINAL_LOG_FORMAT_FULL: Value+= "["+ QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 2).rightJustified(8, '0')+ "]"; break;
                    case TERMINAL_LOG_FORMAT_SEPARATOR: Value+= QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 2).rightJustified(8, '0')+ ","; break;
                }
            }
            if (Value.length()) {
                if (QVRows.at(count).Color.length()> 0) RowsOut+= "<div style='color: "+ QVRows.at(count).Color+ "'>"+ Prefix.toHtmlEscaped()+ Value.toHtmlEscaped()+ "</div>";
                else RowsOut+= "<div style='color: "+ FontColor+ "'>"+ Prefix.toHtmlEscaped()+ Value.toHtmlEscaped()+ "</div>";
            }
        } else if (ui->QRBDec->isChecked()) {
            QString Value;
            for (int count_2= 0; count_2< QVRows.at(count).Value.length(); count_2++) {
                switch (TerminalLogFormat) {
                    case TERMINAL_LOG_FORMAT_COMPACT: Value+= QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 10).rightJustified(3, '0'); break;
                    case TERMINAL_LOG_FORMAT_FULL: Value+= "["+ QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 10).rightJustified(3, '0')+ "]"; break;
                    case TERMINAL_LOG_FORMAT_SEPARATOR: Value+= QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 10).rightJustified(3, '0')+ ","; break;
                }
            }
            if (Value.length()) {
                if (QVRows.at(count).Color.length()> 0) RowsOut+= "<div style='color: "+ QVRows.at(count).Color+ "'>"+ Prefix.toHtmlEscaped()+ Value.toHtmlEscaped()+ "</div>";
                else RowsOut+= "<div style='color: "+ FontColor+ "'>"+ Prefix.toHtmlEscaped()+ Value.toHtmlEscaped()+ "</div>";
            }
        } else if (ui->QRBHex->isChecked()) {
            QString Value;
            for (int count_2= 0; count_2< QVRows.at(count).QBAValue.length(); count_2++) {
                switch (TerminalLogFormat) {
                    case TERMINAL_LOG_FORMAT_COMPACT: Value+= QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 16).rightJustified(2, '0'); break;
                    case TERMINAL_LOG_FORMAT_FULL: Value+= "["+ QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 16).rightJustified(2, '0')+ "]"; break;
                    case TERMINAL_LOG_FORMAT_SEPARATOR: Value+= QString::number(uint8_t(QVRows.at(count).QBAValue.at(count_2)), 16).rightJustified(2, '0')+ ","; break;
                }
            }
            if (Value.length()) {
                if (QVRows.at(count).Color.length()> 0) RowsOut+= "<div style='color: "+ QVRows.at(count).Color+ "'>"+ Prefix.toHtmlEscaped()+ Value.toHtmlEscaped()+ "</div>";
                else RowsOut+= "<div style='color: "+ FontColor+ "'>"+ Prefix.toHtmlEscaped()+ Value.toHtmlEscaped()+ "</div>";
            }
        } else if (ui->QRBPrintableOnly->isChecked()) {
            if (ui->QCBSpecialCharacters->isChecked()) {
                QVRows[count].Value.replace(QChar(0), "[NULL]");
                QVRows[count].Value.replace(QChar(1), "[SOH]");
                QVRows[count].Value.replace(QChar(2), "[STX]");
                QVRows[count].Value.replace(QChar(3), "[ETX]");
                QVRows[count].Value.replace(QChar(4), "[EOT]");
                QVRows[count].Value.replace(QChar(6), "[ACK]");
                QVRows[count].Value.replace(QChar(7), "[BEEP]");
                QVRows[count].Value.replace(QChar(11), "[VT]");
                QVRows[count].Value.replace(QChar(12), "[FF]");
                QVRows[count].Value.replace(QChar(14), "[SO]");
                QVRows[count].Value.replace(QChar(15), "[SI]");
                QVRows[count].Value.replace(QChar(16), "[DLE]");
                QVRows[count].Value.replace(QChar(17), "[DC1]");
                QVRows[count].Value.replace(QChar(18), "[DC2]");
                QVRows[count].Value.replace(QChar(19), "[DC3]");
                QVRows[count].Value.replace(QChar(20), "[DC4]");
                QVRows[count].Value.replace(QChar(21), "[NACK]");
                QVRows[count].Value.replace(QChar(22), "[SYN]");
                QVRows[count].Value.replace(QChar(23), "[ETB]");
                QVRows[count].Value.replace(QChar(24), "[CAN]");
                QVRows[count].Value.replace(QChar(25), "[EM]");
                QVRows[count].Value.replace(QChar(26), "[SUB]");
                QVRows[count].Value.replace(QChar(27), "[ESC]");
            } else {
                if (QVRows.at(count).Value.indexOf(QChar(27))> -1) {
                    QString Color= QVRows.at(count).Value.midRef(QVRows.at(count).Value.indexOf(QChar(27))+ 1, 16).toString();
                    for (int count_2= 0; count_2< Color.length(); count_2++) {
                        if (Color.at(count_2)== QChar(109)) {
                            Color.remove(count_2, Color.length());
                            QVRows[count].Value.remove(QVRows.at(count).Value.indexOf(QChar(27)), Color.length()+ 1+ 1);
                            if (Color.startsWith("[")) {
                                Color.remove(0, 1);
                                QStringList QSLParameters= Color.split(";");
                                QString Color= "0";
                                if (QSLParameters.length()> 1) {
                                    Color= QSLParameters.at(1);
                                    switch(Color.toInt(nullptr)) {
                                        case 30: QVRows[count].Color= ""; break;
                                        case 31: QVRows[count].Color= "red"; break;
                                        case 32: QVRows[count].Color= "green"; break;
                                        case 33: QVRows[count].Color= "yellow"; break;
                                        case 34: QVRows[count].Color= "blue"; break;
                                        case 35: QVRows[count].Color= "magenta"; break;
                                        case 36: QVRows[count].Color= "cyan"; break;
                                        case 37: QVRows[count].Color= "white"; break;
                                        case 90: QVRows[count].Color= "brigh black"; break;
                                        case 91: QVRows[count].Color= "brigh red"; break;
                                        case 92: QVRows[count].Color= "brigh green"; break;
                                        case 93: QVRows[count].Color= "brigh yellow"; break;
                                        case 94: QVRows[count].Color= "brigh blue"; break;
                                        case 95: QVRows[count].Color= "brigh magenta"; break;
                                        case 96: QVRows[count].Color= "brigh cyan"; break;
                                        case 97: QVRows[count].Color= "brigh white"; break;
                                        default: QVRows[count].Color= ""; break;
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }
            if (QVRows.at(count).Color.length()> 0) RowsOut+= "<div style='color: "+ QVRows.at(count).Color+ "'>"+ Prefix.toHtmlEscaped()+ QVRows.at(count).Value.toHtmlEscaped()+ "</div>";
            else RowsOut+= "<div style='color: "+ FontColor+ "'>"+ Prefix.toHtmlEscaped()+ QVRows.at(count).Value.toHtmlEscaped()+ "</div>";
        } else if (ui->QRBSym->isChecked()) {
            QVRows[count].Value.replace(QChar(0), "[NUL]");
            QVRows[count].Value.replace(QChar(1), "[SOH]");
            QVRows[count].Value.replace(QChar(2), "[STX]");
            QVRows[count].Value.replace(QChar(3), "[ETX]");
            QVRows[count].Value.replace(QChar(4), "[EOT]");
            QVRows[count].Value.replace(QChar(5), "[ENQ]");
            QVRows[count].Value.replace(QChar(6), "[ACQ]");
            QVRows[count].Value.replace(QChar(7), "[BEL]");
            QVRows[count].Value.replace(QChar(8), "[BS]");
            QVRows[count].Value.replace(QChar(9), "[TAB]");
            QVRows[count].Value.replace(QChar(10), "[LF]");
            QVRows[count].Value.replace(QChar(11), "[VT]");
            QVRows[count].Value.replace(QChar(12), "[FF]");
            QVRows[count].Value.replace(QChar(13), "[CR]");
            QVRows[count].Value.replace(QChar(14), "[SO]");
            QVRows[count].Value.replace(QChar(15), "[SI]");
            QVRows[count].Value.replace(QChar(16), "[DLE]");
            QVRows[count].Value.replace(QChar(17), "[DC1]");
            QVRows[count].Value.replace(QChar(18), "[DC2]");
            QVRows[count].Value.replace(QChar(19), "[DC3]");
            QVRows[count].Value.replace(QChar(20), "[DC4]");
            QVRows[count].Value.replace(QChar(21), "[NAK]");
            QVRows[count].Value.replace(QChar(22), "[SYN]");
            QVRows[count].Value.replace(QChar(23), "[ETB]");
            QVRows[count].Value.replace(QChar(24), "[CAN]");
            QVRows[count].Value.replace(QChar(25), "[EM]");
            QVRows[count].Value.replace(QChar(26), "[SUB]");
            QVRows[count].Value.replace(QChar(27), "[ESC]");
            QVRows[count].Value.replace(QChar(28), "[FS]");
            QVRows[count].Value.replace(QChar(29), "[GS]");
            QVRows[count].Value.replace(QChar(30), "[RS]");
            QVRows[count].Value.replace(QChar(31), "[US]");
            if (QVRows.at(count).Color.length()> 0) RowsOut+= "<div style='color: "+ QVRows.at(count).Color+ "'>"+ Prefix.toHtmlEscaped()+ QVRows.at(count).Value.toHtmlEscaped()+ "</div>";
            else RowsOut+= "<div style='color: "+ FontColor+ "'>"+ Prefix.toHtmlEscaped()+ QVRows.at(count).Value.toHtmlEscaped()+ "</div>";
        }
    }
    ui->QPTELog->clear();
    ui->QPTELog->appendHtml(RowsOut);
    if (ui->QCBAutoScroll->isChecked()) ui->QPTELog->verticalScrollBar()->setValue(ui->QPTELog->verticalScrollBar()->maximum());
    else ui->QPTELog->verticalScrollBar()->setValue(VerticalScrollBarValue);
    QDTLastByteIn= QDateTime::currentDateTime();
}

void QDTerminal::showEvent(QShowEvent* ) {
    if (!pQDCommandsSequence) {
        pQDCommandsSequence= new QDCommandsSequence(this);
        connect(pQDCommandsSequence, SIGNAL(OnClose()), this, SLOT(OnCloseCommandsSequence()));
        QSettings Settings(ConnectionPath, QSettings::IniFormat);
        Settings.beginGroup("CommandsSequence");
        int Index= 0;
        while (true) {
            QString Command= Settings.value("Command"+ QString::number(Index), "").toString();
            if (Command.compare("")== 0 && Index< pQDCommandsSequence->ui->QTWCommands->rowCount()) break;
            else {
                ((QLineEdit*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(Index, 2)))->setText(Command);
                ((QCheckBox*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(Index, 1)))->setChecked(Settings.value("CommandType"+ QString::number(Index), 0).toInt()== 1);
                ((QSpinBox*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(Index, 3)))->setValue(Settings.value("Delay"+ QString::number(Index), 500).toInt());
                ((QCheckBox*)(pQDCommandsSequence->ui->QTWCommands->cellWidget(Index, 0)))->setChecked(Settings.value("Enabled"+ QString::number(Index), false).toBool());
                Index++;
            }
        }
    }
}

void QDTerminal::SendByteArray(QByteArray QBABufferIn) {
    switch(Mode) {
        case MODE_BLUETOOTH_LOW_ENERGY: {
            if (LowEnergyCharacteristicWrite.isValid()) {
                pQLowEnergyService->writeCharacteristic(LowEnergyCharacteristicWrite, QBABufferIn, LowEnergyServiceWriteMode);
            }
            break;
        }
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
            pQTcpSocketClient->write(QBABufferIn, QBABufferIn.size());
            pQTcpSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_CLIENT_SSL: {
            if (ui->QPBOpen->isEnabled()) ui->QPBOpen->click();
            pQSslSocketClient->write(QBABufferIn, QBABufferIn.size());
            pQSslSocketClient->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_SERVER: {
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->write(QBABufferIn, QBABufferIn.size());
            for (int count= 0; count< QVTcpSocketsServer.length(); count++) QVTcpSocketsServer.at(count)->waitForBytesWritten(2000);
            break;
        }
        case MODE_TCP_SERVER_SSL: {
            for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->write(QBABufferIn, QBABufferIn.size());
            for (int count= 0; count< QVSslSocketsServer.length(); count++) QVSslSocketsServer.at(count)->waitForBytesWritten(2000);
            break;
        }
    }
}

void QDTerminal::SslErrors(const QList<QSslError> &errors) {
    for (int count= 0; count< errors.count(); count++) GenericError(errors.at(count).errorString());
}

void QDTerminal::TextCursorSet() {
    QTextCursor Cursor= QTextCursor(ui->QPTELog->document());
    Cursor.movePosition(QTextCursor::End);
    ui->QPTELog->setTextCursor(Cursor);
}
