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

#include "qdfontdialog.h"
#include "ui_qdfontdialog.h"
#include "QDebug"

QDFontDialog::QDFontDialog(QWidget *parent) : QDialog(parent), ui(new Ui::QDFontDialog) {
    ui->setupUi(this);
    QFontDatabase fdb;
    ui->QLWFonts->addItems(fdb.families());
    for (int count= 0; count< fdb.families().count(); count++) {
        QFont font= ui->QLWFonts->item(count)->font();
        font.setFamily(fdb.families().at(count));
        ui->QLWFonts->item(count)->setFont(font);
    }
    ui->QLWStyles->addItem("Normal");
    ui->QLWStyles->addItem("Italic");
    ui->QLWStyles->addItem("Oblique");
    ui->QLWStyles->addItem("Bold");
    ui->QLWSizes->addItem("6");
    ui->QLWSizes->addItem("7");
    ui->QLWSizes->addItem("8");
    ui->QLWSizes->addItem("9");
    ui->QLWSizes->addItem("10");
    ui->QLWSizes->addItem("11");
    ui->QLWSizes->addItem("12");
    ui->QLWSizes->addItem("13");
    ui->QLWSizes->addItem("14");
    ui->QLWSizes->addItem("16");
    ui->QLWSizes->addItem("17");
    ui->QLWSizes->addItem("18");
    ui->QLWSizes->addItem("20");
    ui->QLWSizes->addItem("22");
    ui->QLWSizes->addItem("24");
    ui->QLWSizes->addItem("26");
    ui->QLWSizes->addItem("28");
    ui->QLWSizes->addItem("32");
    ui->QLWSizes->addItem("36");
    ui->QLWSizes->addItem("40");
    ui->QLWSizes->addItem("48");
    ui->QLWSizes->addItem("56");
    ui->QLWSizes->addItem("64");
    ui->QLWSizes->addItem("72");
}

QDFontDialog::~QDFontDialog() {
    delete ui;
}

void QDFontDialog::on_QLWFonts_clicked(QModelIndex ) {
    QFont font= ui->QLWFonts->item(ui->QLWFonts->currentIndex().row())->font();
    switch(ui->QLWStyles->currentIndex().row()) {
    case 0: font.setStyle(QFont::StyleNormal); break;
    case 1: font.setStyle(QFont::StyleItalic); break;
    case 2: font.setStyle(QFont::StyleOblique); break;
    case 3: font.setBold(true); break;
    }
    if (ui->QLWSizes->currentIndex().row()> -1) font.setPointSize((ui->QLWSizes->item(ui->QLWSizes->currentIndex().row())->text().toInt()));
    ui->QLEPreview->setFont(font);
}

void QDFontDialog::on_QLWStyles_clicked(QModelIndex index) {
    on_QLWFonts_clicked(index);
}

void QDFontDialog::on_QLWSizes_clicked(QModelIndex index) {
    on_QLWFonts_clicked(index);
}

void QDFontDialog::SetCurrentFont(QFont font) {
    for (int count= 0; count< ui->QLWFonts->count(); count++) {
        if (ui->QLWFonts->item(count)->text().compare(font.toString().left(font.toString().indexOf(",")))== 0) {
            ui->QLWFonts->setCurrentRow(count);
            break;
        }
    }
    switch(font.style()) {
    case QFont::StyleNormal: ui->QLWStyles->setCurrentRow(0); break;
    case QFont::StyleItalic: ui->QLWStyles->setCurrentRow(1); break;
    case QFont::StyleOblique: ui->QLWStyles->setCurrentRow(2); break;
    }
    for (int count= 0; count< ui->QLWSizes->count(); count++) {
        if (ui->QLWSizes->item(count)->text().compare(QString::number(font.pointSize()))== 0) {
            ui->QLWSizes->setCurrentRow(count); break;
            break;
        }
    }
    ui->QLEPreview->setFont(font);
}

QFont QDFontDialog::GetCurrentFont() {
    return ui->QLEPreview->font();
}
