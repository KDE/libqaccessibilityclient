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

void QtATSPI::registerTypes()
{
  qRegisterMetaType<QSpiUIntList>();
  qRegisterMetaType<QSpiObjectReference>();
  qDBusRegisterMetaType<QSpiUIntList>();
  qDBusRegisterMetaType<QSpiObjectReference>();
}


QSpiObjectReference::QSpiObjectReference()
    : path(QDBusObjectPath(QSPI_OBJECT_PATH_ACCESSIBLE_NULL))
{}

/* QSpiObjectReference */
/*---------------------------------------------------------------------------*/

QDBusArgument &operator<<(QDBusArgument &argument, const QSpiObjectReference &address)
{
    argument.beginStructure();
    argument << address.service;
    argument << address.path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiObjectReference &address)
{
    argument.beginStructure();
    argument >> address.service;
    argument >> address.path;
    argument.endStructure();
    return argument;
}
