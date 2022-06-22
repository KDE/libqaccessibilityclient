/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "qt-atspi.h"
#include <QDBusMetaType>

namespace QAccessibleClient {

void registerDBusTypes()
{
    qRegisterMetaType<QAccessibleClient::QSpiObjectReference>();
    qDBusRegisterMetaType<QAccessibleClient::QSpiObjectReference>();

    qRegisterMetaType<QAccessibleClient::QSpiObjectReferenceList>();
    qDBusRegisterMetaType<QAccessibleClient::QSpiObjectReferenceList>();

    qRegisterMetaType<QAccessibleClient::QSpiAction>();
    qDBusRegisterMetaType<QAccessibleClient::QSpiAction>();

    qRegisterMetaType<QAccessibleClient::QSpiActionArray>();
    qDBusRegisterMetaType<QAccessibleClient::QSpiActionArray>();
}

/* QSpiObjectReference */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QAccessibleClient::QSpiObjectReference &address)
{
    argument.beginStructure();
    argument << address.service;
    argument << address.path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QAccessibleClient::QSpiObjectReference &address)
{
    argument.beginStructure();
    argument >> address.service;
    argument >> address.path;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const QAccessibleClient::QSpiAction &action)
{
    argument.beginStructure();
    argument << action.name;
    argument << action.description;
    argument << action.keyBinding;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QAccessibleClient::QSpiAction &action)
{
    argument.beginStructure();
    argument >> action.name;
    argument >> action.description;
    argument >> action.keyBinding;
    argument.endStructure();
    return argument;
}

}
