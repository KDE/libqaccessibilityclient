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

#ifndef QT_ATSPI_H
#define QT_ATSPI_H
#define QSPI_OBJECT_PATH_ACCESSIBLE  "/org/a11y/atspi/accessible"
#define QSPI_OBJECT_PATH_ACCESSIBLE_NULL  QSPI_OBJECT_PATH_ACCESSIBLE"/null"

#include <QList>
#include <QString>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusArgument>

namespace QAccessibleClient {

/**
    Register meta types for dbus.
    \internal
 */
void registerDBusTypes();

/**
    The QSpiObjectReference struct is a dbus service and path representing a remote accessible object.
    \internal
 */
struct QSpiObjectReference
{
    QString service;
    QDBusObjectPath path;
};

typedef QList<QAccessibleClient::QSpiObjectReference> QSpiObjectReferenceList;

struct QSpiAction
{
    QString name;
    QString description;
    QString keyBinding;
};

typedef QList <QSpiAction> QSpiActionArray;

/**
    \internal
 */
QDBusArgument &operator<<(QDBusArgument &argument, const QSpiObjectReference &address);

/**
    \internal
 */
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiObjectReference &address);

/**
    \internal
 */
QDBusArgument &operator<<(QDBusArgument &argument, const QSpiAction &address);

/**
    \internal
 */
const QDBusArgument &operator>>(const QDBusArgument &argument, QSpiAction &address);

}

Q_DECLARE_METATYPE(QAccessibleClient::QSpiObjectReference);
Q_DECLARE_METATYPE(QAccessibleClient::QSpiObjectReferenceList);
Q_DECLARE_METATYPE(QAccessibleClient::QSpiAction)
Q_DECLARE_METATYPE(QAccessibleClient::QSpiActionArray)

#endif
