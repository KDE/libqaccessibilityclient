/*
    Copyright 2012 Frederik Gladhorn <gladhorn@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"

#include <qplaintextedit.h>
#include <qstring.h>

#include "accessible/registry.h"

MainWindow::MainWindow(QWidget *parent)
    :KMainWindow(parent)
{
    ui.setupUi(this);

    KAccessibleClient::Registry *reg = new KAccessibleClient::Registry(this);
    reg->subscribeEventListeners(KAccessibleClient::Registry::Focus);
    connect(reg, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)), this, SLOT(focusChanged(KAccessibleClient::AccessibleObject)));

    listAccessibles();
}

void MainWindow::listAccessibles()
{
    QString accessibles;

    KAccessibleClient::Registry registry;
    QList<KAccessibleClient::AccessibleObject> apps = registry.applications();
    accessibles += "Accessible applications:" + QString::number(apps.count()) + '\n';
    foreach(const KAccessibleClient::AccessibleObject &obj, apps) {
        accessibles += QString("App: %1  (parent: %2)\n").arg(obj.name(), obj.parent().name());
        foreach(const KAccessibleClient::AccessibleObject &child, obj.children()) {
            accessibles += QString(" Window: %1  (parent: %2)\n").arg(child.name(), child.parent().name());
        }
        accessibles.append('\n');
    }
    ui.plainTextEdit->setReadOnly(true);
    ui.plainTextEdit->setPlainText(accessibles);
}

void MainWindow::focusChanged(const KAccessibleClient::AccessibleObject &object)
{
    ui.label->setText(i18n("Focus changed to: %1 - %2 (%3)", object.name(), object.roleName(), object.role()));
}

#include <mainwindow.moc>

