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

#include "ServerManager.hpp"
#include "ClientSocketList.hpp"
#include "TcpServer.hpp"
#include "BluetoothServer.hpp"
#include "PluginManager.hpp"
#include "ClientSocket.hpp"
#include "core/PluginBase.hpp"

#include <QSysInfo>

ServerManager::ServerManager(QObject *parent)
    : QObject(parent)
    , m_clientSocketList(new ClientSocketList(this))
    , m_tcpServer(nullptr)
    , m_bluetoothServer(nullptr)
    , m_running(false)
{
    connect(m_clientSocketList, &ClientSocketList::updateListeners, this, &ServerManager::updateListeners);
}

ServerManager::~ServerManager()
{
    for (auto plugin : m_plugins)
        delete plugin;

    stop();

    delete m_clientSocketList;
}

int ServerManager::countListeners() const
{
    return m_clientSocketList->countListeners();
}

void ServerManager::startTcpServer(int port)
{
    if (m_tcpServer)
        return;

    initManager();
    m_tcpServer = new TcpServer(this, m_clientSocketList, port);
}

void ServerManager::startBluetoothServer()
{
#ifdef Q_OS_WIN32
    if (QSysInfo::productVersion() != "10" && QSysInfo::productVersion() != "11")
        return;
#endif

    if (m_bluetoothServer)
        return;

    initManager();
    m_bluetoothServer = new BluetoothServer(this, m_clientSocketList);
}

void ServerManager::stop()
{
    m_clientSocketList->stop();

    delete m_tcpServer;
    delete m_bluetoothServer;

    m_tcpServer = nullptr;
    m_bluetoothServer = nullptr;

    m_running = false;
    emit statusChanged(false);
}

void ServerManager::receiveData(ClientSocketBase *client, const QJsonObject &data)
{
    QString event = data[QStringLiteral(u"event")].toString();

    if (!m_plugins.contains(event))
    {
        auto pluginManager = new PluginManager;

        if (pluginManager->load(event, this))
        {
            connect(pluginManager->get(), &PluginBase::update, this, &ServerManager::onUpdate);
            m_plugins.insert(event, pluginManager);
        }
        else
        {
            delete pluginManager;
            return;
        }
    }

    PluginBase *plugin = m_plugins[event]->get();

    if (data[QStringLiteral(u"data")].toObject()[QStringLiteral(u"action")].toString() == QStringLiteral(u"subscribe"))
    {
        client->putData(plugin->getData());
        return;
    }

    plugin->putData(data[QStringLiteral(u"data")].toObject());

    m_clientSocketList->putData(plugin->getData());
}

void ServerManager::initManager()
{
    m_running = true;
    emit statusChanged(true);
}

void ServerManager::onUpdate(const QJsonObject& data)
{
    m_clientSocketList->putData(data);
}
