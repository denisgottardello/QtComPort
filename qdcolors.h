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

#ifndef QDCOLORS_H
#define QDCOLORS_H

#include <QDialog>
#include "QColorDialog"
#include "ui_qdcolors.h"

namespace Ui {
    class QDColors;
}

class QDColors : public QDialog
{
    Q_OBJECT

public:
    explicit QDColors(QWidget *parent = nullptr);
    ~QDColors();
    Ui::QDColors *ui;

private:

private slots:
    void on_QPBBackgroundColor_clicked();
    void on_QPBFontColor_clicked();
};

#endif // QDCOLORS_H
