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

//> The "New" button could be named "open port". TCP and RS232 open dialogs
//> could be split into two "Open Serial" and "Open TCP" because the settings
//> have nothing in common.

//> * Allow to set a profile as startup default.
//> * Provide a hex/printable hybrid mode like hexeditors or gtkterm do.
//> * Provide a normal Makefile or a readme on how to generate one.


#include "qfmainform.h"
#include "ui_qfmainform.h"

QFMainForm::QFMainForm(QWidget *parent) : QMainWindow(parent), ui(new Ui::QFMainForm) {
    ui->setupUi(this);
    QRect scr= QApplication::screens().at(0)->geometry();
    move(scr.center()- rect().center());
    if (QCoreApplication::arguments().count()> 1) ReadConfigurationFile(QCoreApplication::arguments().at(1));
    NetworkAccessManager= new QNetworkAccessManager(this);
    connect(NetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    NetworkAccessManager->get(QNetworkRequest(QUrl("http://www.denisgottardello.it/QtComPort/QtComPortVersion.txt")));
}

QFMainForm::~QFMainForm() {
    if (NetworkAccessManager) delete NetworkAccessManager;
    delete ui;
}

void QFMainForm::finished(QNetworkReply *reply) {
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()== 200) {
        if (QString(reply->readAll().trimmed()).compare(VERSION)!= 0) {
            if (QMessageBox::information(this, tr("Info"), tr("New version available. Do you want to open the QtComPort download page?"), QMessageBox::Yes | QMessageBox::No)== QMessageBox::Yes) {
                QDesktopServices::openUrl(QUrl("http://www.denisgottardello.it/QtComPort"));
            }
        }
    }
}

void QFMainForm::on_QAAuthor_triggered() {
    QMessageBox::information(this, tr("Info"), "Denis Gottardello\nwww.denisgottardello.it\ninfo@denisgottardello.it", "Ok");
}

void QFMainForm::on_QAQtVersion_triggered() {
    QMessageBox::information(this, tr("Info"), tr("Qt version: ")+ QT_VERSION_STR, "Ok");
}

void QFMainForm::on_QAVersion_triggered() {
    QMessageBox::information(this, tr("Info"), tr("QtComPort Version ")+ VERSION, "Ok");
}

void QFMainForm::on_QPBBridge_clicked() {
    QDBridge *pQDBridge= new QDBridge(this); {
        for (int count= 0; count< QVTerminals.count(); count++) {
            QDBridge::TerminalRadioButton RadioButton1;
            QListWidgetItem *NewItem1= new QListWidgetItem(pQDBridge->ui->QLWConnections1);
            RadioButton1.RadioButton= new QRadioButton();
            RadioButton1.count= count;
            RadioButton1.RadioButton->setText(QVTerminals[count]->ui->QLConnection->text());
            NewItem1->setSizeHint(ui->QPBBridge->size());
            connect(RadioButton1.RadioButton, SIGNAL(toggled(bool)), pQDBridge, SLOT(OnTerminalsToggled(bool)));
            pQDBridge->ui->QLWConnections1->setItemWidget(NewItem1, RadioButton1.RadioButton);
            pQDBridge->QVTerminalRadioButtons1.append(RadioButton1);

            QDBridge::TerminalRadioButton RadioButton2;
            QListWidgetItem *NewItem2= new QListWidgetItem(pQDBridge->ui->QLWConnections2);
            RadioButton2.RadioButton= new QRadioButton();
            RadioButton2.count= count;
            RadioButton2.RadioButton->setText(QVTerminals[count]->ui->QLConnection->text());
            NewItem2->setSizeHint(ui->QPBBridge->size());
            connect(RadioButton2.RadioButton, SIGNAL(toggled(bool)), pQDBridge, SLOT(OnTerminalsToggled(bool)));
            pQDBridge->ui->QLWConnections2->setItemWidget(NewItem2, RadioButton2.RadioButton);
            pQDBridge->QVTerminalRadioButtons2.append(RadioButton2);
        }
        if (pQDBridge->exec()== QDialog::Accepted) {
            for (int count= 0; count< pQDBridge->QVTerminalRadioButtons1.count(); count++) {
                if (pQDBridge->QVTerminalRadioButtons1[count].RadioButton->isChecked()) {
                    for (int count_2= 0; count_2< pQDBridge->QVTerminalRadioButtons2.count(); count_2++) {
                        if (pQDBridge->QVTerminalRadioButtons2[count_2].RadioButton->isChecked()) {
                            QVTerminals[count]->pQDTerminal= QVTerminals[count_2];
                            QVTerminals[count_2]->pQDTerminal= QVTerminals[count];
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }{
        delete pQDBridge;
    }
}

void QFMainForm::on_QPBLoadProfile_clicked() {
    QFileDialog FileDialog(this);
    FileDialog.setViewMode(QFileDialog::Detail);
    FileDialog.setFilter(QDir::Files);
    FileDialog.setNameFilter("Qcp (*.qcp)");
    if (FileDialog.exec()== QDialog::Accepted) {
        ReadConfigurationFile(FileDialog.selectedFiles().at(0));
        if (QVTerminals.count()> 1) ui->QPBBridge->setEnabled(true);
        ui->QTBTerminal->setCurrentIndex(ui->QTBTerminal->count()- 1);
    }
}

void QFMainForm::on_QPBNewProfile_clicked() {
    QDOpenComPort OpenComPort(this);
    foreach (const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts()) OpenComPort.ui->QCBComPort->addItem(SerialPortInfo.portName());
    OpenComPort.ui->QCBComPort->count()> 0 ? OpenComPort.ui->QPBOk->setEnabled(true) : OpenComPort.ui->QPBOk->setEnabled(false);
    if (OpenComPort.exec()== QDialog::Accepted) {
        QDTerminal *Terminal= new QDTerminal(this, "");
        Terminal->TabNumber= ui->QTBTerminal->addTab(Terminal, tr("New"));
        Terminal->pQTBTerminal= ui->QTBTerminal;
        switch(OpenComPort.ui->QCBParity->currentIndex()) {
            case 0: Terminal->Parity= 'N'; break;
            case 1: Terminal->Parity= 'E'; break;
            case 2: Terminal->Parity= 'O'; break;
            case 3: Terminal->Parity= 'M'; break;
            case 4: Terminal->Parity= 'S'; break;
        }
        switch(OpenComPort.ui->QCBDataBits->currentIndex()) {
            case 0: Terminal->ByteSize= 5; break;
            case 1: Terminal->ByteSize= 6; break;
            case 2: Terminal->ByteSize= 7; break;
            case 3: Terminal->ByteSize= 8; break;
        }
        switch(OpenComPort.ui->QCBStopBits->currentIndex()) {
            case 0: Terminal->StopBits= 0; break;
            case 1: Terminal->StopBits= 1; break;
            case 2: Terminal->StopBits= 2; break;
        }
        Terminal->ComPort= OpenComPort.ui->QCBComPort->currentText();
        Terminal->BaudRate= OpenComPort.ui->QCBBaudRate->currentText().toInt();
        Terminal->FlowControl= OpenComPort.ui->QCBFlowControl->currentIndex();
        Terminal->MaxClients= OpenComPort.ui->QSBTCPServerMaxClients->value();
        Terminal->ui->QPTELog->setStyleSheet("background-color: black; color: white");
        Terminal->SendBreak= OpenComPort.ui->QCBSendBreak->isChecked();
        Terminal->Server= OpenComPort.ui->QLEServer->text();
        Terminal->Socket= static_cast<quint16>(OpenComPort.ui->QSBSocket->value());
        if (OpenComPort.ui->QRBRS232->isChecked()) Terminal->Mode= MODE_RS232;
        else if (OpenComPort.ui->QRBTCPClient->isChecked()) Terminal->Mode= MODE_TCP_CLIENT;
        else if (OpenComPort.ui->QRBTCPClientSsl->isChecked()) Terminal->Mode= MODE_TCP_CLIENT_SSL;
        else if (OpenComPort.ui->QRBTCPServer->isChecked()) Terminal->Mode= MODE_TCP_SERVER;
        else if (OpenComPort.ui->QRBTCPServerSsl->isChecked()) Terminal->Mode= MODE_TCP_SERVER_SSL;
        Terminal->ui->QPBOpen->setEnabled(true);
        Terminal->ui->QPBOpen->click();
        Terminal->pQVTerminals= &QVTerminals;
        ui->QTBTerminal->setCurrentIndex(ui->QTBTerminal->count()- 1);
        QVTerminals.append(Terminal);
        if (QVTerminals.count()> 1) ui->QPBBridge->setEnabled(true);
    }
}

void QFMainForm::ReadConfigurationFile(QString ConnectionPath) {
    QDTerminal *pQDTerminal= new QDTerminal(this, ConnectionPath);
    pQDTerminal->TabNumber= ui->QTBTerminal->addTab(pQDTerminal, ConnectionPath);
    pQDTerminal->pQTBTerminal= ui->QTBTerminal;
    pQDTerminal->pQVTerminals= &QVTerminals;
    pQDTerminal->ReadConfigurationFile();
    QVTerminals.append(pQDTerminal);
}
