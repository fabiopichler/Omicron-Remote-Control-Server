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
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostInfo>

class ORC_HIDDEN ClientSocketBase : public QObject
{
    Q_OBJECT

public:
    virtual void disconnectSocket() = 0;
    virtual void putData(const QJsonObject &data) = 0;

signals:
    void newClientSocket(ClientSocketBase *socket);
    void newData(ClientSocketBase *socket, const QJsonObject &data);
    void disconnected(ClientSocketBase *socket);
};

template <typename T>
class ORC_HIDDEN ClientSocket : public ClientSocketBase
{
public:
    explicit ClientSocket(T *socket)
        : m_socket(socket)
        , m_connected(false)
    {
        connect(socket, &T::disconnected, this, &ClientSocket<T>::onDisconnected);
        connect(socket, &T::readyRead, this, &ClientSocket<T>::onReadyRead);
    }

    ~ClientSocket() override
    {
        m_socket->deleteLater();
    }

    void disconnectSocket() override
    {
        disconnect(m_socket, &T::disconnected, this, &ClientSocket<T>::onDisconnected);
        m_socket->close();
    }

    void putData(const QJsonObject &data) override
    {
        m_socket->write(QJsonDocument(data).toJson());
    }

private:
    void onReadyRead()
    {
        QJsonParseError error{};

        QByteArray arr = m_socket->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(arr, &error);

        if (error.error != QJsonParseError::NoError || jsonDoc.isNull() || jsonDoc.isEmpty())
        {
            if (!m_connected)
                m_socket->close();

            return;
        }

        QJsonObject data = jsonDoc.object();

        if (!m_connected)
        {
            if (data[QStringLiteral(u"action")].toString() == QStringLiteral(u"new_connection"))
            {
                m_connected = true;

                QJsonObject json;
                json.insert(QStringLiteral(u"connection"), QStringLiteral(u"accepted"));
                json.insert(QStringLiteral(u"device_name"), QHostInfo::localHostName());
#ifdef __linux
                json.insert(QStringLiteral(u"device_os"), QStringLiteral(u"linux"));
#elif _WIN32
                json.insert(QStringLiteral(u"device_os"), QStringLiteral(u"windows"));
#else
                json.insert(QStringLiteral(u"device_os"), QStringLiteral(u"other"));
#endif
                putData(json);

                emit newClientSocket(this);
            }
            else
            {
                m_socket->close();
            }

            return;
        }

        emit newData(this, data);
    }

    void onDisconnected()
    {
        emit disconnected(this);
        deleteLater();
    }

private:
    T *m_socket;
    bool m_connected;
};
