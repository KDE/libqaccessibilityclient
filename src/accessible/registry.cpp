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

Q_GLOBAL_STATIC(Registry, reg)

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

Registry *Registry::instance()
{
    return reg();
}

void RegistryPrivate::init()
{
    conn->connection().registerService(QLatin1String("org.kde.a11y.library"));

    bool success = conn->connection().connect(QString(), QString(), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Activate"), this,
                           SLOT(slotWindowActivated(QString,int,int,QDBusVariant,QSpiObjectReference)));
    Q_ASSERT(success); // for now make sure we connect, else nothing will work
    conn->connection().connect(QString(), QString(),
                           QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Create"), this,
                           SLOT(slotWindowCreated(QString,int,int,QDBusVariant,QSpiObjectReference)));
    conn->connection().connect(QString(), QString(),
                           QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Activate"), this,
                           SLOT(slotWindowActivated(QString,int,int,QDBusVariant,QSpiObjectReference)));

    conn->connection().connect(QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("StateChanged"), this,
                                  SLOT(slotStateChanged(QString, int, int, QDBusVariant, QSpiObjectReference)));
    conn->connection().connect(QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("ChildrenChanged"), this,
                                  SLOT(slotChildrenChanged(QString, int, int, QDBusVariant, QSpiObjectReference)));
    conn->connection().connect(QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("PropertyChanged"), this,
                                  SLOT(slotPropertyChange(QString, int, int, QDBusVariant, QSpiObjectReference)));

    QStringList subscriptions;

    subscriptions << QLatin1String("object:children-changed")
                  << QLatin1String("object:property-change:accessiblename")
                  << QLatin1String("object:state-changed")
                  << QLatin1String("object:bounds-changed")
                  << QLatin1String("object:visibledata-changed")
                  << QLatin1String("object:state-changed")
                  << QLatin1String("object:selection-changed")
    << QLatin1String("object:")
    << QLatin1String("focus:")
    << QLatin1String("window:");

// accerciser
//     (u':1.7', u'Object:StateChanged:'),
//     (u':1.7', u'Object:BoundsChanged:'),
//     (u':1.7', u'Object:VisibleDataChanged:'),
//     (u':1.7', u'Object:StateChanged:'),
// orca
//    [(u':1.8', u'Object:ChildrenChanged:'),
//     (u':1.8', u'Mouse:Button:'),
//     (u':1.8', u'Mouse:Abs:'),
//     (u':1.8', u'Object:StateChanged:Selected'),
//     (u':1.8', u'Object:StateChanged:Expanded'),
//     (u':1.8', u'Object:ValueChanged:'),
//     (u':1.8', u'Object:StateChanged:Focused'),
//     (u':1.8', u'Object:StateChanged:Active'),
//     (u':1.8', u'Window:Create:'),
//     (u':1.8', u'Object:TextAttributesChanged:'),
//     (u':1.8', u'Object:TextCaretMoved:'),
//     (u':1.8', u'Object:SelectionChanged:'),
//     (u':1.8', u'Focus::'),
//     (u':1.8', u'Object:ActiveDescendantChanged:'),
//     (u':1.8', u'Object:PropertyChange:AccessibleName'),
//     (u':1.8', u'Window:Activate:'),
//     (u':1.8', u'Window:Deactivate:'),
//     (u':1.8', u'Mouse:Button:'),
//     (u':1.8', u'Object:StateChanged:Indeterminate'),
//     (u':1.8', u'Object:LinkSelected:'),
//     (u':1.8', u'Object:TextChanged:Insert'),
//     (u':1.8', u'Object:PropertyChange:AccessibleValue'),
//     (u':1.8', u'Object:TextSelectionChanged:'),
//     (u':1.8', u'Object:StateChanged:Showing'),
//     (u':1.8', u'Object:TextChanged:Delete'),
//     (u':1.8', u'Object:StateChanged:Pressed'),
//     (u':1.8', u'Object:StateChanged:Checked'),
//     (u':1.8', u'Object:ChildrenChanged:Remove')]

    Q_FOREACH(const QString &subscription, subscriptions) {
        QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.atspi.Registry"),
                                                        QLatin1String("/org/a11y/atspi/registry"),
                                                        QLatin1String("org.a11y.atspi.Registry"), QLatin1String("RegisterEvent"));
        m.setArguments(QVariantList() << subscription);

        QDBusMessage reply = conn->connection().call(m);
        Q_UNUSED(reply)
        //qDebug() << "reply: " << reply << reply.type() << reply.arguments();
    }
    qDebug() << "AtspiWatcher done init";
}


//AccessibleObject AtSpiDBus::parent(AccessibleObjectPrivate *d)
//{
//    return ;
//}


QList<AccessibleObject> Registry::applications()
{
    return d->bus->topLevelAccessibles();
}

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

