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

#include <QMainWindow>
#include "QDesktopWidget"
#include "QFileDialog"
#include "QScrollBar"
#include "qdopencomport.h"
#include "QMessageBox"
#include "qdterminal.h"
#include "qdbridge.h"
#include "qdopencomport.h"
#include "QSerialPortInfo"
#include "QDesktopServices"

namespace Ui {
    class QFMainForm;
}

class QFMainForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit QFMainForm(QWidget *parent = 0);
    ~QFMainForm();

private:
    Ui::QFMainForm *ui;
    QVector <QDTerminal*>QVTerminals;
    void ReadConfigurationFile(QString Path);

private slots:
    void on_QAAuthor_triggered();
    void on_QAQtVersion_triggered();
    void on_QAVersion_triggered();
    void on_QPBBridge_clicked();
    void on_QPBLoadProfile_clicked();
    void on_QPBNewProfile_clicked();
    void on_QPBQuit_clicked();

};

#endif // QFMAINFORM_H
