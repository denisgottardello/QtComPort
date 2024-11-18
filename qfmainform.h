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

#ifndef QFMAINFORM_H
#define QFMAINFORM_H

#include "qdbridge.h"
#include "QDesktopServices"
#include "QDesktopWidget"
#include "qdterminal.h"
#include "QFileDialog"
#include "QMessageBox"
#include "QNetworkAccessManager"
#include "QNetworkReply"
#include "QScreen"
#include "QScrollBar"
#include "QSerialPortInfo"
#include <QMainWindow>

#define VERSION QString("0.0.29.0")

namespace Ui {
    class QFMainForm;
}

class QFMainForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit QFMainForm(QWidget *parent = nullptr);
    ~QFMainForm();

private:
    Ui::QFMainForm *ui;
    QNetworkAccessManager *NetworkAccessManager= nullptr;
    QVector<QDTerminal*> QVTerminals;
    void ReadConfigurationFile(QString Path);

private slots:
    void finished(QNetworkReply *reply);
    void on_QAAuthor_triggered();
    void on_QAQtVersion_triggered();
    void on_QAVersion_triggered();
    void on_QPBBridge_clicked();
    void on_QPBLoadProfile_clicked();
    void on_QPBNewProfile_clicked();

};

#endif // QFMAINFORM_H
