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

#ifndef LIBKDEACCESSIBILITYCLIENT_REGISTRY_P_H
#define LIBKDEACCESSIBILITYCLIENT_REGISTRY_P_H

#include <atspi/atspi-constants.h>

#include <qobject.h>
#include <qdbuscontext.h>
#include <qdbusargument.h>

#include "atspi/dbusconnection.h"
#include "kdeaccessibilityclient/registry.h"
#include "kdeaccessibilityclient/accessibleobject.h"
#include "atspi/qt-atspi.h"

namespace KAccessibleClient {

class DBusConnection;
class AtSpiDBus;

class RegistryPrivate :public QObject, public QDBusContext
{
    Q_OBJECT
public:
    RegistryPrivate(Registry *qq)
        :q(qq)
    {
    }

    void init();

    void subscribeEventListeners(const Registry::EventListeners & listeners);
    Registry::EventListeners eventListeners() const;

    QString name(const AccessibleObject &object) const;
    QString description(const AccessibleObject &object) const;
    AtspiRole role(const AccessibleObject &object) const;
    QString roleName(const AccessibleObject &object) const;
    QString localizedRoleName(const AccessibleObject &object) const;

    QList<AccessibleObject> topLevelAccessibles() const;
    AccessibleObject parentAccessible(const AccessibleObject &object) const;

    int childCount(const AccessibleObject &object) const;
    AccessibleObject child(const AccessibleObject &object, int index) const;
    QList<AccessibleObject> children(const AccessibleObject &object) const;

Q_SIGNALS:
    void focusChanged(const KAccessibleClient::AccessibleObject &object);

private Q_SLOTS:
    void slotChildrenChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    void slotPropertyChange(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    void slotStateChanged(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QSpiObjectReference &reference);
    void slotWindowActivated(const QString &change, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    void slotWindowCreated(const QString &change, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference);
    AccessibleObject accessibleFromPath(const QString &service, const QString &path) const;

private:
    QVariant getProperty ( const QString &service, const QString &path, const QString &interface, const QString &name ) const;

    DBusConnection conn;
    Registry *q;
};

}

#endif
