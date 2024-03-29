/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef DBUSCONNECTION_H
#define DBUSCONNECTION_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusPendingCallWatcher>

namespace QAccessibleClient {


/**
    Connection to the a11y dbus bus.
    \internal
 */
class DBusConnection : public QObject
{
    Q_OBJECT
public:
    /**
        \brief Constructor.

        When called, means the instance is created, we try instantly
        to fetch the \a connection . When done the \a connectionFetched
        will be emitted. If \a connection is called before it will
        block till the connection was fetched and is ready for use.
     */
    DBusConnection();

    /**
        \brief Returns true if the \a connection is not ready yet.

        The initial fetch of the comnnection happens over dbus and
        as such can block for a longer time means may need longer.
        To make it possible that users of this class do not block
        while that happens it is possible to use this method to
        determinate if fetching the connection is currently work
        in progress and if so connect with the \a connectionFetched
        signal to be called back when the connection is ready.
     */
    bool isFetchingConnection() const;

    /**
        \brief Returns the accessibility dbus connection.

        This may either be the session bus or a referenced
        accessibility bus. If the connection was not fetched
        yet, means \a isFetchingConnection returns true, then
        calling this method will block till the connection was
        fetched.
     */
    QDBusConnection connection() const;

    enum Status {
        Disconnected,
        ConnectionError,
        Connected
    };

    /**
        \brief Returns the state the connection is in.

        If Disconnected then we got not asked to connect yet or
        connection is in progress but not finished yet (see
        signal \a connectionFetched which will be emitted if the
        connection is not Disconnected any longer.
        If connection failed then the state is set to ConnectionError
        otherwise, in the case everything went fine and we are
        proper connected with tthe atspi daemon now Connected
        is returned.
     */
    Status status() const;

Q_SIGNALS:

    /**
        \brief Emitted when the \a connection was fetched.

        This will happen exactly one time during the lifetime
        of a DBusConnection instance at the very beginning when
        the instance is created.
     */
    void connectionFetched();

private Q_SLOTS:
    void initFinished();

private:
    void init();

    QDBusConnection m_connection;
    mutable Status m_status = Disconnected;
    QDBusPendingCallWatcher *m_initWatcher = nullptr;
};
}

#endif // DBUSCONNECTION_H
