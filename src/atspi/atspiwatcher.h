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

#ifndef DBUSWATCHER_H
#define DBUSWATCHER_H

#include <QtCore/qobject.h>
#include <QtDBus/qdbusvirtualobject.h>
#include <QtDBus/QDBusVariant>
#include <QtDBus/qdbuscontext.h>

#include <QtCore/QSharedPointer>

#include "dbusconnection.h"
#include "qt-atspi.h"

#include "accessibleobject.h"

class AtspiWatcher :public QObject, public QDBusContext
{
    Q_OBJECT
public:
    static AtspiWatcher *instance();
    AtspiWatcher();
    void init();
    QDBusConnection connection() const {return c.connection();}

Q_SIGNALS:
    void signalWindowActivated(QSharedPointer<AccessibleObject>);
    void signalWindowCreated(QSharedPointer<AccessibleObject>);
    void signalFocusChanged(QSharedPointer<AccessibleObject>);

private Q_SLOTS:
    void slotWindowActivated(const QString &change, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    void slotWindowCreated(const QString &change, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    void slotStateChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    void slotChildrenChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    void slotPropertyChange(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);

private:
    AccessibleObject *accessibleFromPath(const QString &service, const QString &path) const;
//    QString introspect(const QString &path) const;
//    bool handleMessage(const QDBusMessage &message, const QDBusConnection &connection);

    DBusConnection c;
};

#endif
