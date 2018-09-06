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

#include "qdbridge.h"
#include "ui_qdbridge.h"

QDBridge::QDBridge(QWidget *parent) : QDialog(parent), ui(new Ui::QDBridge) {
    ui->setupUi(this);
}

QDBridge::~QDBridge() {
    delete ui;
}

void QDBridge::OnTerminalsToggled(bool ) {
    ui->QPBOk->setEnabled(false);
    bool IsChecked1= false, IsChecked2= false;
    for (int count= 0; count< QVTerminalRadioButtons1.count(); count++) {
        QVTerminalRadioButtons1[count].RadioButton->setEnabled(true);
        QVTerminalRadioButtons2[count].RadioButton->setEnabled(true);
        if (QVTerminalRadioButtons1[count].RadioButton->isChecked()) {
            QVTerminalRadioButtons2[count].RadioButton->setEnabled(false);
            IsChecked1= true;
        }
        if (QVTerminalRadioButtons2[count].RadioButton->isChecked()) {
            QVTerminalRadioButtons1[count].RadioButton->setEnabled(false);
            IsChecked2= true;
        }
    }
    if (IsChecked1 && IsChecked2) ui->QPBOk->setEnabled(true);
}
