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

#include "MonitorPlugin.hpp"

#ifdef __unix
  #include "linux/Monitor_Linux.hpp"
#else
  #include "windows/Monitor_Win32.hpp"
#endif

ORC_PLUGIN_MAIN(MonitorPlugin)

MonitorPlugin::MonitorPlugin(QObject *parent)
    : PluginBase(QStringLiteral(u"monitor"), parent)
{
}

MonitorPlugin::~MonitorPlugin() = default;

QJsonObject MonitorPlugin::onExec(const QJsonObject &data)
{
    QString action = data[QStringLiteral(u"action")].toString();
    QString value = data[QStringLiteral(u"value")].toString();

    if (action == QStringLiteral(u"on"))
        Monitor::on();

    else if (action == QStringLiteral(u"off"))
        Monitor::off();

    QJsonObject responseJson;

    //responseJson.insert(QStringLiteral(u"status"), Monitor::status());

    return responseJson;
}
