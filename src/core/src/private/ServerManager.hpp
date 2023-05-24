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

#pragma once

#include "core/global.hpp"

#include <QObject>
#include <QHash>
#include <QJsonObject>

class ClientSocketList;
class TcpServer;
class BluetoothServer;
class ClientSocketBase;
class PluginManager;

class ORC_HIDDEN ServerManager : public QObject
{
    Q_OBJECT

public:
    explicit ServerManager(QObject *parent);
    ~ServerManager() override;

    int countListeners() const;

    inline bool isRunning() const { return m_running; }

    void startTcpServer(int port);
    void startBluetoothServer();
    void stop();

public slots:
    void receiveData(ClientSocketBase *client, const QJsonObject &data);

signals:
    void statusChanged(bool started);
    void updateListeners(int count);

private:
    void initManager();

private slots:
    void onUpdate(const QJsonObject& data);

private:
    ClientSocketList *m_clientSocketList;
    TcpServer *m_tcpServer;
    BluetoothServer *m_bluetoothServer;

    QHash<QString, PluginManager *> m_plugins;

    bool m_running;
};
