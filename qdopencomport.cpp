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
}

QDOpenComPort::~QDOpenComPort() {
    delete ui;
}

void QDOpenComPort::on_QPBOk_clicked() {
    this->accept();
}

void QDOpenComPort::on_QPBCancel_clicked() {
    this->reject();
}

void QDOpenComPort::on_QRBRS232_toggled(bool ) {
    ui->QCBComPort->setEnabled(true);
    ui->QCBBaudRate->setEnabled(true);
    ui->QCBParity->setEnabled(true);
    ui->QCBDataBits->setEnabled(true);
    ui->QCBStopBits->setEnabled(true);
    ui->QCBFlowControl->setEnabled(true);
    ui->QLEServer->setEnabled(false);
    ui->QSBSocket->setEnabled(false);
    ui->QPBOk->setEnabled(true);
}

void QDOpenComPort::on_QRBTCP_toggled(bool ) {
    ui->QCBComPort->setEnabled(false);
    ui->QCBBaudRate->setEnabled(false);
    ui->QCBParity->setEnabled(false);
    ui->QCBDataBits->setEnabled(false);
    ui->QCBStopBits->setEnabled(false);
    ui->QCBFlowControl->setEnabled(false);
    ui->QLEServer->setEnabled(true);
    ui->QSBSocket->setEnabled(true);
    if (ui->QLEServer->text().length()> 0) ui->QPBOk->setEnabled(true);
    else ui->QPBOk->setEnabled(false);
}

void QDOpenComPort::on_QRBTCPSsl_toggled(bool checked) {
    on_QRBTCP_toggled(checked);
}

void QDOpenComPort::on_QLEServer_returnPressed() {
    ui->QPBOk->click();
}

void QDOpenComPort::on_QLEServer_textChanged(QString ) {
    if (ui->QLEServer->text().length()> 0) ui->QPBOk->setEnabled(true);
    else ui->QPBOk->setEnabled(false);
}
