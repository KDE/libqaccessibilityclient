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


#include "dbusconnection.h"

#include <QtDBus/QDBusMessage>
#include <QtCore/QDebug>

using namespace KAccessibleClient;

DBusConnection::DBusConnection()
    : dbusConnection(connectDBus())
{}

QDBusConnection DBusConnection::connectDBus()
{
    QString address = getAccessibilityBusAddress();

    if (!address.isEmpty()) {
        QDBusConnection c = QDBusConnection::connectToBus(address, QLatin1String("a11y"));
        if (c.isConnected()) {
            qDebug() << "Connected to accessibility bus at: " << address;
            return c;
        }
        qWarning("Found Accessibility DBus address but cannot connect. Falling back to session bus.");
    } else {
        qWarning("Accessibility DBus not found. Falling back to session bus.");
    }

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("Could not connect to DBus.");
    }
    return QDBusConnection::sessionBus();
}

QString DBusConnection::getAccessibilityBusAddress() const
{
    QString address = getAccessibilityBusAddressDBus();
    return address;
}

QString DBusConnection::getAccessibilityBusAddressDBus() const
{
    QDBusConnection c = QDBusConnection::sessionBus();

    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.Bus"),
                                                    QLatin1String("/org/a11y/bus"),
                                                    QLatin1String("org.a11y.Bus"), QLatin1String("GetAddress"));
    QDBusMessage reply = c.call(m);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Qt at-spi: error getting the accessibility dbus address: " << reply.errorMessage();
        return QString();
    }

    QString busAddress = reply.arguments().at(0).toString();
    qDebug() << "Got bus address: " << busAddress;
    return busAddress;
}

QDBusConnection DBusConnection::connection() const
{
    return dbusConnection;
}
