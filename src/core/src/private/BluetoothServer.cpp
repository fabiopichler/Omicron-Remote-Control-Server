/*-------------------------------------------------------------------------------

Copyright (c) 2023 Fábio Pichler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------------------------------------------------------*/

#include "BluetoothServer.hpp"
#include "core/Bluetooth.hpp"

#include <qbluetoothserviceinfo.h>
#include <qbluetoothaddress.h>

#include <QtCore/QObject>
#include <QtCore/QLinkedList>

#include <qbluetoothserver.h>
#include <qbluetoothsocket.h>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <iostream>

static const QString serviceUuid(QStringLiteral(u"BA9598DB-41EF-4A14-95BD-6A2A865430D8"));

BluetoothServer::BluetoothServer(QObject *parent, ClientSocketList *socketList)
    : ServerBase<QBluetoothServer, QBluetoothSocket>(parent, socketList)
{
    startServer();
}

BluetoothServer::~BluetoothServer()
{
    m_serviceInfo.unregisterService();
}

void BluetoothServer::startServer()
{
    if (m_server)
        return;

    if (!Bluetooth::localDeviceIsValid())
        throw std::runtime_error("Local device is not valid");

    m_server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(m_server, &QBluetoothServer::newConnection, this, &BluetoothServer::newConnection);

    const QBluetoothAddress localAdapter;
    bool result = m_server->listen(localAdapter);

    if (!result)
    {
        delete m_server;
        m_server = nullptr;

        throw std::runtime_error(QString(QStringLiteral(u"Cannot bind server to") + localAdapter.toString()).toUtf8());
    }

    //serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceRecordHandle, (uint)0x00010010);

    QBluetoothServiceInfo::Sequence profileSequence;
    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
    classId << QVariant::fromValue(quint16(0x100));
    profileSequence.append(QVariant::fromValue(classId));
    m_serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                             profileSequence);

    classId.clear();
    classId << QVariant::fromValue(QBluetoothUuid(serviceUuid));
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));

    m_serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

    m_serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, QStringLiteral(u"Omicron Remote Control Server"));
    m_serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             QStringLiteral(u"Omicron Remote Control Server"));
    m_serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, QStringLiteral(u"fabiopichler.net"));

    m_serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));

    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
    m_serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             publicBrowse);

    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(m_server->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    m_serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);

    m_serviceInfo.registerService(localAdapter);
}
