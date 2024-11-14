#ifndef QDCOMMANDSSEQUENCE_H
#define QDCOMMANDSSEQUENCE_H

#include "QCheckBox"
#include <QDialog>
#include "QDomDocument"
#include "QLineEdit"
#include "QSpinBox"
#include "QTimer"
#include "ui_qdcommandssequence.h"
#ifndef QDTERMINAL_H
    class QDCommandsSequence;
    #include "qdterminal.h"
#endif

namespace Ui {
class QDCommandsSequence;
}

class QDCommandsSequence : public QDialog
{
    Q_OBJECT

public:
    explicit QDCommandsSequence(QWidget *parent = nullptr);
    ~QDCommandsSequence();
    Ui::QDCommandsSequence *ui;

signals:
    void OnClose();

private slots:
    void closeEvent(QCloseEvent *e);
    void on_QPBClose_clicked();
    void on_QPBExport_clicked();
    void on_QPBImport_clicked();
    void on_QPBRun_clicked();
    void Timeout();

private:
    int Index= 0;
    QDTerminal *pQDTerminal= nullptr;
    QTimer Timer;

};

#endif // QDCOMMANDSSEQUENCE_H
