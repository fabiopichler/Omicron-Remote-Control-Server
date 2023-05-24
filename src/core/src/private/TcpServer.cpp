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

#include "TcpServer.hpp"

#include <QTcpServer>
#include <QTcpSocket>

TcpServer::TcpServer(QObject *parent, ClientSocketList *socketList, int port)
    : ServerBase<QTcpServer, QTcpSocket>(parent, socketList)
    , m_port(port)
{
    startServer();
}

TcpServer::~TcpServer() = default;

void TcpServer::startServer()
{
    if (m_server)
        return;

    m_server = new QTcpServer(this);

    connect(m_server, &QTcpServer::newConnection, this, &TcpServer::newConnection);

    if (!m_server->listen(QHostAddress::Any, m_port))
        throw std::runtime_error("Server could not start");
}