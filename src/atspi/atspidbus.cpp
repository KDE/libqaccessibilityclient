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

#include "atspidbus.h"

#include <qdbusmessage.h>
#include <qdbusreply.h>
#include <qdbusargument.h>

#include <qdebug.h>

#include "atspi/qt-atspi.h"
#include "accessible/accessibleobject_p.h"

using namespace KAccessibleClient;

AtSpiDBus::AtSpiDBus(DBusConnection *conn)
    : m_connection(conn)
{
}

AtSpiDBus::~AtSpiDBus()
{
}

AccessibleObject AtSpiDBus::parent(const AccessibleObject &object)
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetParent"));

    QDBusReply<QVariant> reply = m_connection->connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access parent." << reply.error().message();
        return AccessibleObject(0, QString(), QString());
    }

    QVariant v = reply.value();
    const QDBusArgument arg = v.value<QDBusArgument>();
    QSpiObjectReference ref;
    arg >> ref;

    return AccessibleObject(const_cast<AtSpiDBus*>(this), ref.service, ref.path.path());
}

QList<AccessibleObject> AtSpiDBus::children(const AccessibleObject &object) const
{
    QList<AccessibleObject> accs;

    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetChildren"));

    QDBusReply<QSpiObjectReferenceList> reply = m_connection->connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access children." << reply.error().message();
        return accs;
    }

    const QSpiObjectReferenceList children = reply.value();
    Q_FOREACH(const QSpiObjectReference &child, children) {
        accs.append(AccessibleObject(const_cast<AtSpiDBus*>(this), child.service, child.path.path()));
    }

    return accs;
}

QList<AccessibleObject> AtSpiDBus::topLevelAccessibles() const
{
    QString service = QLatin1String("org.a11y.atspi.Registry");
    QString path = QLatin1String("/org/a11y/atspi/accessible/root");
    return children(AccessibleObject(const_cast<AtSpiDBus*>(this), service, path));
}

QString AtSpiDBus::name(const QString &service, const QString &path) const
{
    if (service.isEmpty() || path.isEmpty())
        return QLatin1String("Invalid Object");
    return getProperty(service, path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("Name")).toString();
}

QVariant AtSpiDBus::getProperty(const QString &service, const QString &path, const QString &interface, const QString &name) const
{
    QVariantList args;
    args.append(interface);
    args.append(name);

    QDBusMessage message = QDBusMessage::createMethodCall (
                service, path, QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get"));

    message.setArguments(args);
    QDBusMessage reply = m_connection->connection().call(message);
    if (reply.arguments().isEmpty()) return QVariant();

    QDBusVariant v = reply.arguments().at(0).value<QDBusVariant>();
    return v.variant();
}

