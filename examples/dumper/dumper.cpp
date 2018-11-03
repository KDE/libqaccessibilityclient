/*
    Copyright 2018 Frederik Gladhorn <gladhorn@kde.org>

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

#include <QCoreApplication>
#include <QTextStream>

#include "dumper.h"

using namespace QAccessibleClient;

QTextStream out(stdout);

Dumper::Dumper(QObject *parent)
    : QObject(parent)
{
}

void Dumper::run(const QString &appname) const {
    Registry r;
    const auto apps = r.applications();
    for (const AccessibleObject &app : apps) {
        if (appname.isEmpty() || app.name().contains(appname)) {
            printChild(app);
        }
    }
}

void Dumper::printChild(const AccessibleObject &object, int indent) const
{
    auto spaces = QStringLiteral("  ");
    if (!object.isValid()) {
        out << spaces.repeated(indent) << "INVALID CHILD" << endl;
        return;
    }

    auto name = object.name().isEmpty() ? QStringLiteral("[unnamed]") : object.name();
    QString info = QString("%1 [%2 - %3] '%4'").arg(name, QString::number(object.role()), object.roleName(), object.description());
    if (m_showStates) {
        info += QString(" [%1]").arg(object.stateString());
    }
    out << spaces.repeated(indent) << info << endl;
    int childCount = object.childCount();
    for (int i = 0; i < childCount; ++i) {
        AccessibleObject child = object.child(i);
        // simple test if the parent is consistent
        if (child.parent() != object) {
            out << spaces.repeated(indent + 4) << "WARNING: inconsistent parent/child hierarchy";
        }
        if (child.indexInParent() != i) {
            out << spaces.repeated(indent + 4) << QString::fromLatin1("WARNING: child index inconsistent: child claims to be child %1 parent thinks it is %2").arg(child.indexInParent(), i);
        }
        printChild(child, indent + 1);
    }
}
