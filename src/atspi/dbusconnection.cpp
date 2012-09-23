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
#include <QtDBus/QDBusPendingReply>
#include <QtCore/QDebug>

using namespace QAccessibleClient;

DBusConnection::DBusConnection()
    : QObject()
    , m_connection(QDBusConnection::sessionBus())
    , m_initWatcher(0)
    , m_status(Disconnected)
{
    init();
}

void DBusConnection::init()
{
    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected()) {
        qWarning("Could not connect to DBus session bus.");
        return;
    }

    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.Bus"),
                                                    QLatin1String("/org/a11y/bus"),
                                                    QLatin1String("org.a11y.Bus"), QLatin1String("GetAddress"));

    QDBusPendingCall async = c.asyncCall(m);
    m_initWatcher = new QDBusPendingCallWatcher(async, this);
    connect(m_initWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(initFinished()));
}

void DBusConnection::initFinished()
{
    if (!m_initWatcher)
        return;
    m_status = ConnectionError;
    QDBusPendingReply<QString> reply = *m_initWatcher;
    if (reply.isError() || reply.value().isEmpty()) {
        qWarning() << "Accessibility DBus not found. Falling back to session bus.";
    } else {
        QString busAddress = reply.value();
        qDebug() << "Got Accessibility DBus address:" << busAddress;
        QDBusConnection c = QDBusConnection::connectToBus(busAddress, QLatin1String("a11y"));
        if (c.isConnected()) {
            qDebug() << "Connected to Accessibility DBus at address=" << busAddress;
            m_connection = c;
            m_status = Connected;
        } else {
            qWarning() << "Found Accessibility DBus address=" << busAddress << "but cannot connect. Falling back to session bus.";
        }
    }
    m_initWatcher->deleteLater();
    m_initWatcher = 0;
    emit connectionFetched();
}

bool DBusConnection::isFetchingConnection() const
{
    return m_initWatcher;
}

QDBusConnection DBusConnection::connection() const
{
    if (m_initWatcher) {
        m_initWatcher->waitForFinished();
        const_cast<DBusConnection*>(this)->initFinished();
    }
    return m_connection;
}

DBusConnection::Status DBusConnection::status() const
{
    return m_status;
}
