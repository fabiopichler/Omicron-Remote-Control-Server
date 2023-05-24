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

#include "PluginManager.hpp"

#include "core/PluginBase.hpp"
#include "core/Config.hpp"

using ORC_PluginInitCallback = PluginBase *(*)(QObject *parent);

PluginManager::PluginManager() : m_pluginBase(nullptr)
{}

PluginManager::~PluginManager()
{
    delete m_pluginBase;

    if (m_library.isLoaded())
        m_library.unload();
}

bool PluginManager::load(const QString &event, QObject *parent)
{
    if (!event.contains(QRegExp(QStringLiteral(u"^[a-z_]{2,26}$"))))
        return false;

    QString fileName;

    {
        QStringList list = event.split(QStringLiteral(u"_"));

        for (int i = 0; i < list.size(); ++i)
        {
            QString str = list[i];

            if (i == 0)
                fileName.append(str);
            else if (!str.isEmpty())
                fileName.append(str.replace(0, 1, str[0].toUpper()));
        }
    }

    QString filePath;
    filePath.append(config()->getPluginsPath());
    filePath.append(QStringLiteral(u"/plugins/"));

#ifdef __unix
    filePath.append(QStringLiteral(u"lib"));
#endif

    filePath.append(QStringLiteral(u"Omicron-Remote-Control_"));
    filePath.append(fileName);

#ifdef __unix
    filePath.append(QStringLiteral(u".so"));
#else
    filePath.append(QStringLiteral(u".dll"));
#endif

    if (!QLibrary::isLibrary(filePath))
        return false;

    m_library.setFileName(filePath);

    if (!m_library.load())
        return false;

    auto ORC_PluginInit = reinterpret_cast<ORC_PluginInitCallback>(m_library.resolve("ORC_PluginInit"));

    if (!ORC_PluginInit)
        return false;

    if (!(m_pluginBase = ORC_PluginInit(parent)))
        return false;

    return true;
}
