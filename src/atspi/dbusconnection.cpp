/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/


#include "dbusconnection.h"

#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QDebug>

using namespace QAccessibleClient;

DBusConnection::DBusConnection()
    : QObject()
    , m_connection(QDBusConnection::sessionBus())
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

    QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.a11y.Bus"),
                                                    QStringLiteral("/org/a11y/bus"),
                                                    QStringLiteral("org.a11y.Bus"), QStringLiteral("GetAddress"));

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
        QDBusConnection c = QDBusConnection::connectToBus(busAddress, QStringLiteral("a11y"));
        if (c.isConnected()) {
            qDebug() << "Connected to Accessibility DBus at address=" << busAddress;
            m_connection = c;
            m_status = Connected;
        } else {
            qWarning() << "Found Accessibility DBus address=" << busAddress << "but cannot connect. Falling back to session bus.";
        }
    }
    m_initWatcher->deleteLater();
    m_initWatcher = nullptr;
    Q_EMIT connectionFetched();
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

#include "moc_dbusconnection.cpp"
