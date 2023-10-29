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

#include "qdopencomport.h"
#include "ui_qdopencomport.h"

QDOpenComPort::QDOpenComPort(QWidget *parent) : QDialog(parent), ui(new Ui::QDOpenComPort) {
    ui->setupUi(this);
    ui->QCBBaudRate->addItem("1200");
    ui->QCBBaudRate->addItem("2400");
    ui->QCBBaudRate->addItem("4800");
    ui->QCBBaudRate->addItem("9600");
    ui->QCBBaudRate->addItem("19200");
    ui->QCBBaudRate->addItem("38400");
    ui->QCBBaudRate->addItem("57600");
    ui->QCBBaudRate->addItem("115200");
    ui->QCBBaudRate->addItem("230400");
    ui->QCBBaudRate->addItem("460800");
    ui->QCBBaudRate->addItem("576000");
    ui->QCBBaudRate->addItem("921600");
    ui->QCBBaudRate->addItem("2000000");
    ui->QCBBaudRate->setCurrentIndex(3);
    ui->QCBParity->addItem("None");
    ui->QCBParity->addItem("Even");
    ui->QCBParity->addItem("Odd");
    ui->QCBParity->addItem("Mark");
    ui->QCBParity->addItem("Space");
    ui->QCBParity->setCurrentIndex(0);
    ui->QCBDataBits->addItem("5");
    ui->QCBDataBits->addItem("6");
    ui->QCBDataBits->addItem("7");
    ui->QCBDataBits->addItem("8");
    ui->QCBDataBits->setCurrentIndex(3);
    ui->QCBStopBits->addItem("1");
    ui->QCBStopBits->addItem("1.5");
    ui->QCBStopBits->addItem("2");
    ui->QCBStopBits->setCurrentIndex(0);
    ui->QCBFlowControl->addItem("None");
    ui->QCBFlowControl->addItem("RTS/CTS");
    ui->QCBFlowControl->addItem("XON/XOFF");
    ui->QCBFlowControl->setCurrentIndex(0);
    on_QSBSocket_valueChanged(ui->QSBSocket->value());
    ui->tabWidget->tabBar()->hide();
    ui->QSBSocket->installEventFilter(this);
    ui->QWKeyAndCertificate->hide();
    pQBluetoothDeviceDiscoveryAgent= new QBluetoothDeviceDiscoveryAgent();
    pQBluetoothDeviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);
    connect(pQBluetoothDeviceDiscoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(BluetoothLowEnergyDeviceDiscovered(QBluetoothDeviceInfo)));
    connect(pQBluetoothDeviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)), this, SLOT(BluetoothLowEnergyError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(pQBluetoothDeviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(BluetoothLowEnergyFinished()));
}

QDOpenComPort::~QDOpenComPort() {
    if (pQLowEnergyController) delete pQLowEnergyController;
    if (pQLowEnergyService) delete pQLowEnergyService;
    if (pQBluetoothDeviceDiscoveryAgent) delete pQBluetoothDeviceDiscoveryAgent;
    delete ui;
}

void QDOpenComPort::LowEnergyCharacteristicsParse(Service &service) {
    foreach (QLowEnergyCharacteristic LowEnergyCharacteristic, service.QLowEnergyCharacteristics) {
        if (LowEnergyCharacteristic.isValid()) {
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::WriteNoResponse) qDebug() << "QLowEnergyCharacteristic::WriteNoResponse";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Unknown) qDebug() << "QLowEnergyCharacteristic::Unknown";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Broadcasting) qDebug() << "QLowEnergyCharacteristic::Broadcasting";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Read) qDebug() << "QLowEnergyCharacteristic::Read";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::WriteNoResponse) qDebug() << "QLowEnergyCharacteristic::WriteNoResponse";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Write) qDebug() << "QLowEnergyCharacteristic::Write";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Notify) qDebug() << "QLowEnergyCharacteristic::Notify";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Indicate) qDebug() << "QLowEnergyCharacteristic::Indicate";
            if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::WriteSigned) qDebug() << "QLowEnergyCharacteristic::WriteSigned";
        }
        if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::WriteNoResponse || LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Write) {
            /*LowEnergyCharacteristicWrite= LowEnergyCharacteristic;
                if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::WriteNoResponse) LowEnergyServiceWriteMode= QLowEnergyService::WriteWithoutResponse;
                else LowEnergyServiceWriteMode= QLowEnergyService::WriteWithResponse;*/
            ui->QCBCharacteristicWrite->setChecked(true);
        }
        if (LowEnergyCharacteristic.properties() & QLowEnergyCharacteristic::Read) {
            //LowEnergyCharacteristicRead= LowEnergyCharacteristic;
            /*connect(&Timer, SIGNAL(timeout()), this, SLOT(BluetoothLowEnergyOnRead()));
                Timer.start(1000);*/
            ui->QCBCharacteristicRead->setChecked(true);
        }
    }
}

void QDOpenComPort::BluetoothLowEnergyCharacteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value) {
    qDebug() << "BluetoothLowEnergyCharacteristicRead()" << value;
}

void QDOpenComPort::BluetoothLowEnergyCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue) {
    qDebug() << "BluetoothLowEnergyCharacteristicWritten()" << newValue;
}

void QDOpenComPort::BluetoothLowEnergyConnected() {
    pQLowEnergyController->discoverServices();
}

void QDOpenComPort::BluetoothLowEnergyDeviceDiscovered(const QBluetoothDeviceInfo &) {}

void QDOpenComPort::BluetoothLowEnergyDisconnected() {
    qDebug() << "Disconnected()";
}

void QDOpenComPort::BluetoothLowEnergyDiscoveryFinished() {
    qDebug() << "BluetoothLowEnergyDiscoveryFinished()";
    QLBluetoothUuids= pQLowEnergyController->services();
    foreach (auto BluetoothUuid, pQLowEnergyController->services()) {
        ui->QCBBluetoothLowEnergyServices->addItem(BluetoothUuid.toString());
    }
    ui->QPBBluetoothLowEnergyConnectCharacteristicsGet->setEnabled(QLBluetoothUuids.length()> 0);
}

void QDOpenComPort::BluetoothLowEnergyError(QLowEnergyController::Error ) {
    qDebug() << "Error: " << pQLowEnergyController->errorString();
}

void QDOpenComPort::BluetoothLowEnergyError(QBluetoothDeviceDiscoveryAgent::Error error) {
    if (error== QBluetoothDeviceDiscoveryAgent::PoweredOffError) qDebug() << "The Bluetooth adaptor is powered off, power it on before doing discovery.";
    else if (error== QBluetoothDeviceDiscoveryAgent::InputOutputError) qDebug() << "Writing or reading from the device resulted in an error.";
    else {
        QMetaEnum qme= pQBluetoothDeviceDiscoveryAgent->metaObject()->enumerator(pQBluetoothDeviceDiscoveryAgent->metaObject()->indexOfEnumerator("Error"));
        qDebug() << "Error:" << QLatin1String(qme.valueToKey(error));
    }
}

void QDOpenComPort::BluetoothLowEnergyFinished() {
    QLDevices= pQBluetoothDeviceDiscoveryAgent->discoveredDevices();
    for (int count= 0; count< QLDevices.length(); count++) {
        if (QLDevices.at(count).coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
            ui->QCBBluetoothLowEnergyDevices->addItem(QLDevices.at(count).name()/*+ " "+ QLDevices.at(count).address().toString()*/);
        }
    }
    ui->QPBBluetoothLowEnergyConnect->setEnabled(QLDevices.length()> 0);
}

/*void QDOpenComPort::BluetoothLowEnergyOnRead() {
    if (pQLowEnergyService && LowEnergyCharacteristicRead.isValid()) pQLowEnergyService->readCharacteristic(LowEnergyCharacteristicRead);
}*/

void QDOpenComPort::BluetoothLowEnergyServiceDiscovered(const QBluetoothUuid &) {
    qDebug() << "ServiceDiscovered()";
}

void QDOpenComPort::BluetoothLowEnergyStateChanged(QLowEnergyService::ServiceState newState) {
    qDebug() << "BluetoothLowEnergyStateChanged()" << newState;
    if (newState== QLowEnergyService::ServiceDiscovered) {
        Service service;
        service.Index= ui->QCBBluetoothLowEnergyServices->currentIndex();
        service.QLowEnergyCharacteristics= pQLowEnergyService->characteristics();
        LowEnergyCharacteristicsParse(service);
        QVServices.append(service);
    }
}

bool QDOpenComPort::eventFilter(QObject *object, QEvent *event) {
    if (event->type()== QEvent::KeyPress && object== ui->QSBSocket) {
        if (static_cast<QKeyEvent*>(event)->key()== Qt::Key_Enter) {
            ui->QPBOk->click();
            return true;
        }
    }
    return false;
}

void QDOpenComPort::on_QCBComPort_currentIndexChanged(int index) {
    if (index> -1) ui->QPBOk->setEnabled(true);
}

void QDOpenComPort::on_QLESslCertificate_textChanged(const QString &arg1) {
    on_QLEServer_textChanged(arg1);
}

void QDOpenComPort::on_QLESslKeyPrivate_textChanged(const QString &arg1) {
    on_QLEServer_textChanged(arg1);
}

void QDOpenComPort::on_QLEServer_returnPressed() {
    ui->QPBOk->click();
}

void QDOpenComPort::on_QLEServer_textChanged(QString ) {
    if (ui->QLEServer->text().length()> 0) ui->QPBOk->setEnabled(true);
    //else ui->QPBOk->setEnabled(false);
}

void QDOpenComPort::on_QPBOk_clicked() {
    this->accept();
}

void QDOpenComPort::on_QPBBluetoothLowEnergyConnectCharacteristicsGet_clicked() {
    ui->QCBCharacteristicRead->setChecked(false);
    ui->QCBCharacteristicWrite->setChecked(false);
    for (int count= 0; count< QVServices.size(); count++) {
        if (QVServices.at(count).Index== ui->QCBBluetoothLowEnergyServices->currentIndex()) {
            LowEnergyCharacteristicsParse(QVServices[count]);
            return;
        }
    }
    if (pQLowEnergyService) delete pQLowEnergyService;
    pQLowEnergyService= pQLowEnergyController->createServiceObject(QLBluetoothUuids.at(ui->QCBBluetoothLowEnergyServices->currentIndex()));
    connect(pQLowEnergyService, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)), this, SLOT(BluetoothLowEnergyCharacteristicRead(QLowEnergyCharacteristic,QByteArray)));
    connect(pQLowEnergyService, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)), this, SLOT(BluetoothLowEnergyCharacteristicWritten(QLowEnergyCharacteristic,QByteArray)));
    connect(pQLowEnergyService, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(BluetoothLowEnergyStateChanged(QLowEnergyService::ServiceState)));
    if (pQLowEnergyService->state()== QLowEnergyService::DiscoveryRequired) pQLowEnergyService->discoverDetails();
}

void QDOpenComPort::on_QPBBluetoothLowEnergyConnect_clicked() {
    ui->QCBBluetoothLowEnergyServices->clear();
    ui->QCBCharacteristicRead->setChecked(false);
    ui->QCBCharacteristicWrite->setChecked(false);
    QVServices.clear();
    if (pQLowEnergyController) delete pQLowEnergyController;
    pQLowEnergyController= QLowEnergyController::createCentral(QLDevices.at(ui->QCBBluetoothLowEnergyDevices->currentIndex()));
    connect(pQLowEnergyController, SIGNAL(connected()), this, SLOT(BluetoothLowEnergyConnected()));
    connect(pQLowEnergyController, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(BluetoothLowEnergyError(QLowEnergyController::Error)));
    connect(pQLowEnergyController, SIGNAL(disconnected()), this, SLOT(BluetoothLowEnergyDisconnected()));
    connect(pQLowEnergyController, SIGNAL(serviceDiscovered(QBluetoothUuid)), this, SLOT(BluetoothLowEnergyServiceDiscovered(QBluetoothUuid)));
    connect(pQLowEnergyController, SIGNAL(discoveryFinished()), this, SLOT(BluetoothLowEnergyDiscoveryFinished()));
    pQLowEnergyController->setRemoteAddressType(QLowEnergyController::PublicAddress);
    pQLowEnergyController->connectToDevice();
}

void QDOpenComPort::on_QPBBluetoothLowEnergyScan_clicked() {
    ui->QPBBluetoothLowEnergyConnect->setEnabled(false);
    ui->QCBBluetoothLowEnergyDevices->clear();
    ui->QCBBluetoothLowEnergyServices->clear();
    ui->QCBCharacteristicRead->setChecked(false);
    ui->QCBCharacteristicWrite->setChecked(false);
    QVServices.clear();
    pQBluetoothDeviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void QDOpenComPort::on_QPBCancel_clicked() {
    this->reject();
}

void QDOpenComPort::on_QPBHelp_clicked() {
    QMessageBox::information(this, tr("How to generate self signed keys and certifcate"), "<br><br>"
                                                   "<b>" + tr("Private key")+ "</b><br>openssl genrsa -out KeyPrivate.key 2048<br><br>"
                                                   "<b>" + tr("Public key")+ "</b><br>openssl rsa -in KeyPrivate.key -pubout -out KeyPublic.key<br><br>"
                                                   "<b>" + tr("Certificate")+ "</b><br>openssl req -new -x509 -key KeyPrivate.key -out Certificate.crt -days 360<br><br>"
                                                   "<b>" + tr("Extract the certificate from remote server")+ "</b><br>openssl s_client -showcerts -connect smtp.gmail.com:465 &lt;/dev/null | sed -n -e '/-.BEGIN/,/-.END/ p' &gt; gmail.crt<br><br>"
                                                   "<b>" + tr("Notes")+ "</b><br>"+ tr("Generating certificate it is important to define a Common name (e.g. server FQDN or YOUR name)"), "Ok");
}

void QDOpenComPort::on_QRBBluetoothLowEnergy_toggled(bool ) {
    ui->tabWidget->setCurrentIndex(2);
}

void QDOpenComPort::on_QRBRS232_toggled(bool ) {
    ui->tabWidget->setCurrentIndex(0);
    //ui->QCBComPort->count()> 0 ? ui->QPBOk->setEnabled(true) : ui->QPBOk->setEnabled(false);
}

void QDOpenComPort::on_QRBTCPClient_toggled(bool ) {
    ui->tabWidget->setCurrentIndex(1);
    ui->QLEServer->setEnabled(true);
    if (ui->QLEServer->text().length()> 0) ui->QPBOk->setEnabled(true);
    //else ui->QPBOk->setEnabled(false);
    ui->QGBFirewallRule->hide();
}

void QDOpenComPort::on_QRBTCPClientSsl_toggled(bool checked) {
    if (ui->QRBTCPServerSsl->isChecked() || ui->QRBTCPClientSsl->isChecked()) ui->QWKeyAndCertificate->show();
    else ui->QWKeyAndCertificate->hide();
    on_QRBTCPClient_toggled(checked);
}

void QDOpenComPort::on_QRBTCPServer_toggled(bool ) {
    ui->tabWidget->setCurrentIndex(1);
    ui->QLEServer->setEnabled(false);
    ui->QPBOk->setEnabled(true);
    ui->QGBFirewallRule->show();
}

void QDOpenComPort::on_QRBTCPServerSsl_toggled(bool ) {
    if (ui->QRBTCPServerSsl->isChecked() || ui->QRBTCPClientSsl->isChecked()) ui->QWKeyAndCertificate->show();
    else ui->QWKeyAndCertificate->hide();
    ui->tabWidget->setCurrentIndex(1);
    ui->QLEServer->setEnabled(false);
    ui->QPBOk->setEnabled(true);
    ui->QGBFirewallRule->show();
}

void QDOpenComPort::on_QSBSocket_valueChanged(int arg1) {
    ui->QTEFirewallRule->clear();
    ui->QTEFirewallRule->append(tr("To add"));
    ui->QTEFirewallRule->append("netsh advfirewall firewall add rule name=\"QtComPort\" dir=in action=allow protocol=TCP localport="+ QString::number(arg1));
    ui->QTEFirewallRule->append("");
    ui->QTEFirewallRule->append(tr("To remove"));
    ui->QTEFirewallRule->append("netsh advfirewall firewall delete rule name=\"QtComPort\" protocol=TCP localport="+ QString::number(arg1));
    ui->QPBOk->setEnabled(true);
}

void QDOpenComPort::on_QRBSslKeyCertificateEmbedded_toggled(bool ) {
    ui->QPBOk->setEnabled(true);
}

void QDOpenComPort::on_QTBSslCertificate_clicked() {
    QFileDialog FileDialog(this);
    FileDialog.setViewMode(QFileDialog::Detail);
    FileDialog.setFileMode(QFileDialog::ExistingFile);
    if (FileDialog.exec()== QDialog::Accepted) {
        ui->QLESslCertificate->setText(FileDialog.selectedFiles().at(0));
        ui->QPBOk->setEnabled(true);
    }
}

void QDOpenComPort::on_QTBSslKeyPrivate_clicked() {
    QFileDialog FileDialog(this);
    FileDialog.setViewMode(QFileDialog::Detail);
    FileDialog.setFileMode(QFileDialog::ExistingFile);
    if (FileDialog.exec()== QDialog::Accepted) {
        ui->QLESslKeyPrivate->setText(FileDialog.selectedFiles().at(0));
        ui->QPBOk->setEnabled(true);
    }
}

/*void QDOpenComPort::on_pushButton_clicked() {
    if (pQLowEnergyService && LowEnergyCharacteristicWrite.isValid()) {
        pQLowEnergyService->writeCharacteristic(LowEnergyCharacteristicWrite, "abcdf", LowEnergyServiceWriteMode);
    }
}*/

