/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef QT_ATSPI_H
#define QT_ATSPI_H
#define QSPI_OBJECT_PATH_ACCESSIBLE  "/org/a11y/atspi/accessible"
#define QSPI_OBJECT_PATH_ACCESSIBLE_NULL  QSPI_OBJECT_PATH_ACCESSIBLE"/null"

#include <QList>
#include <QString>
#include <QDBusArgument>

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
