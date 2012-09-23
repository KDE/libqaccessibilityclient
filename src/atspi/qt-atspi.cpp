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

#include "qt-atspi.h"
#include <QtDBus/QDBusArgument>
#include <QtDBus/qdbusmetatype.h>

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
