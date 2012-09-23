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

#ifndef DBUSCONNECTION_H
#define DBUSCONNECTION_H

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusPendingCallWatcher>

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

signals:

    /**
        \brief Emitted when the \a connection was fetched.

        This will happen exactly one time during the lifetime
        of a DBusConnection instance at the very beginning when
        the instance is created.
     */
    void connectionFetched();

private slots:
    void initFinished();

private:
    void init();

    QDBusConnection m_connection;
    mutable Status m_status;
    QDBusPendingCallWatcher *m_initWatcher;
};
}

#endif // DBUSCONNECTION_H
