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

#ifndef QDBRIDGE_H
#define QDBRIDGE_H

#include <QDialog>
#include "ui_qdbridge.h"
#include "QRadioButton"

namespace Ui {
    class QDBridge;
}

class QDBridge : public QDialog
{
    Q_OBJECT

public:
    explicit QDBridge(QWidget *parent = nullptr);
    ~QDBridge();
    Ui::QDBridge *ui;
    struct TerminalRadioButton {
        int count;
        QRadioButton *RadioButton;
    };
    QVector<TerminalRadioButton> QVTerminalRadioButtons1, QVTerminalRadioButtons2;

private:

private slots:
    void OnTerminalsToggled(bool checked);

};

#endif // QDBRIDGE_H
