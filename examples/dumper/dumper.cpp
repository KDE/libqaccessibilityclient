/*
    SPDX-FileCopyrightText: 2018 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
        out << spaces.repeated(indent) << "INVALID CHILD" << Qt::endl;
        return;
    }

    auto name = object.name().isEmpty() ? QStringLiteral("[unnamed]") : object.name();
    QString info = QStringLiteral("%1 [%2 - %3] '%4'").arg(name, QString::number(object.role()), object.roleName(), object.description());
    if (m_showStates) {
        info += QStringLiteral(" [%1]").arg(object.stateString());
    }
    out << spaces.repeated(indent) << info << Qt::endl;
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

#include "moc_dumper.cpp"
