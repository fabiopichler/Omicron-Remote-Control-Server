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

#include "global.hpp"

#include <QObject>
#include <QJsonObject>

class ORC_CORE_EXPORT PluginBase : public QObject
{
    Q_OBJECT

public:
    PluginBase(const QString &name, QObject *parent);

    inline const QString &name() const { return m_name; }
    virtual void putData(const QJsonObject &data);

    QJsonObject getData();

signals:
    void update(QJsonObject data);

protected:
    virtual QJsonObject onExec(const QJsonObject &data) = 0;

    const QString m_name;
    QJsonObject m_data;
};

#define ORC_PLUGIN_MAIN(plugin_class) \
extern "C" PluginBase Q_DECL_EXPORT *ORC_PluginInit(QObject *parent) \
{ \
    return new plugin_class(parent); \
}
