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

#include "AboutDialog.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    createWidgets();

    setWindowTitle(QStringLiteral(u"Sobre o Omicron Remote Control"));
}

void AboutDialog::createWidgets()
{
    auto label = new QLabel(
                QStringLiteral(
                u"<div style=\"text-align:center\">"
                u"  <h2>Omicron Remote Control - Server</h2>"
                u"  <h3>&copy; 2021, Fábio Pichler</h3>"
                u"  <p>"
                      u"Um servidor para o aplicativo mobile <strong>Omicron Remote Control</strong>"
                      u"<br>"
                      u"<a href=\"https://play.google.com/store/apps/details?id=net.fabiopichler.omicronremotecontrol\">Ver aplicativo na Google Play</a>"
                  u"</p>"
                u"</div>")
    );

    label->setOpenExternalLinks(true);

    auto button = new QPushButton(QStringLiteral(u"Ok!"));
    connect(button, &QPushButton::clicked, this, &AboutDialog::close);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(button, 0, Qt::AlignHCenter);

    setLayout(mainLayout);
}
