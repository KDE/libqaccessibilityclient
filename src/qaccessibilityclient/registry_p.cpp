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

#include "registry_p.h"
#include "registry.h"

#include <qdbusmessage.h>
#include <qdbusargument.h>
#include <qdbusreply.h>
#include <qdbuspendingcall.h>
#include <qdbusinterface.h>
#include <qdbusargument.h>

#include <qdebug.h>
#include <qdbusmessage.h>
#include <qstringlist.h>
#include <qurl.h>

#include "atspi/atspi-constants.h"
#include "atspi/qt-atspi.h"
#include "atspi/dbusconnection.h"

#include <qstring.h>
#include <qhash.h>

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

//#define ATSPI_DEBUG

using namespace QAccessibleClient;

RegistryPrivate::RegistryPrivate(Registry *qq)
    :q(qq)
    , m_subscriptions(Registry::NoEventListeners)
    , m_cacheStrategy(new CacheWeakStrategy())
{
    connect(&conn, SIGNAL(connectionFetched()), this, SLOT(connectionFetched()));
    connect(&conn, SIGNAL(enabledChanged(bool)), q, SIGNAL(enabledChanged(bool)));
    connect(&m_actionMapper, SIGNAL(mapped(QString)), this, SLOT(actionTriggered(QString)));
    init();
}

void RegistryPrivate::init()
{
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_CACHE)] = AccessibleObject::Cache;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_ACCESSIBLE)] = AccessibleObject::Accessible;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_ACTION)] = AccessibleObject::Action;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_APPLICATION)] = AccessibleObject::Application;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_COLLECTION)] = AccessibleObject::Collection;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_COMPONENT)] = AccessibleObject::Component;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_DOCUMENT)] = AccessibleObject::Document;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EDITABLE_TEXT)] = AccessibleObject::Text;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_KEYBOARD)] = AccessibleObject::EventKeyboard;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_MOUSE)] = AccessibleObject::EventMouse;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT)] = AccessibleObject::EventObject;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_HYPERLINK)] = AccessibleObject::Hyperlink;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_HYPERTEXT)] = AccessibleObject::Hypertext;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_IMAGE)] = AccessibleObject::Image;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_SELECTION)] = AccessibleObject::Selection;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_TABLE)] = AccessibleObject::Table;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_TEXT)] = AccessibleObject::Text;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_VALUE)] = AccessibleObject::Value;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_SOCKET)] = AccessibleObject::Socket;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_WINDOW)] = AccessibleObject::EventWindow;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_FOCUS)] = AccessibleObject::EventFocus;
}

bool RegistryPrivate::isEnabled() const
{
    if (conn.status() != DBusConnection::Connected)
        return false;

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected())
        return false;

    QDBusMessage message = QDBusMessage::createMethodCall(
                QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get"));
    message.setArguments(QVariantList() << QLatin1String("org.a11y.Status") << QLatin1String("IsEnabled"));
    QDBusReply<QVariant> reply  = c.call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not get org.a11y.Status.isEnabled." << reply.error().message();
        return false;
    }

    bool enabled = qdbus_cast< QVariant >(reply).toBool();
    return enabled;
}

void RegistryPrivate::setEnabled(bool enable)
{
    if (conn.status() != DBusConnection::Connected)
        return;

    QDBusConnection c = QDBusConnection::sessionBus();
    if (!c.isConnected())
        return;

    QDBusMessage message = QDBusMessage::createMethodCall(
                QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Set"));
    message.setArguments(QVariantList() << QLatin1String("org.a11y.Status") << QLatin1String("IsEnabled") << QVariant::fromValue(QDBusVariant(enable)));
    QDBusMessage reply = c.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Could not set org.a11y.Status.isEnabled." << reply.errorName() << reply.errorMessage();
    }
}

QUrl RegistryPrivate::url(const AccessibleObject &object) const
{
    QUrl u;
    u.setScheme(QLatin1String("AccessibleObject"));
    u.setPath(object.d->path);
    u.setFragment(object.d->service);
    return u;
}

AccessibleObject RegistryPrivate::fromUrl(const QUrl &url) const
{
    Q_ASSERT(url.scheme() == QLatin1String("AccessibleObject"));
    if (url.scheme() != QLatin1String("AccessibleObject"))
        return AccessibleObject();
    QString path = url.path();
    QString service = url.fragment();
    return accessibleFromPath(service, path);
}

void RegistryPrivate::connectionFetched()
{
    Q_ASSERT(conn.status() == DBusConnection::Connected);

    QDBusConnection session = QDBusConnection::sessionBus();
    if (session.isConnected()) {
        bool connected = session.connect(QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus"), QLatin1String("PropertiesChanged"), this, SLOT(a11yConnectionChanged(QString,QVariantMap,QStringList)));
        if (!connected)
            qWarning() << Q_FUNC_INFO << "Failed to connect with signal org.a11y.Status.PropertiesChanged on org.a11y.Bus";
    }

    if (m_pendingSubscriptions > 0) {
        subscribeEventListeners(m_pendingSubscriptions);
        m_pendingSubscriptions = 0;
    }
}

void RegistryPrivate::subscribeEventListeners(const Registry::EventListeners &listeners)
{
    if (conn.isFetchingConnection()) {
        m_pendingSubscriptions = listeners;
        return;
    }

    Registry::EventListeners addedListeners = listeners & ~m_subscriptions;
    Registry::EventListeners removedListeners = m_subscriptions & ~listeners;

    QStringList newSubscriptions;
    QStringList removedSubscriptions;

    if (removedListeners.testFlag(Registry::Window)) {
        removedSubscriptions << QLatin1String("window:");
    } else if (addedListeners.testFlag(Registry::Window)) {
        // subscribe all window events
        newSubscriptions << QLatin1String("window:");

        bool created = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Create"),
                    this, SLOT(slotWindowCreate(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool destroyed = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Destroy"),
                    this, SLOT(slotWindowDestroy(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));

        bool closed = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Close"),
                    this, SLOT(slotWindowClose(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool reparented = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Reparent"),
                    this, SLOT(slotWindowReparent(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));

        bool minimized = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Minimize"),
                    this, SLOT(slotWindowMinimize(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool maximized = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Maximize"),
                    this, SLOT(slotWindowMaximize(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool restored = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Restore"),
                    this, SLOT(slotWindowRestore(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));

        bool activated = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Activate"),
                    this, SLOT(slotWindowActivate(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool deactivated = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Deactivate"),
                    this, SLOT(slotWindowDeactivate(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));

        bool desktopCreated = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("DesktopCreate"),
                    this, SLOT(slotWindowDesktopCreate(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool desktopDestroyed = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("DesktopDestroy"),
                    this, SLOT(slotWindowDesktopDestroy(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool raised = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Raise"),
                    this, SLOT(slotWindowRaise(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool lowered = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Lower"),
                    this, SLOT(slotWindowLower(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool moved = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Move"),
                    this, SLOT(slotWindowMove(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool resized = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Resize"),
                    this, SLOT(slotWindowResize(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool shaded = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Shade"),
                    this, SLOT(slotWindowShade(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        bool unshaded = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Unshade"),
                    this, SLOT(slotWindowUnshade(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));

        if (!created || !destroyed || !closed || !reparented || !minimized || !maximized || !restored || 
            !activated || !deactivated || !desktopCreated || !desktopDestroyed ||
            !raised || !lowered || !moved || !resized || !shaded || !unshaded
        ) {
            qWarning() << "Could not subscribe to Window event(s)."
                       << "created:" << created << "destroyed:" << destroyed
                       << "closed:" << closed << "reparented:" << reparented
                       << "minimized:" << minimized << "maximized:" << maximized << "restored:" << restored
                       << "activated:" << activated << "deactivated:" << deactivated
                       << "desktopCreated:" << desktopCreated << "desktopDestroyed:" << desktopDestroyed
                       << "raised:" << raised << "lowered:" << lowered
                       << "moved:" << moved << "resized:" << resized
                       << "shaded:" << shaded << "unshaded:" << unshaded
                       ;
        }
    }

    if (removedListeners.testFlag(Registry::ChildrenChanged)) {
        removedSubscriptions << QLatin1String("object:children-changed");
    } else if (addedListeners.testFlag(Registry::ChildrenChanged)) {
        newSubscriptions << QLatin1String("object:children-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("ChildrenChanged"),
                    this, SLOT(slotChildrenChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility ChildrenChanged events.";
    }

    if (removedListeners.testFlag(Registry::VisibleDataChanged)) {
        removedSubscriptions << QLatin1String("object:visibledata-changed");
    } else if (addedListeners.testFlag(Registry::VisibleDataChanged)) {
        newSubscriptions << QLatin1String("object:visibledata-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("VisibleDataChanged"),
                    this, SLOT(slotVisibleDataChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility VisibleDataChanged events.";
    }

    if (removedListeners.testFlag(Registry::SelectionChanged)) {
        removedSubscriptions << QLatin1String("object:selection-changed");
    } else if (addedListeners.testFlag(Registry::SelectionChanged)) {
        newSubscriptions << QLatin1String("object:selection-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("SelectionChanged"),
                    this, SLOT(slotSelectionChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility SelectionChanged events.";
    }


    if (removedListeners.testFlag(Registry::ModelChanged)) {
        removedSubscriptions << QLatin1String("object:model-changed");
    } else if (addedListeners.testFlag(Registry::ModelChanged)) {
        newSubscriptions << QLatin1String("object:model-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("ModelChanged"),
                    this, SLOT(slotModelChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility ModelChanged events.";
    }

    // we need state-changed-focus for focus events
    if ((removedListeners.testFlag(Registry::StateChanged) || removedListeners.testFlag(Registry::Focus))
            && (!(addedListeners.testFlag(Registry::StateChanged) || addedListeners.testFlag(Registry::Focus)))) {
        removedSubscriptions << QLatin1String("object:state-changed");
    } else if (addedListeners.testFlag(Registry::StateChanged) || addedListeners.testFlag(Registry::Focus)) {
        if (listeners.testFlag(Registry::Focus)) newSubscriptions << QLatin1String("focus:");
        newSubscriptions << QLatin1String("object:state-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("StateChanged"),
                    this, SLOT(slotStateChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility Focus events.";
    }

    if (removedListeners.testFlag(Registry::TextChanged)) {
        removedSubscriptions << QLatin1String("object:text-changed");
    } else if (addedListeners.testFlag(Registry::TextChanged)) {
        newSubscriptions << QLatin1String("object:text-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("TextChanged"),
                    this, SLOT(slotTextChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility TextChanged events.";
    }

    if (removedListeners.testFlag(Registry::TextCaretMoved)) {
        removedSubscriptions << QLatin1String("object:text-caret-moved");
    } else if (addedListeners.testFlag(Registry::TextCaretMoved)) {
        newSubscriptions << QLatin1String("object:text-caret-moved");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("TextCaretMoved"),
                    this, SLOT(slotTextCaretMoved(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility TextCaretMoved events.";
    }

    if (removedListeners.testFlag(Registry::TextSelectionChanged)) {
        removedSubscriptions << QLatin1String("object:text-selection-changed");
    } else if (addedListeners.testFlag(Registry::TextSelectionChanged)) {
        newSubscriptions << QLatin1String("object:text-selection-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("TextSelectionChanged"),
                    this, SLOT(slotTextSelectionChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility TextSelectionChanged events.";
    }

    if (removedListeners.testFlag(Registry::PropertyChanged)) {
        removedSubscriptions << QLatin1String("object:property-change");
    } else if (addedListeners.testFlag(Registry::PropertyChanged )) {
        newSubscriptions << QLatin1String("object:property-change");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("PropertyChange"),
                    this, SLOT(slotPropertyChange(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qWarning() << "Could not subscribe to accessibility PropertyChange events.";
    }

    Q_FOREACH(const QString &subscription, newSubscriptions) {
        QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.atspi.Registry"),
                                                        QLatin1String("/org/a11y/atspi/registry"),
                                                        QLatin1String("org.a11y.atspi.Registry"), QLatin1String("RegisterEvent"));
        m.setArguments(QVariantList() << subscription);

        QDBusPendingCall async = conn.connection().asyncCall(m);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(slotSubscribeEventListenerFinished(QDBusPendingCallWatcher*)));
    }

    Q_FOREACH(const QString &subscription, removedSubscriptions) {
        QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.atspi.Registry"),
                                                        QLatin1String("/org/a11y/atspi/registry"),
                                                        QLatin1String("org.a11y.atspi.Registry"), QLatin1String("DeregisterEvent"));
        m.setArguments(QVariantList() << subscription);
        conn.connection().asyncCall(m);
    }

    m_subscriptions = listeners;

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
// //     (u':1.8', u'Object:StateChanged:Showing'),
//     (u':1.8', u'Object:TextChanged:Delete'),
//     (u':1.8', u'Object:StateChanged:Pressed'),
//     (u':1.8', u'Object:StateChanged:Checked'),
//     (u':1.8', u'Object:ChildrenChanged:Remove')]

}

Registry::EventListeners RegistryPrivate::eventListeners() const
{
    return m_subscriptions | m_pendingSubscriptions;
}

void RegistryPrivate::slotSubscribeEventListenerFinished(QDBusPendingCallWatcher *call)
{
    if (call->isError()) {
        qWarning() << "Could not subscribe to accessibility event: " << call->error().type() << call->error().message();
    }
    call->deleteLater();
}

void RegistryPrivate::a11yConnectionChanged(const QString &interface,const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    //qDebug() << Q_FUNC_INFO << "interface=" << interface << "changedProperties=" << changedProperties << "invalidatedProperties=" << invalidatedProperties;
    if (conn.status() != DBusConnection::Connected)
        return;
    if (interface == QLatin1String("org.a11y.Status")) {
        bool enabled = false;
        QVariantMap::ConstIterator it = changedProperties.constFind(QLatin1String("IsEnabled"));
        if (it != changedProperties.constEnd()) {
            enabled = it.value().toBool();
        } else if (invalidatedProperties.contains(QLatin1String("IsEnabled"))) {
            enabled = isEnabled();
        } else {
            return;
        }
        emit q->enabledChanged(enabled);
    }
}

AccessibleObject RegistryPrivate::parentAccessible(const AccessibleObject &object) const
{
    QVariant parent = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("Parent"));
    if (!parent.isValid())
        return AccessibleObject();
    const QDBusArgument arg = parent.value<QDBusArgument>();
    QSpiObjectReference ref;
    arg >> ref;
    //if (ref.service.isEmpty() || ref.path.path().isEmpty()) return AccessibleObject();
    return AccessibleObject(const_cast<RegistryPrivate*>(this), ref.service, ref.path.path());
}

int RegistryPrivate::childCount(const AccessibleObject &object) const
{
    QVariant childCount = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("ChildCount"));
    return childCount.toInt();
}

int RegistryPrivate::indexInParent(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetIndexInParent"));

    QDBusReply<int> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        QDBusReply<uint> reply2 = conn.connection().call(message);
        if (reply2.isValid()) {
            qWarning() << "Found old api returning uint in GetIndexInParent." << reply.error().message();
            return static_cast<int>(reply.value());
        }
        qWarning() << "Could not access index in parent." << reply.error().message();
        return -1;
    }
    return reply.value();
}

AccessibleObject RegistryPrivate::child(const AccessibleObject &object, int index) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetChildAtIndex"));
    QVariantList args;
    args << index;
    message.setArguments(args);

    QDBusReply<QSpiObjectReference> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access child." << reply.error().message();
        return AccessibleObject(0, QString(), QString());
    }
    const QSpiObjectReference child = reply.value();
    return AccessibleObject(const_cast<RegistryPrivate*>(this), child.service, child.path.path());
}

QList<AccessibleObject> RegistryPrivate::children(const AccessibleObject &object) const
{
    QList<AccessibleObject> accs;

    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetChildren"));

    QDBusReply<QSpiObjectReferenceList> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access children." << reply.error().message();
        return accs;
    }

    const QSpiObjectReferenceList children = reply.value();
    Q_FOREACH(const QSpiObjectReference &child, children) {
        accs.append(AccessibleObject(const_cast<RegistryPrivate*>(this), child.service, child.path.path()));
    }

    return accs;
}

QList<AccessibleObject> RegistryPrivate::topLevelAccessibles() const
{
    QString service = QLatin1String("org.a11y.atspi.Registry");
    QString path = QLatin1String("/org/a11y/atspi/accessible/root");
    return children(AccessibleObject(const_cast<RegistryPrivate*>(this), service, path));
}

QString RegistryPrivate::name(const AccessibleObject &object) const
{
    if (!object.isValid())
        return QString();
    return getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("Name")).toString();
}

QString RegistryPrivate::description(const AccessibleObject &object) const
{
    if (!object.isValid())
        return QString();
    return getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("Description")).toString();
}

AtspiRole RegistryPrivate::role(const AccessibleObject &object) const
{
    if (!object.isValid())
        return ATSPI_ROLE_INVALID;

    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetRole"));

    QDBusReply<uint> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access role." << reply.error().message();
        return ATSPI_ROLE_INVALID;
    }
    return (AtspiRole) reply.value();
}

QString RegistryPrivate::roleName(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetRoleName"));

    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access roleName." << reply.error().message();
        return QString();
    }
    return reply.value();
}

QString RegistryPrivate::localizedRoleName(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetLocalizedRoleName"));

    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access localizedRoleName." << reply.error().message();\
        return QString();
    }
    return reply.value();
}

quint64 RegistryPrivate::state(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetState"));

    QDBusReply<QList<quint32> > reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access state." << reply.error().message();
        return 0;
    }
    int low = reply.value().at(0);
    int high = reply.value().at(1);
    quint64 state = low + ((quint64)high << 32);
    return state;
}

int RegistryPrivate::layer(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Component"), QLatin1String("GetLayer"));
    QDBusReply<uint> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access layer." << reply.error().message();
        return 1;
    }
    return reply.value();
}

int RegistryPrivate::mdiZOrder(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Component"), QLatin1String("GetMDIZOrder"));
    QDBusReply<short> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access mdiZOrder." << reply.error().message();
        return 0;
    }
    return reply.value();
}

double RegistryPrivate::alpha(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Component"), QLatin1String("GetAlpha"));
    QDBusReply<double> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access alpha." << reply.error().message();
        return 1.0;
    }
    return reply.value();
}

QRect RegistryPrivate::boundingRect(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Component"), QLatin1String("GetExtents") );
    QVariantList args;
    quint32 coords = ATSPI_COORD_TYPE_SCREEN;
    args << coords;
    message.setArguments(args);

    QDBusReply< QRect > reply = conn.connection().call(message);
    if(!reply.isValid()){
        qWarning() << "Could not get extents." << reply.error().message();
        return QRect();
    }

    return QRect( reply.value() );
}

QRect RegistryPrivate::characterRect(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"),
                    QLatin1String("GetCharacterExtents"));

    QVariantList args;
    quint32 coords = ATSPI_COORD_TYPE_SCREEN;
    args << caretOffset(object);
    args << coords;
    message.setArguments(args);

    QDBusReply< QRect > reply = conn.connection().call(message);
    if(!reply.isValid()){
        qWarning() << "Could not get Character Extents. " << reply.error().message();
        return QRect();
    }

    return reply.value();
}

AccessibleObject::Interfaces RegistryPrivate::supportedInterfaces(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"),
                    QLatin1String("GetInterfaces"));

    QDBusReply<QStringList > reply = conn.connection().call(message);
    if(!reply.isValid()){
        qWarning() << "Could not get Interfaces. " << reply.error().message();
        return AccessibleObject::NoInterface;
    }

    AccessibleObject::Interfaces interfaces = AccessibleObject::NoInterface;
    Q_FOREACH(const QString &interface, reply.value()){
        interfaces |= interfaceHash[interface];
    }

    return interfaces;
}

int RegistryPrivate::caretOffset(const AccessibleObject &object) const
{
    QVariant offset= getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("CaretOffset"));
    if (offset.isNull()) qWarning() << "Could not get caret offset";
    return offset.toInt();
}

AccessibleObject RegistryPrivate::application(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetApplication"));
    QDBusReply<QSpiObjectReference> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access application." << reply.error().message();
        return AccessibleObject(0, QString(), QString());
    }
    const QSpiObjectReference child = reply.value();
    return AccessibleObject(const_cast<RegistryPrivate*>(this), child.service, child.path.path());
}

QString RegistryPrivate::appToolkitName(const AccessibleObject &object) const
{
    QVariant v = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Application"), QLatin1String("ToolkitName"));
    return v.toString();
}

QString RegistryPrivate::appVersion(const AccessibleObject &object) const
{
    QVariant v = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Application"), QLatin1String("Version"));
    return v.toString();
}

int RegistryPrivate::appId(const AccessibleObject &object) const
{
    QVariant v = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Application"), QLatin1String("Id"));
    return v.toInt();
}

QString RegistryPrivate::appLocale(const AccessibleObject &object, uint lctype) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Application"), QLatin1String("GetLocale"));

    QVariantList args;
    args.append(lctype);
    message.setArguments(args);

    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access appLocale." << reply.error().message();
        return QString();
    }
    return reply.value();
}

QString RegistryPrivate::appBusAddress(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Application"), QLatin1String("GetApplicationBusAddress"));
    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access appBusAddress." << reply.error().message();
        return QString();
    }
    return reply.value();
}

double RegistryPrivate::minimumValue(const AccessibleObject &object) const
{
    QVariant v = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Value"), QLatin1String("MinimumValue"));
    return v.toDouble();
}

double RegistryPrivate::maximumValue(const AccessibleObject &object) const
{
    QVariant v = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Value"), QLatin1String("MaximumValue"));
    return v.toDouble();
}

double RegistryPrivate::minimumValueIncrement(const AccessibleObject &object) const
{
    QVariant v = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Value"), QLatin1String("MinimumIncrement"));
    return v.toDouble();
}

double RegistryPrivate::currentValue(const AccessibleObject &object) const
{
    QVariant v = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Value"), QLatin1String("CurrentValue"));
    return v.toDouble();
}

QList<AccessibleObject> RegistryPrivate::selection(const AccessibleObject &object) const
{
    QList<AccessibleObject> result;
    int count = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Selection"), QLatin1String("CurrentValue")).toInt();
    for(int i = 0; i < count; ++i) {
        QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Selection"), QLatin1String("GetSelectedChild"));
        QDBusReply<QSpiObjectReference> reply = conn.connection().call(message);
        if (!reply.isValid()) {
            qWarning() << "Could not access selection." << reply.error().message();
            return QList<AccessibleObject>();
        }
        const QSpiObjectReference ref = reply.value();
        result.append(AccessibleObject(const_cast<RegistryPrivate*>(this), ref.service, ref.path.path()));
    }
    return result;
}

QString RegistryPrivate::imageDescription(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Image"), QLatin1String("ImageDescription"));
    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access imageDescription." << reply.error().message();
        return QString();
    }
    return reply.value();
}

QString RegistryPrivate::imageLocale(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Image"), QLatin1String("ImageLocale"));
    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access imageLocale." << reply.error().message();
        return QString();
    }
    return reply.value();
}

QRect RegistryPrivate::imageRect(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Image"), QLatin1String("GetImageExtents"));
    QVariantList args;
    quint32 coords = ATSPI_COORD_TYPE_SCREEN;
    args << coords;
    message.setArguments(args);
    QDBusReply<QRect> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access imageRect." << reply.error().message();
        return QRect();
    }
    return QRect( reply.value() );
}

QList<QAction*> RegistryPrivate::actions(const AccessibleObject &object)
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Action"), QLatin1String("GetActions"));

    QDBusReply<QSpiActionArray> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access actions." << reply.error().message();
        return QList<QAction*>();
    }

    QSpiActionArray actionArray = reply.value();
    QList<QAction*> list;
    for(int i = 0; i < actionArray.count(); ++i) {
        const QSpiAction &a = actionArray[i];
        QAction *action = new QAction(this);
        QString id = QString(QLatin1String("%1;%2;%3")).arg(object.d->service).arg(object.d->path).arg(i);
        action->setObjectName(id);
        action->setText(a.name);
        action->setWhatsThis(a.description);
        QKeySequence shortcut(a.keyBinding);
        action->setShortcut(shortcut);
        m_actionMapper.setMapping(action, id);
        connect(action, SIGNAL(triggered()), &m_actionMapper, SLOT(map()));
        list.append(action);
    }
    return list;
}

void RegistryPrivate::actionTriggered(const QString &action)
{
    QStringList actionParts = action.split(QLatin1Char(';'));
    Q_ASSERT(actionParts.count() == 3);
    QString service = actionParts[0];
    QString path = actionParts[1];
    int index = actionParts[2].toInt();

    QDBusMessage message = QDBusMessage::createMethodCall (
                service, path, QLatin1String("org.a11y.atspi.Action"), QLatin1String("DoAction"));

    QVariantList args;
    args << index;
    message.setArguments(args);

    QDBusReply<bool> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not execute action=" << action << reply.error().message();
        return;
    }

    if (reply.value()) {
        qDebug() << "Successful executed action=" << action;
    } else {
        qWarning() << "Failed to execute action=" << action;
    }
}

QVariant RegistryPrivate::getProperty(const QString &service, const QString &path, const QString &interface, const QString &name) const
{
    QVariantList args;
    args.append(interface);
    args.append(name);

    QDBusMessage message = QDBusMessage::createMethodCall (
                service, path, QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get"));

    message.setArguments(args);
    QDBusMessage reply = conn.connection().call(message);
    if (reply.arguments().isEmpty())
        return QVariant();

    QDBusVariant v = reply.arguments().at(0).value<QDBusVariant>();
    return v.variant();
}

AccessibleObject RegistryPrivate::accessibleFromPath(const QString &service, const QString &path) const
{
    return AccessibleObject(const_cast<RegistryPrivate*>(this), service, path);
}

AccessibleObject RegistryPrivate::accessibleFromContext(const QSpiObjectReference &reference) const
{
    return AccessibleObject(const_cast<RegistryPrivate*>(this), reference.service, QDBusContext::message().path());
}

void RegistryPrivate::slotWindowCreate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowCreated(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowDestroy(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowDestroyed(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowClose(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowClosed(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowReparent(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowReparented(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowMinimize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowMinimized(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowMaximize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowMaximized(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowRestore(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowRestored(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowActivate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowActivated(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowDeactivate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowDeactivated(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowDesktopCreate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowDesktopCreated(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowDesktopDestroy(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowDesktopDestroyed(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowRaise(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowRaised(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowLower(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowLowered(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowMove(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowMoved(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowResize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowResized(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowShade(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowShaded(accessibleFromContext(reference));
}

void RegistryPrivate::slotWindowUnshade(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->windowUnshaded(accessibleFromContext(reference));
}

void RegistryPrivate::slotPropertyChange(const QString &property, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
#ifdef ATSPI_DEBUG
    qDebug() << Q_FUNC_INFO << property << detail1 << detail2 << args.variant() << reference.path.path();
#endif
    if (property == QLatin1String("accessible-name")) {
        emit q->accessibleNameChanged(accessibleFromContext(reference));
    } else if (property == QLatin1String("accessible-description")) {
        emit q->accessibleDescriptionChanged(accessibleFromContext(reference));
    }
}

void RegistryPrivate::slotStateChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
    //qDebug() << Q_FUNC_INFO << state << detail1 << detail2 << args.variant() << reference.path.path();
    if (state == QLatin1String("defunct")) {
        if (detail1 == 1) {
            removeAccessibleObject(reference);
            return;
        }
    } else if (state == QLatin1String("focused")) {
        if (detail1 == 1) {
            if (q->subscribedEventListeners().testFlag(Registry::Focus)) {
                QAccessibleClient::AccessibleObject accessible = accessibleFromContext(reference);
                emit q->focusChanged(accessible);
            }
        }
    }
    if (q->subscribedEventListeners().testFlag(Registry::StateChanged)) {
        if (m_cacheStrategy) {
            const QString id = QDBusContext::message().path() + reference.service;
            QSharedPointer<AccessibleObjectPrivate> p = m_cacheStrategy->get(id);
            if (p) {
                QAccessibleClient::AccessibleObject accessible(p);
                emit q->stateChanged(accessible, state, detail1, detail2);
            }
        } else {
            QAccessibleClient::AccessibleObject accessible = accessibleFromContext(reference);
            emit q->stateChanged(accessible, state, detail1, detail2);
        }
    }
}

// void RegistryPrivate::slotLinkSelected(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
// {
//     emit q->linkSelected(accessibleFromContext(reference));
// }

bool RegistryPrivate::removeAccessibleObject(const QAccessibleClient::AccessibleObject &accessible)
{
    Q_ASSERT(accessible.isValid());
    if (m_cacheStrategy) {
        const QString id = accessible.id();
        if (m_cacheStrategy->remove(id)) {
            emit q->removed(accessible);
        }
    } else {
        emit q->removed(accessible);
    }
    if (accessible.d)
        accessible.d->setDefunct();
    return true;
}

bool RegistryPrivate::removeAccessibleObject(const QAccessibleClient::QSpiObjectReference &reference)
{
    QAccessibleClient::AccessibleObject acc;
    if (m_cacheStrategy) {
        const QString id = reference.path.path() + reference.service;
        acc = m_cacheStrategy->get(id);
    } else {
        acc = accessibleFromContext(reference);
    }
    if (acc.isValid()) {
        if (removeAccessibleObject(acc))
            return true;
    }
    return false;
}

void RegistryPrivate::slotChildrenChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
    qDebug() << Q_FUNC_INFO << state << detail1 << detail2 << args.variant() << reference.path.path();
    QAccessibleClient::AccessibleObject parentAccessible = accessibleFromContext(reference);
    if (!parentAccessible.isValid()) {
        qWarning() << Q_FUNC_INFO << "Children change with invalid parent." << reference.path.path();
        return;
    }

    int index = detail1;
    if (state == QLatin1String("add")) {
        emit q->childAdded(parentAccessible, index);
    } else if (state == QLatin1String("remove")) {
        emit q->childRemoved(parentAccessible, index);
    } else {
        qWarning() << "Invalid state in ChildrenChanged." << state;
    }
}

void RegistryPrivate::slotVisibleDataChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->visibleDataChanged(accessibleFromContext(reference));
}

void RegistryPrivate::slotSelectionChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->selectionChanged(accessibleFromContext(reference));
}

void RegistryPrivate::slotModelChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
    emit q->modelChanged(accessibleFromContext(reference));
}

void RegistryPrivate::slotTextCaretMoved(const QString &/*state*/, int detail1, int /*detail2*/, const QDBusVariant &/*args*/, const QSpiObjectReference &reference)
{
    emit q->textCaretMoved(accessibleFromContext(reference), detail1);
}

void RegistryPrivate::slotTextSelectionChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &/*args*/, const QSpiObjectReference &reference)
{
    emit q->textSelectionChanged(accessibleFromContext(reference));
}

void RegistryPrivate::slotTextChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &/*args*/, const QSpiObjectReference &reference)
{
    emit q->textChanged(accessibleFromContext(reference));
}

#include "registry_p.moc"
