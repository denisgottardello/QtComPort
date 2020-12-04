#include "qdterminallogformats.h"

QDTerminalLogFormats::QDTerminalLogFormats(QWidget *parent) : QDialog(parent), ui(new Ui::QDTerminalLogFormats) {
    ui->setupUi(this);
}

QDTerminalLogFormats::~QDTerminalLogFormats() {
    delete ui;
}

void QDTerminalLogFormats::on_QPBCancel_clicked() {
    this->reject();
}

void QDTerminalLogFormats::on_QPBOk_clicked() {
    this->accept();
}
