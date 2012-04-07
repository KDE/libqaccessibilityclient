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

#include "registry.h"
#include "registry_p.h"



#include <qdbusmessage.h>
#include <qdbusargument.h>
#include <qdbusreply.h>

#include <qdebug.h>
#include <qdbusmessage.h>
#include <qstringlist.h>

#include "atspi/atspidbus.h"
#include "atspi/qt-atspi.h"
#include "atspi/dbusconnection.h"

// interface names from at-spi2-core/atspi/atspi-misc-private.h
#define ATSPI_DBUS_NAME_REGISTRY "org.a11y.atspi.Registry"
#define ATSPI_DBUS_PATH_REGISTRY "/org/a11y/atspi/registry"
#define ATSPI_DBUS_INTERFACE_REGISTRY "org.a11y.atspi.Registry"

#define ATSPI_DBUS_PATH_NULL "/org/a11y/atspi/null"
#define ATSPI_DBUS_PATH_ROOT "/org/a11y/atspi/accessible/root"

#define ATSPI_DBUS_PATH_DEC "/org/a11y/atspi/registry/deviceeventcontroller"
#define ATSPI_DBUS_INTERFACE_DEC "org.a11y.atspi.DeviceEventController"
#define ATSPI_DBUS_INTERFACE_DEVICE_EVENT_LISTENER "org.a11y.atspi.DeviceEventListener"

#define ATSPI_DBUS_INTERFACE_CACHE "org.a11y.atspi.Cache"
#define ATSPI_DBUS_INTERFACE_ACCESSIBLE "org.a11y.atspi.Accessible"
#define ATSPI_DBUS_INTERFACE_ACTION "org.a11y.atspi.Action"
#define ATSPI_DBUS_INTERFACE_APPLICATION "org.a11y.atspi.Application"
#define ATSPI_DBUS_INTERFACE_COLLECTION "org.a11y.atspi.Collection"
#define ATSPI_DBUS_INTERFACE_COMPONENT "org.a11y.atspi.Component"
#define ATSPI_DBUS_INTERFACE_DOCUMENT "org.a11y.atspi.Document"
#define ATSPI_DBUS_INTERFACE_EDITABLE_TEXT "org.a11y.atspi.EditableText"
#define ATSPI_DBUS_INTERFACE_EVENT_KEYBOARD "org.a11y.atspi.Event.Keyboard"
#define ATSPI_DBUS_INTERFACE_EVENT_MOUSE "org.a11y.atspi.Event.Mouse"
#define ATSPI_DBUS_INTERFACE_EVENT_OBJECT "org.a11y.atspi.Event.Object"
#define ATSPI_DBUS_INTERFACE_HYPERLINK "org.a11y.atspi.Hyperlink"
#define ATSPI_DBUS_INTERFACE_HYPERTEXT "org.a11y.atspi.Hypertext"
#define ATSPI_DBUS_INTERFACE_IMAGE "org.a11y.atspi.Image"
#define ATSPI_DBUS_INTERFACE_SELECTION "org.a11y.atspi.Selection"
#define ATSPI_DBUS_INTERFACE_TABLE "org.a11y.atspi.Table"
#define ATSPI_DBUS_INTERFACE_TEXT "org.a11y.atspi.Text"
#define ATSPI_DBUS_INTERFACE_VALUE "org.a11y.atspi.Value"
#define ATSPI_DBUS_INTERFACE_SOCKET "org.a11y.atspi.Socket"

// missing from at-spi2-core:
#define ATSPI_DBUS_INTERFACE_EVENT_WINDOW "org.a11y.atspi.Event.Window"
#define ATSPI_DBUS_INTERFACE_EVENT_FOCUS  "org.a11y.atspi.Event.Focus"

#define QSPI_OBJECT_PATH_ACCESSIBLE  "/org/a11y/atspi/accessible"
#define QSPI_OBJECT_PATH_PREFIX      "/org/a11y/atspi/accessible/"
#define QSPI_OBJECT_PATH_ROOT    QSPI_OBJECT_PATH_PREFIX "root"

#define QSPI_REGISTRY_NAME "org.a11y.atspi.Registry"


namespace KAccessibleClient {

Registry::Registry()
    : d(new RegistryPrivate())
{
    registerDBusTypes();

    d->conn = new DBusConnection();
    d->bus = new AtSpiDBus(d->conn);
}

Registry::~Registry()
{
    delete d->bus;
    delete d->conn;
    delete d;
}

void Registry::subscribeEventListeners(const EventListeners &listeners) const
{
    Q_UNUSED(listeners);
    d->bus->subscribeEventListeners(listeners);
}

QList<AccessibleObject> Registry::applications() const
{
    return d->bus->topLevelAccessibles();
}

#define ATSPI_DEBUG

void RegistryPrivate::slotWindowCreated(const QString &change, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
#ifdef ATSPI_DEBUG
    qDebug() << "New window: " << change << detail1 << detail2 << args.variant() << reference.path.path();
#endif
//    AccessibleObject accessible = accessibleFromPath(reference.service, QDBusContext::message().path());
    //emit signalWindowCreated(QSharedPointer<AccessibleObject>(accessible));
}

void RegistryPrivate::slotWindowActivated(const QString &change, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
#ifdef ATSPI_DEBUG
    qDebug() << "Window activated: " << change << detail1 << detail2 << args.variant() << reference.path.path();
#endif
//    AccessibleObject accessible = accessibleFromPath(reference.service, QDBusContext::message().path());
//    emit signalWindowActivated(QSharedPointer<AccessibleObject>(accessible));
}

void RegistryPrivate::slotStateChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
#ifdef ATSPI_DEBUG
    qDebug() << "State changed: " << state << detail1 << detail2 << args.variant() << reference.path.path();
#endif

    // have a cache or create a qml accessible?
    // let it be spoken

    // find accessible, emit signal

    if ((state == QLatin1String("focused")) && (detail1 == 1)) {
//        AccessibleObject accessible = accessibleFromPath(reference.service, QDBusContext::message().path());
//        emit signalFocusChanged(QSharedPointer<AccessibleObject>(accessible));
    }
}

void RegistryPrivate::slotChildrenChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
#ifdef ATSPI_DEBUG
    qDebug() << "Children changed: " << state << detail1 << detail2 << args.variant() << reference.path.path();
#endif
}

void RegistryPrivate::slotPropertyChange(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
#ifdef ATSPI_DEBUG
    qDebug() << "Children changed: " << state << detail1 << detail2 << args.variant() << reference.path.path();
#endif
}

AccessibleObject RegistryPrivate::accessibleFromPath(const QString &service, const QString &path) const
{
     return AccessibleObject(bus, service, path);
}

}

