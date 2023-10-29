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
#include "QFileDialog"
#include "QBluetoothServiceDiscoveryAgent"
#include "QKeyEvent"
#include "QLowEnergyController"
#include "QMessageBox"
#include "QMetaEnum"
#include "QTimer"
#include "ui_qdopencomport.h"

struct Service {
    int Index;
    QList<QLowEnergyCharacteristic> QLowEnergyCharacteristics;
};

namespace Ui {
    class QDOpenComPort;
}

class QDOpenComPort : public QDialog
{
    Q_OBJECT

public:
    explicit QDOpenComPort(QWidget *parent = nullptr);
    ~QDOpenComPort();
    Ui::QDOpenComPort *ui;

private slots:
    void BluetoothLowEnergyCharacteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void BluetoothLowEnergyCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void BluetoothLowEnergyDeviceDiscovered(const QBluetoothDeviceInfo &info);
    void BluetoothLowEnergyError(QBluetoothDeviceDiscoveryAgent::Error error);
    void BluetoothLowEnergyFinished();
    void BluetoothLowEnergyConnected();
    void BluetoothLowEnergyError(QLowEnergyController::Error newError);
    void BluetoothLowEnergyDisconnected();
    void BluetoothLowEnergyDiscoveryFinished();
    //void BluetoothLowEnergyOnRead();
    void BluetoothLowEnergyServiceDiscovered(const QBluetoothUuid &newService);
    void BluetoothLowEnergyStateChanged(QLowEnergyService::ServiceState newState);
    bool eventFilter(QObject *object, QEvent *event);
    void on_QCBComPort_currentIndexChanged(int index);
    void on_QLEServer_returnPressed();
    void on_QLEServer_textChanged(QString );
    void on_QLESslCertificate_textChanged(const QString &arg1);
    void on_QLESslKeyPrivate_textChanged(const QString &arg1);
    void on_QRBBluetoothLowEnergy_toggled(bool checked);
    void on_QRBSslKeyCertificateEmbedded_toggled(bool checked);
    void on_QRBTCPClient_toggled(bool checked);
    void on_QRBTCPClientSsl_toggled(bool checked);
    void on_QRBTCPServer_toggled(bool checked);
    void on_QRBTCPServerSsl_toggled(bool checked);
    void on_QRBRS232_toggled(bool checked);
    void on_QPBCancel_clicked();
    void on_QPBBluetoothLowEnergyConnectCharacteristicsGet_clicked();
    void on_QPBBluetoothLowEnergyConnect_clicked();
    void on_QPBBluetoothLowEnergyScan_clicked();
    void on_QPBHelp_clicked();
    void on_QPBOk_clicked();
    void on_QSBSocket_valueChanged(int arg1);
    void on_QTBSslKeyPrivate_clicked();
    void on_QTBSslCertificate_clicked();

    //void on_pushButton_clicked();

private:
    QBluetoothDeviceDiscoveryAgent *pQBluetoothDeviceDiscoveryAgent= nullptr;
    QList<QBluetoothDeviceInfo> QLDevices;
    QList<QBluetoothUuid> QLBluetoothUuids;
    QList<QLowEnergyService*> QLServices;
    /*QLowEnergyCharacteristic LowEnergyCharacteristicRead;
    QLowEnergyCharacteristic LowEnergyCharacteristicWrite;*/
    QLowEnergyController *pQLowEnergyController= nullptr;
    QLowEnergyService *pQLowEnergyService= nullptr;
    QVector<Service> QVServices;
    //QLowEnergyService::WriteMode LowEnergyServiceWriteMode;
    //QTimer Timer;
    void LowEnergyCharacteristicsParse(Service &service);

};

#endif // QDOPENCOMPORT_H
