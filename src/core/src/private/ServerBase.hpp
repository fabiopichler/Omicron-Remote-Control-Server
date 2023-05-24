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
#include "ClientSocket.hpp"
#include "ClientSocketList.hpp"

template <typename T_Server, typename T_Socket>
class ORC_HIDDEN ServerBase : public QObject
{
public:
    ServerBase(QObject *parent, ClientSocketList *socketList)
        : QObject(parent)
        , m_server(nullptr)
        , m_clientSocketList(socketList)
    { }

    ~ServerBase() override
    {
        if (m_server)
        {
            m_server->close();
            delete m_server;
        }
    }

protected:
    void newConnection()
    {
        T_Socket *socket = m_server->nextPendingConnection();

        connect(new ClientSocket<T_Socket>(socket), &ClientSocket<T_Socket>::newClientSocket,
                m_clientSocketList, &ClientSocketList::onClientConnected);
    }

protected:
    T_Server *m_server;
    ClientSocketList *m_clientSocketList;
};
