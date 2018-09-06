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
    QRect scr= QApplication::desktop()->screenGeometry();
    move(scr.center()- rect().center());
    if (QCoreApplication::arguments().count()> 1) ReadConfigurationFile(QCoreApplication::arguments().at(1));
}

QFMainForm::~QFMainForm() {
    delete ui;
}

void QFMainForm::on_QAAuthor_triggered() {
    QMessageBox::information(this, "Info", "Denis Gottardello\nwww.denisgottardello.it\ninfo@denisgottardello.it", "Ok");
}

void QFMainForm::on_QAQtVersion_triggered() {
    QMessageBox::information(this, "Info", tr("Qt version: ")+ QT_VERSION_STR, "Ok");
}

void QFMainForm::on_QAVersion_triggered() {
    QMessageBox::information(this, "Info", tr("QtComPort Version ")+ ("0.0.9.0"), "Ok");
}

void QFMainForm::on_QPBBridge_clicked() {
    QDBridge *Bridge= new QDBridge(this); {
        for (int count= 0; count< QVTerminals.count(); count++) {
            QDBridge::TerminalRadioButton RadioButton1;
            QListWidgetItem *NewItem1= new QListWidgetItem(Bridge->ui->QLWConnections1);
            RadioButton1.RadioButton= new QRadioButton();
            RadioButton1.count= count;
            RadioButton1.RadioButton->setText(QVTerminals[count]->ui->QLConnection->text());
            NewItem1->setSizeHint(ui->QPBBridge->size());
            connect(RadioButton1.RadioButton, SIGNAL(toggled(bool)), Bridge, SLOT(OnTerminalsToggled(bool)));
            Bridge->ui->QLWConnections1->setItemWidget(NewItem1, RadioButton1.RadioButton);
            Bridge->QVTerminalRadioButtons1.append(RadioButton1);

            QDBridge::TerminalRadioButton RadioButton2;
            QListWidgetItem *NewItem2= new QListWidgetItem(Bridge->ui->QLWConnections2);
            RadioButton2.RadioButton= new QRadioButton();
            RadioButton2.count= count;
            RadioButton2.RadioButton->setText(QVTerminals[count]->ui->QLConnection->text());
            NewItem2->setSizeHint(ui->QPBBridge->size());
            connect(RadioButton2.RadioButton, SIGNAL(toggled(bool)), Bridge, SLOT(OnTerminalsToggled(bool)));
            Bridge->ui->QLWConnections2->setItemWidget(NewItem2, RadioButton2.RadioButton);
            Bridge->QVTerminalRadioButtons2.append(RadioButton2);
        }
        if (Bridge->exec()== QDialog::Accepted) {
            for (int count= 0; count< Bridge->QVTerminalRadioButtons1.count(); count++) {
                if (Bridge->QVTerminalRadioButtons1[count].RadioButton->isChecked()) {
                    for (int count_2= 0; count_2< Bridge->QVTerminalRadioButtons2.count(); count_2++) {
                        if (Bridge->QVTerminalRadioButtons2[count_2].RadioButton->isChecked()) {
                            QVTerminals[count]->QdTerminal= QVTerminals[count_2];
                            QVTerminals[count_2]->QdTerminal= QVTerminals[count];
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }{
        delete Bridge;
    }
}

void QFMainForm::on_QPBLoadProfile_clicked() {
    QString ConnectionPath= QFileDialog::getOpenFileName(this, "Open connection file", QDir::currentPath());
    if (ConnectionPath!= NULL) {
        ReadConfigurationFile(ConnectionPath);
        if (QVTerminals.count()> 1) ui->QPBBridge->setEnabled(true);
        ui->QTBTerminal->setCurrentIndex(ui->QTBTerminal->count()- 1);
    }
}

void QFMainForm::on_QPBNewProfile_clicked() {
    QDOpenComPort QdOpenComPort;
    foreach (const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts()) QdOpenComPort.ui->QCBComPort->addItem(SerialPortInfo.portName());
    if (QdOpenComPort.exec()== QDialog::Accepted) {
        QDTerminal *Terminal= new QDTerminal(this, "");
        Terminal->TabNumber= ui->QTBTerminal->addTab(Terminal, "New");
        Terminal->QTBTerminal= ui->QTBTerminal;
        switch(QdOpenComPort.ui->QCBParity->currentIndex()) {
        case 0: Terminal->Parity= 'N'; break;
        case 1: Terminal->Parity= 'E'; break;
        case 2: Terminal->Parity= 'O'; break;
        case 3: Terminal->Parity= 'M'; break;
        case 4: Terminal->Parity= 'S'; break;
        }
        switch(QdOpenComPort.ui->QCBDataBits->currentIndex()) {
        case 0: Terminal->ByteSize= 5; break;
        case 1: Terminal->ByteSize= 6; break;
        case 2: Terminal->ByteSize= 7; break;
        case 3: Terminal->ByteSize= 8; break;
        }
        switch(QdOpenComPort.ui->QCBStopBits->currentIndex()) {
        case 0: Terminal->StopBits= 0; break;
        case 1: Terminal->StopBits= 1; break;
        case 2: Terminal->StopBits= 2; break;
        }
        Terminal->ComPort= QdOpenComPort.ui->QCBComPort->currentText();
        Terminal->BaudRate= QdOpenComPort.ui->QCBBaudRate->currentText().toInt();
        Terminal->FlowControl= QdOpenComPort.ui->QCBFlowControl->currentIndex();
        Terminal->ui->QPTELog->setStyleSheet("background-color: black; color: white");
        Terminal->SendBreak= QdOpenComPort.ui->QCBSendBreak->isChecked();
        Terminal->Server= QdOpenComPort.ui->QLEServer->text();
        Terminal->Socket= QdOpenComPort.ui->QSBSocket->value();
        if (QdOpenComPort.ui->QRBRS232->isChecked()) Terminal->Mode= MODERS232;
        else Terminal->Mode= MODETCPIP;
        Terminal->ui->QPBOpen->setEnabled(true);
        Terminal->ui->QPBOpen->click();
        Terminal->QVTerminals= &QVTerminals;
        ui->QTBTerminal->setCurrentIndex(ui->QTBTerminal->count()- 1);
        QVTerminals.append(Terminal);
        if (QVTerminals.count()> 1) ui->QPBBridge->setEnabled(true);
    }
}

void QFMainForm::on_QPBQuit_clicked() {
    QCoreApplication::quit();
}

void QFMainForm::ReadConfigurationFile(QString ConnectionPath) {
    QDTerminal *Terminal= new QDTerminal(this, ConnectionPath);
    Terminal->TabNumber= ui->QTBTerminal->addTab(Terminal, ConnectionPath);
    Terminal->QTBTerminal= ui->QTBTerminal;
    Terminal->QVTerminals= &QVTerminals;
    Terminal->ReadConfigurationFile();
    QVTerminals.append(Terminal);
}
