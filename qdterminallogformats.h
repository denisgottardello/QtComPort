#ifndef QDTERMINALLOGFORMATS_H
#define QDTERMINALLOGFORMATS_H

#include <QDialog>
#include "ui_qdterminallogformats.h"

namespace Ui {
class QDTerminalLogFormats;
}

class QDTerminalLogFormats : public QDialog
{
    Q_OBJECT

public:
    explicit QDTerminalLogFormats(QWidget *parent = nullptr);
    ~QDTerminalLogFormats();
    Ui::QDTerminalLogFormats *ui;

private slots:
    void on_QPBCancel_clicked();
    void on_QPBOk_clicked();

private:

};

#endif // QDTERMINALLOGFORMATS_H
