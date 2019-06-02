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

#ifndef QDOPENCOMPORT_H
#define QDOPENCOMPORT_H

#include <QDialog>
#include "ui_qdopencomport.h"

namespace Ui {
    class QDOpenComPort;
}

class QDOpenComPort : public QDialog
{
    Q_OBJECT

public:
    explicit QDOpenComPort(QWidget *parent = 0);
    ~QDOpenComPort();
    Ui::QDOpenComPort *ui;

private:

private slots:
    void on_QLEServer_returnPressed();
    void on_QLEServer_textChanged(QString );
    void on_QRBTCPIP_clicked();
    void on_QRBRS232_clicked();
    void on_QPBCancel_clicked();
    void on_QPBOk_clicked();

};

#endif // QDOPENCOMPORT_H