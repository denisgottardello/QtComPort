#include "qdcommandssequence.h"

QDCommandsSequence::QDCommandsSequence(QWidget *parent) : QDialog(parent), ui(new Ui::QDCommandsSequence) {
    ui->setupUi(this);
    pQDTerminal= static_cast<QDTerminal*>(parent);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
    QStringList QSLHeader;
    QSLHeader.append(tr("Enabled"));
    QSLHeader.append(tr("Command as hex"));
    QSLHeader.append(tr("Command"));
    QSLHeader.append(tr("Delay (ms)"));
    ui->QTWCommands->setColumnCount(QSLHeader.count());
    ui->QTWCommands->setHorizontalHeaderLabels(QSLHeader);
    for (int count= 0; count< 20; count++) {
        ui->QTWCommands->insertRow(ui->QTWCommands->rowCount());
        QCheckBox *QCBCommand= new QCheckBox(ui->QTWCommands);
        QCheckBox *QCBCommandAsHex= new QCheckBox(ui->QTWCommands);
        QLineEdit *QLECommand= new QLineEdit(ui->QTWCommands);
        QSpinBox *QSBCommand= new QSpinBox(ui->QTWCommands);
        QCBCommand->setStyleSheet("margin: 1px; margin-left: 50%; margin-right: 50%");
        QCBCommandAsHex->setStyleSheet("margin: 1px; margin-left: 50%; margin-right: 50%");
        QLECommand->setStyleSheet("margin: 1px");
        QLECommand->setMinimumWidth(256);
        QSBCommand->setMaximum(9999);
        QSBCommand->setValue(500);
        ui->QTWCommands->setCellWidget(ui->QTWCommands->rowCount()- 1, 0, QCBCommand);
        ui->QTWCommands->setCellWidget(ui->QTWCommands->rowCount()- 1, 1, QCBCommandAsHex);
        ui->QTWCommands->setCellWidget(ui->QTWCommands->rowCount()- 1, 2, QLECommand);
        ui->QTWCommands->setCellWidget(ui->QTWCommands->rowCount()- 1, 3, QSBCommand);
    }
    ui->QTWCommands->resizeColumnsToContents();
    ui->QTWCommands->resizeRowsToContents();
    connect(&Timer, SIGNAL(timeout()), this, SLOT(Timeout()));
}

QDCommandsSequence::~QDCommandsSequence() {
    delete ui;
}

void QDCommandsSequence::closeEvent(QCloseEvent *) {
    emit OnClose();
}

void QDCommandsSequence::on_QPBClose_clicked() {
    this->close();
}

void QDCommandsSequence::on_QPBExport_clicked() {
    QString Path= QFileDialog::getSaveFileName(this, tr("Save As"), QDir::currentPath(), "(*.xml)");
    if (Path.length()> 0) {
        if (!Path.endsWith(".xml", Qt::CaseInsensitive)!= 0) Path+= ".xml";
        QDomDocument XmlDocOut;
        XmlDocOut.appendChild(XmlDocOut.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\""));
        QDomElement rootDomOut= XmlDocOut.createElement("CommandsSequence");
        for (int count= 0; count< ui->QTWCommands->rowCount(); count++) {
            if (((QLineEdit*)(ui->QTWCommands->cellWidget(count, 2)))->text().length()) {
                QDomElement nodeFirstLevelDomOut= XmlDocOut.createElement("CommandSequence"+ QString::number(count));
                nodeFirstLevelDomOut.setAttribute("Command", ((QLineEdit*)(ui->QTWCommands->cellWidget(count, 2)))->text());
                nodeFirstLevelDomOut.setAttribute("CommandType", ((QCheckBox*)(ui->QTWCommands->cellWidget(count, 1)))->isChecked() ? 1 : 0);
                nodeFirstLevelDomOut.setAttribute("Delay", ((QSpinBox*)(ui->QTWCommands->cellWidget(count, 3)))->value());
                nodeFirstLevelDomOut.setAttribute("Enabled", ((QCheckBox*)(ui->QTWCommands->cellWidget(count, 0)))->isChecked());
                rootDomOut.appendChild(nodeFirstLevelDomOut);
            }
        }
        XmlDocOut.appendChild(rootDomOut);
        QFile FileOut(Path);
        FileOut.open(QIODevice::WriteOnly);
        if (FileOut.isOpen()) {
            FileOut.write(XmlDocOut.toByteArray());
            FileOut.close();
        }
    }
}

void QDCommandsSequence::on_QPBImport_clicked() {
    QFileDialog FileDialog;
    FileDialog.setViewMode(QFileDialog::Detail);
    FileDialog.setNameFilter(tr("(*.xml)"));
    if (FileDialog.exec()== QDialog::Accepted) {
        QFile FileIn(FileDialog.selectedFiles().at(0));
        if (FileIn.open(QIODevice::ReadOnly)) {
            QDomDocument XmlDocIn;
            XmlDocIn.setContent(FileIn.readAll());
            QDomNodeList DomNodeList= XmlDocIn.elementsByTagName("CommandsSequence").item(0).childNodes();
            int count= 0;
            for (; count< DomNodeList.count(); count++) {
                if (count< ui->QTWCommands->rowCount()) {
                    ((QLineEdit*)(ui->QTWCommands->cellWidget(count, 2)))->setText(DomNodeList.item(count).toElement().attribute("Command", ""));
                    ((QCheckBox*)(ui->QTWCommands->cellWidget(count, 1)))->setChecked(DomNodeList.item(count).toElement().attribute("CommandType", 0).toInt()== 1);
                    ((QSpinBox*)(ui->QTWCommands->cellWidget(count, 3)))->setValue(DomNodeList.item(count).toElement().attribute("Delay", "250").toInt());
                    ((QCheckBox*)(ui->QTWCommands->cellWidget(count, 0)))->setChecked(QVariant(DomNodeList.item(count).toElement().attribute("Enabled", "")).toBool());
                } else break;
            }
            for (; count< ui->QTWCommands->rowCount(); count++) {
                ((QLineEdit*)(ui->QTWCommands->cellWidget(count, 2)))->setText("");
                ((QCheckBox*)(ui->QTWCommands->cellWidget(count, 1)))->setChecked(false);
                ((QSpinBox*)(ui->QTWCommands->cellWidget(count, 3)))->setValue(500);
                ((QCheckBox*)(ui->QTWCommands->cellWidget(count, 0)))->setChecked(false);
            }
            FileIn.close();
        }
    }
}

void QDCommandsSequence::on_QPBRun_clicked() {
    if (ui->QPBRun->isChecked()) {
        Index= 0;
        ui->QLEIndex->setText(QString::number(Index));
        Timeout();
    }
}

void QDCommandsSequence::Timeout() {
    Timer.stop();
    if (ui->QPBRun->isChecked()) {
        while (Index< ui->QTWCommands->rowCount() && !((QCheckBox*)(ui->QTWCommands->cellWidget(Index, 0)))->isChecked()) {
            Index++;
            ui->QLEIndex->setText(QString::number(Index));
        }
        if (Index< ui->QTWCommands->rowCount()) {
            if (((QCheckBox*)(ui->QTWCommands->cellWidget(Index, 1)))->isChecked()) {
                QByteArray QBABuffferOut;
                QString Temp= ((QLineEdit*)(ui->QTWCommands->cellWidget(Index, 2)))->text();
                while (Temp.length()> 1) {
                    QBABuffferOut.append(Temp.midRef(0, 2).toString().toUInt(nullptr, 16));
                    Temp.remove(0, 2);
                }
                pQDTerminal->SendByteArray(QBABuffferOut);
            } else pQDTerminal->SendByteArray(((QLineEdit*)(ui->QTWCommands->cellWidget(Index, 2)))->text().toLatin1()+ "\n");
            Timer.start(((QSpinBox*)(ui->QTWCommands->cellWidget(Index, 3)))->value());
            Index++;
            ui->QLEIndex->setText(QString::number(Index));
        } else {
            ui->QPBRun->setChecked(false);
            if (ui->QCBLoop->isChecked()) {
                for (int count= 0; count< ui->QTWCommands->rowCount(); count++) {
                    if (((QCheckBox*)(ui->QTWCommands->cellWidget(count, 0)))->isChecked()) {
                        ui->QPBRun->setChecked(true);
                        Index= 0;
                        ui->QLEIndex->setText(QString::number(Index));
                        Timeout();
                        break;
                    }
                }
            }
        }
    }
}
