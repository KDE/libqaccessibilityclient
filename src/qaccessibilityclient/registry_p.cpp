/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "registry_p.h"
#include "registry.h"
#include "qaccessibilityclient_debug.h"

#include <QDBusMessage>
#include <QDBusArgument>
#include <QDBusReply>
#include <QDBusPendingCall>
#include <QDBusArgument>
#include <QDBusMetaType>

#include <QDBusMessage>
#include <QStringList>
#include <qurl.h>

#include "atspi/atspi-constants.h"

#include <QString>

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

QString RegistryPrivate::ACCESSIBLE_OBJECT_SCHEME_STRING = QLatin1String("accessibleobject");

RegistryPrivate::RegistryPrivate(Registry *qq)
    :q(qq)
    , m_subscriptions(Registry::NoEventListeners)
{
    qDBusRegisterMetaType<QVector<quint32> >();

    connect(&conn, SIGNAL(connectionFetched()), this, SLOT(connectionFetched()));
    connect(&m_actionMapper, SIGNAL(mappedString(QString)), this, SLOT(actionTriggered(QString)));
    init();
}

RegistryPrivate::~RegistryPrivate()
{
    delete m_cache;
}

void RegistryPrivate::init()
{
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_CACHE)] = AccessibleObject::CacheInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_ACCESSIBLE)] = AccessibleObject::AccessibleInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_ACTION)] = AccessibleObject::ActionInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_APPLICATION)] = AccessibleObject::ApplicationInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_COLLECTION)] = AccessibleObject::CollectionInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_COMPONENT)] = AccessibleObject::ComponentInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_DOCUMENT)] = AccessibleObject::DocumentInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EDITABLE_TEXT)] = AccessibleObject::EditableTextInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_KEYBOARD)] = AccessibleObject::EventKeyboardInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_MOUSE)] = AccessibleObject::EventMouseInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_OBJECT)] = AccessibleObject::EventObjectInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_HYPERLINK)] = AccessibleObject::HyperlinkInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_HYPERTEXT)] = AccessibleObject::HypertextInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_IMAGE)] = AccessibleObject::ImageInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_SELECTION)] = AccessibleObject::SelectionInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_TABLE)] = AccessibleObject::TableInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_TEXT)] = AccessibleObject::TextInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_VALUE)] = AccessibleObject::ValueInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_SOCKET)] = AccessibleObject::SocketInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_WINDOW)] = AccessibleObject::EventWindowInterface;
    interfaceHash[QLatin1String(ATSPI_DBUS_INTERFACE_EVENT_FOCUS)] = AccessibleObject::EventFocusInterface;
}

bool RegistryPrivate::isEnabled() const
{
    if (conn.status() != DBusConnection::Connected)
        return false;
    QDBusMessage message = QDBusMessage::createMethodCall(
                QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get"));
    message.setArguments(QVariantList() << QLatin1String("org.a11y.Status") << QLatin1String("IsEnabled"));
    QDBusReply<QVariant> reply  = QDBusConnection::sessionBus().call(message);
    if (!reply.isValid())
        return false;
    return reply.value().toBool();
}

void RegistryPrivate::setEnabled(bool enable)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
                QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Set"));
    message.setArguments(QVariantList() << QLatin1String("org.a11y.Status") << QLatin1String("IsEnabled") << QVariant::fromValue(QDBusVariant(enable)));
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not set org.a11y.Status.isEnabled." << reply.errorName() << reply.errorMessage();
    }
}

bool RegistryPrivate::isScreenReaderEnabled() const
{
    if (conn.status() != DBusConnection::Connected)
        return false;
    QDBusMessage message = QDBusMessage::createMethodCall(
                QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get"));
    message.setArguments(QVariantList() << QLatin1String("org.a11y.Status") << QLatin1String("ScreenReaderEnabled"));
    QDBusReply<QVariant> reply  = QDBusConnection::sessionBus().call(message);
    if (!reply.isValid())
        return false;
    return reply.value().toBool();
}

void RegistryPrivate::setScreenReaderEnabled(bool enable)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
                QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Set"));
    message.setArguments(QVariantList() << QLatin1String("org.a11y.Status") << QLatin1String("ScreenReaderEnabled") << QVariant::fromValue(QDBusVariant(enable)));
    QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not set org.a11y.Status.ScreenReaderEnabled." << reply.errorName() << reply.errorMessage();
    }
}

AccessibleObject RegistryPrivate::fromUrl(const QUrl &url) const
{
    Q_ASSERT(url.scheme() == ACCESSIBLE_OBJECT_SCHEME_STRING);
    if (url.scheme() != ACCESSIBLE_OBJECT_SCHEME_STRING)
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
        bool connected = session.connect(QLatin1String("org.a11y.Bus"), QLatin1String("/org/a11y/bus"), QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("PropertiesChanged"), this, SLOT(a11yConnectionChanged(QString,QVariantMap,QStringList)));
        if (!connected)
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << Q_FUNC_INFO << "Failed to connect with signal org.a11y.Status.PropertiesChanged on org.a11y.Bus";
    }

    if (m_pendingSubscriptions > 0) {
        subscribeEventListeners(m_pendingSubscriptions);
        m_pendingSubscriptions = {};
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
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to Window event(s)."
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
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility ChildrenChanged events.";
    }

    if (removedListeners.testFlag(Registry::VisibleDataChanged)) {
        removedSubscriptions << QLatin1String("object:visibledata-changed");
    } else if (addedListeners.testFlag(Registry::VisibleDataChanged)) {
        newSubscriptions << QLatin1String("object:visibledata-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("VisibleDataChanged"),
                    this, SLOT(slotVisibleDataChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility VisibleDataChanged events.";
    }

    if (removedListeners.testFlag(Registry::SelectionChanged)) {
        removedSubscriptions << QLatin1String("object:selection-changed");
    } else if (addedListeners.testFlag(Registry::SelectionChanged)) {
        newSubscriptions << QLatin1String("object:selection-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("SelectionChanged"),
                    this, SLOT(slotSelectionChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility SelectionChanged events.";
    }


    if (removedListeners.testFlag(Registry::ModelChanged)) {
        removedSubscriptions << QLatin1String("object:model-changed");
    } else if (addedListeners.testFlag(Registry::ModelChanged)) {
        newSubscriptions << QLatin1String("object:model-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("ModelChanged"),
                    this, SLOT(slotModelChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility ModelChanged events.";
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
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility Focus events.";
    }

    if (removedListeners.testFlag(Registry::TextChanged)) {
        removedSubscriptions << QLatin1String("object:text-changed");
    } else if (addedListeners.testFlag(Registry::TextChanged)) {
        newSubscriptions << QLatin1String("object:text-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("TextChanged"),
                    this, SLOT(slotTextChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility TextChanged events.";
    }

    if (removedListeners.testFlag(Registry::TextCaretMoved)) {
        removedSubscriptions << QLatin1String("object:text-caret-moved");
    } else if (addedListeners.testFlag(Registry::TextCaretMoved)) {
        newSubscriptions << QLatin1String("object:text-caret-moved");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("TextCaretMoved"),
                    this, SLOT(slotTextCaretMoved(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility TextCaretMoved events.";
    }

    if (removedListeners.testFlag(Registry::TextSelectionChanged)) {
        removedSubscriptions << QLatin1String("object:text-selection-changed");
    } else if (addedListeners.testFlag(Registry::TextSelectionChanged)) {
        newSubscriptions << QLatin1String("object:text-selection-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("TextSelectionChanged"),
                    this, SLOT(slotTextSelectionChanged(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility TextSelectionChanged events.";
    }

    if (removedListeners.testFlag(Registry::PropertyChanged)) {
        removedSubscriptions << QLatin1String("object:property-change");
    } else if (addedListeners.testFlag(Registry::PropertyChanged )) {
        newSubscriptions << QLatin1String("object:property-change");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("PropertyChange"),
                    this, SLOT(slotPropertyChange(QString,int,int,QDBusVariant,QAccessibleClient::QSpiObjectReference)));
        if (!success) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility PropertyChange events.";
    }

    for (const QString &subscription : std::as_const(newSubscriptions)) {
        QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.atspi.Registry"),
                                                        QLatin1String("/org/a11y/atspi/registry"),
                                                        QLatin1String("org.a11y.atspi.Registry"), QLatin1String("RegisterEvent"));
        m.setArguments(QVariantList() << subscription);

        QDBusPendingCall async = conn.connection().asyncCall(m);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(slotSubscribeEventListenerFinished(QDBusPendingCallWatcher*)));
    }

    for (const QString &subscription : std::as_const(removedSubscriptions)) {
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not subscribe to accessibility event: " << call->error().type() << call->error().message();
    }
    call->deleteLater();
}

void RegistryPrivate::a11yConnectionChanged(const QString &interface,const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    //qDebug() << Q_FUNC_INFO << "interface=" << interface << "changedProperties=" << changedProperties << "invalidatedProperties=" << invalidatedProperties;
    if (conn.status() != DBusConnection::Connected)
        return;
    if (interface == QLatin1String("org.a11y.Status")) {
        QVariantMap::ConstIterator IsEnabledIt = changedProperties.constFind(QLatin1String("IsEnabled"));
        if (IsEnabledIt != changedProperties.constEnd())
            Q_EMIT q->enabledChanged(IsEnabledIt.value().toBool());
        else if (invalidatedProperties.contains(QLatin1String("IsEnabled")))
            Q_EMIT q->enabledChanged(isEnabled());

        QVariantMap::ConstIterator ScreenReaderEnabledIt = changedProperties.constFind(QLatin1String("ScreenReaderEnabled"));
        if (ScreenReaderEnabledIt != changedProperties.constEnd())
            Q_EMIT q->screenReaderEnabledChanged(ScreenReaderEnabledIt.value().toBool());
        else if (invalidatedProperties.contains(QLatin1String("ScreenReaderEnabled")))
            Q_EMIT q->screenReaderEnabledChanged(isScreenReaderEnabled());
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

    if (ref.path.path() == object.d->path) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "WARNING: Accessible claims to be its own parent: " << object;
        return AccessibleObject();
    }

    if (ref.service.isEmpty() || ref.path.path().isEmpty())
        return AccessibleObject();

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
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Found old api returning uint in GetIndexInParent." << reply.error().message();
            return static_cast<int>(reply.value());
        }
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access index in parent." << reply.error().message();
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access child." << reply.error().message();
        return AccessibleObject();
    }
    const QSpiObjectReference child = reply.value();
    return AccessibleObject(const_cast<RegistryPrivate*>(this), child.service, child.path.path());
}

QList<AccessibleObject> RegistryPrivate::children(const AccessibleObject &object) const
{
    QList<AccessibleObject> accs;

    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetChildren"));

    QDBusReply<QSpiObjectReferenceList> reply = conn.connection().call(message, QDBus::Block, 500);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access children." << reply.error().message();
        return accs;
    }

    const QSpiObjectReferenceList children = reply.value();
    for (const QSpiObjectReference &child : children) {
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

QString RegistryPrivate::accessibleId(const AccessibleObject &object) const
{
    if (!object.isValid())
        return QString();
    return getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("AccessibleId")).toString();
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

AccessibleObject::Role RegistryPrivate::role(const AccessibleObject &object) const
{
    if (!object.isValid())
        return AccessibleObject::NoRole;

    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetRole"));

    QDBusReply<uint> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access role." << reply.error().message();
        return AccessibleObject::NoRole;
    }
    return atspiRoleToRole(static_cast<AtspiRole>(reply.value()));
}

AccessibleObject::Role RegistryPrivate::atspiRoleToRole(AtspiRole role)
{
    switch (role) {
    case ATSPI_ROLE_INVALID: return AccessibleObject::NoRole;
//    case ATSPI_ROLE_ACCELERATOR_LABEL: return AccessibleObject::;
//    case ATSPI_ROLE_ALERT: return AccessibleObject::;
//    case ATSPI_ROLE_ANIMATION: return AccessibleObject::;
//    case ATSPI_ROLE_ARROW: return AccessibleObject::;
//    case ATSPI_ROLE_CALENDAR: return AccessibleObject::;
//    case ATSPI_ROLE_CANVAS: return AccessibleObject::;
    case ATSPI_ROLE_CHECK_BOX: return AccessibleObject::CheckBox;
    case ATSPI_ROLE_CHECK_MENU_ITEM: return AccessibleObject::CheckableMenuItem;
//    case ATSPI_ROLE_COLOR_CHOOSER: return AccessibleObject::;
    case ATSPI_ROLE_COLUMN_HEADER: return AccessibleObject::ColumnHeader;
    case ATSPI_ROLE_COMBO_BOX: return AccessibleObject::ComboBox;
//    case ATSPI_ROLE_DATE_EDITOR: return AccessibleObject::;
//    case ATSPI_ROLE_DESKTOP_ICON: return AccessibleObject::;
    case ATSPI_ROLE_DESKTOP_FRAME: return AccessibleObject::DesktopFrame;
//    case ATSPI_ROLE_DIAL: return AccessibleObject::;
    case ATSPI_ROLE_DIALOG: return AccessibleObject::Dialog;
//    case ATSPI_ROLE_DIRECTORY_PANE: return AccessibleObject::;
//    case ATSPI_ROLE_DRAWING_AREA: return AccessibleObject::;
//    case ATSPI_ROLE_FILE_CHOOSER: return AccessibleObject::;
    case ATSPI_ROLE_FILLER: return AccessibleObject::Filler;
//    case ATSPI_ROLE_FOCUS_TRAVERSABLE: return AccessibleObject::;
//    case ATSPI_ROLE_FONT_CHOOSER: return AccessibleObject::;
    case ATSPI_ROLE_FRAME: return AccessibleObject::Frame;
//    case ATSPI_ROLE_GLASS_PANE: return AccessibleObject::;
//    case ATSPI_ROLE_HTML_CONTAINER: return AccessibleObject::;
    case ATSPI_ROLE_ICON: return AccessibleObject::Icon;
//    case ATSPI_ROLE_IMAGE: return AccessibleObject::;
//    case ATSPI_ROLE_INTERNAL_FRAME: return AccessibleObject::;
    case ATSPI_ROLE_LABEL: return AccessibleObject::Label;
//    case ATSPI_ROLE_LAYERED_PANE: return AccessibleObject::;
    case ATSPI_ROLE_LIST: return AccessibleObject::ListView;
    case ATSPI_ROLE_LIST_ITEM: return AccessibleObject::ListItem;
    case ATSPI_ROLE_MENU: return AccessibleObject::Menu;
    case ATSPI_ROLE_MENU_BAR: return AccessibleObject::MenuBar;
    case ATSPI_ROLE_MENU_ITEM: return AccessibleObject::MenuItem;
//    case ATSPI_ROLE_OPTION_PANE: return AccessibleObject::;
    case ATSPI_ROLE_PAGE_TAB: return AccessibleObject::Tab;
    case ATSPI_ROLE_PAGE_TAB_LIST: return AccessibleObject::TabContainer;
//    case ATSPI_ROLE_PANEL: return AccessibleObject::;
    case ATSPI_ROLE_PASSWORD_TEXT: return AccessibleObject::PasswordText;
    case ATSPI_ROLE_POPUP_MENU: return AccessibleObject::PopupMenu;
    case ATSPI_ROLE_PROGRESS_BAR: return AccessibleObject::ProgressBar;
    case ATSPI_ROLE_PUSH_BUTTON: return AccessibleObject::Button;
    case ATSPI_ROLE_RADIO_BUTTON: return AccessibleObject::RadioButton;
    case ATSPI_ROLE_RADIO_MENU_ITEM: return AccessibleObject::RadioMenuItem;
//    case ATSPI_ROLE_ROOT_PANE: return AccessibleObject::;
    case ATSPI_ROLE_ROW_HEADER: return AccessibleObject::RowHeader;
    case ATSPI_ROLE_SCROLL_BAR: return AccessibleObject::ScrollBar;
    case ATSPI_ROLE_SCROLL_PANE: return AccessibleObject::ScrollArea;
    case ATSPI_ROLE_SEPARATOR: return AccessibleObject::Separator;
    case ATSPI_ROLE_SLIDER: return AccessibleObject::Slider;
    case ATSPI_ROLE_SPIN_BUTTON: return AccessibleObject::SpinButton;
//    case ATSPI_ROLE_SPLIT_PANE: return AccessibleObject::;
    case ATSPI_ROLE_STATUS_BAR: return AccessibleObject::StatusBar;
    case ATSPI_ROLE_TABLE: return AccessibleObject::TableView;
    case ATSPI_ROLE_TABLE_CELL: return AccessibleObject::TableCell;
    case ATSPI_ROLE_TABLE_COLUMN_HEADER: return AccessibleObject::TableColumnHeader;
    case ATSPI_ROLE_TABLE_ROW_HEADER: return AccessibleObject::TableRowHeader;
//    case ATSPI_ROLE_TEAROFF_MENU_ITEM: return AccessibleObject::;
    case ATSPI_ROLE_TERMINAL: return AccessibleObject::Terminal;
    case ATSPI_ROLE_TEXT: return AccessibleObject::Text;
    case ATSPI_ROLE_TOGGLE_BUTTON: return AccessibleObject::ToggleButton;
    case ATSPI_ROLE_TOOL_BAR: return AccessibleObject::ToolBar;
    case ATSPI_ROLE_TOOL_TIP: return AccessibleObject::ToolTip;
    case ATSPI_ROLE_TREE: return AccessibleObject::TreeView;
    case ATSPI_ROLE_TREE_TABLE: return AccessibleObject::TreeView;
    case ATSPI_ROLE_UNKNOWN: return AccessibleObject::NoRole;
//    case ATSPI_ROLE_VIEWPORT: return AccessibleObject::;
    case ATSPI_ROLE_WINDOW: return AccessibleObject::Window;
//    case ATSPI_ROLE_EXTENDED: return AccessibleObject::;
//    case ATSPI_ROLE_HEADER: return AccessibleObject::;
//    case ATSPI_ROLE_FOOTER: return AccessibleObject::;
//    case ATSPI_ROLE_PARAGRAPH: return AccessibleObject::;
//    case ATSPI_ROLE_RULER: return AccessibleObject::;
//    case ATSPI_ROLE_APPLICATION: return AccessibleObject::;
//    case ATSPI_ROLE_AUTOCOMPLETE: return AccessibleObject::;
//    case ATSPI_ROLE_EDITBAR: return AccessibleObject::;
//    case ATSPI_ROLE_EMBEDDED: return AccessibleObject::;
//    case ATSPI_ROLE_ENTRY: return AccessibleObject::;
//    case ATSPI_ROLE_CHART: return AccessibleObject::;
//    case ATSPI_ROLE_CAPTION: return AccessibleObject::;
//    case ATSPI_ROLE_DOCUMENT_FRAME: return AccessibleObject::;
//    case ATSPI_ROLE_HEADING: return AccessibleObject::;
//    case ATSPI_ROLE_PAGE: return AccessibleObject::;
//    case ATSPI_ROLE_SECTION: return AccessibleObject::;
//    case ATSPI_ROLE_REDUNDANT_OBJECT: return AccessibleObject::;
//    case ATSPI_ROLE_FORM: return AccessibleObject::;
//    case ATSPI_ROLE_LINK: return AccessibleObject::;
//    case ATSPI_ROLE_INPUT_METHOD_WINDOW: return AccessibleObject::;
    case ATSPI_ROLE_TABLE_ROW: return AccessibleObject::TableRow;
    case ATSPI_ROLE_TREE_ITEM: return AccessibleObject::TreeItem;
//    case ATSPI_ROLE_DOCUMENT_SPREADSHEET: return AccessibleObject::;
//    case ATSPI_ROLE_DOCUMENT_PRESENTATION: return AccessibleObject::;
//    case ATSPI_ROLE_DOCUMENT_TEXT: return AccessibleObject::;
//    case ATSPI_ROLE_DOCUMENT_WEB: return AccessibleObject::;
//    case ATSPI_ROLE_DOCUMENT_EMAIL: return AccessibleObject::;
//    case ATSPI_ROLE_COMMENT: return AccessibleObject::;
//    case ATSPI_ROLE_LIST_BOX: return AccessibleObject::;
//    case ATSPI_ROLE_GROUPING: return AccessibleObject::;
//    case ATSPI_ROLE_IMAGE_MAP: return AccessibleObject::;
//    case ATSPI_ROLE_NOTIFICATION: return AccessibleObject::;
//    case ATSPI_ROLE_INFO_BAR: return AccessibleObject::;
//    case ATSPI_ROLE_LAST_DEFINED: return AccessibleObject::;
    }
    return AccessibleObject::NoRole;
}

QString RegistryPrivate::roleName(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetRoleName"));

    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access roleName." << reply.error().message();
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access localizedRoleName." << reply.error().message();\
        return QString();
    }
    return reply.value();
}

quint64 RegistryPrivate::state(const AccessibleObject &object) const
{
    if (m_cache) {
        quint64 cachedValue = m_cache->state(object);
        if (cachedValue != QAccessibleClient::ObjectCache::StateNotFound)
            return cachedValue;
    }

    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetState"));

    QDBusReply<QVector<quint32> > reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access state." << reply.error().message();
        return 0;
    }
    if (reply.value().size() < 2) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Did not receive expected reply.";
        return 0;
    }
    quint32 low = reply.value().at(0);
    quint32 high = reply.value().at(1);
    quint64 state = low + (static_cast<quint64>(high) << 32);

    if (m_cache) {
        m_cache->setState(object, state);
    }

    return state;
}

int RegistryPrivate::layer(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Component"), QLatin1String("GetLayer"));
    QDBusReply<uint> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access layer." << reply.error().message();
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access mdiZOrder." << reply.error().message();
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access alpha." << reply.error().message();
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not get extents." << reply.error().message();
        return QRect();
    }

    return QRect( reply.value() );
}

QRect RegistryPrivate::characterRect(const AccessibleObject &object, int offset) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"),
                    QLatin1String("GetCharacterExtents"));

    QVariantList args;
    quint32 coords = ATSPI_COORD_TYPE_SCREEN;
    args << offset;
    args << coords;
    message.setArguments(args);


    QDBusReply< QRect > reply = conn.connection().call(message);
    if(!reply.isValid()){
        if (reply.error().type() == QDBusError::InvalidSignature) {
            QDBusMessage reply2 = conn.connection().call(message);
            if (reply2.signature() != QLatin1String("iiii")) {
                qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not get Character Extents. " << reply.error().message();
                return QRect();
            }
            QList<QVariant> args = reply2.arguments();
            QRect rect(args.at(0).toInt(), args.at(1).toInt(), args.at(2).toInt(), args.at(3).toInt());
            return rect;
        }
    }

    return reply.value();
}

AccessibleObject::Interfaces RegistryPrivate::supportedInterfaces(const AccessibleObject &object) const
{
    if (m_cache) {
        AccessibleObject::Interfaces interfaces = m_cache->interfaces(object);
        if (!(interfaces & AccessibleObject::InvalidInterface))
            return interfaces;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"),
                    QLatin1String("GetInterfaces"));

    QDBusReply<QStringList > reply = conn.connection().call(message);
    if(!reply.isValid()){
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not get Interfaces. " << reply.error().message();
        return AccessibleObject::NoInterface;
    }

    AccessibleObject::Interfaces interfaces = AccessibleObject::NoInterface;
    const auto values{reply.value()};
    for (const QString &interface : values){
        interfaces |= interfaceHash[interface];
    }

    if (m_cache) {
        m_cache->setInterfaces(object, interfaces);
    }

    return interfaces;
}

int RegistryPrivate::caretOffset(const AccessibleObject &object) const
{
    QVariant offset= getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("CaretOffset"));
    if (offset.isNull()) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not get caret offset";
    return offset.toInt();
}

int RegistryPrivate::characterCount(const AccessibleObject &object) const
{
    QVariant count = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("CharacterCount"));
    if (count.isNull()) qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not get character count";
    return count.toInt();
}

QList< QPair<int,int> > RegistryPrivate::textSelections(const AccessibleObject &object) const
{
    QList< QPair<int,int> > result;
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("GetNSelections"));
    QDBusReply<int> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access GetNSelections." << reply.error().message();
        return result;
    }
    int count = reply.value();
    for(int i = 0; i < count; ++i) {
        QDBusMessage m = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("GetSelection"));
        m.setArguments(QVariantList() << i);
        m = conn.connection().call(m);
        QList<QVariant> args = m.arguments();
        if (args.count() < 2) {
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Invalid number of arguments. Expected=2 Actual=" << args.count();
            continue;
        }
        int startOffset = args[0].toInt();
        int endOffset = args[1].toInt();
        if (startOffset > endOffset)
            qSwap(startOffset, endOffset);
        result.append(qMakePair(startOffset, endOffset));
    }
    return result;
}

void RegistryPrivate::setTextSelections(const AccessibleObject &object, const QList< QPair<int,int> > &selections)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("GetNSelections"));
    QDBusReply<int> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access GetNSelections." << reply.error().message();
        return;
    }
    int count = reply.value();
    int setSel = qMin(selections.count(), count);
    for(int i = 0; i < setSel; ++i) {
        Q_ASSERT(i < selections.count());
        QPair<int,int> p = selections[i];
        QDBusMessage m = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("SetSelection"));
        m.setArguments(QVariantList() << i << p.first << p.second);
        QDBusReply<bool> r = conn.connection().call(m);
        if (!r.isValid()) {
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Failed call text.SetSelection." << r.error().message();
            continue;
        }
    }
    int removeSel = qMax(0, count - selections.count());
    for(int i = 0, k = selections.count(); i < removeSel; ++i, ++k) {
        QDBusMessage m = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("RemoveSelection"));
        m.setArguments(QVariantList() << k);
        QDBusReply<bool> r = conn.connection().call(m);
        if (!r.isValid()) {
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Failed call text.RemoveSelection." << r.error().message();
            continue;
        }
    }
    int addSel = qMax(0, selections.count() - count);
    for(int i = 0, k = count; i < addSel; ++i, ++k) {
        Q_ASSERT(k < selections.count());
        QPair<int,int> p = selections[k];
        QDBusMessage m = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("AddSelection"));
        m.setArguments(QVariantList() << p.first << p.second);
        QDBusReply<bool> r = conn.connection().call(m);
        if (!r.isValid()) {
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Failed call text.AddSelection." << r.error().message();
            continue;
        }
    }
}

QString RegistryPrivate::text(const AccessibleObject &object, int startOffset, int endOffset) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("GetText"));
    message.setArguments(QVariantList() << startOffset << endOffset);
    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access text." << reply.error().message();
        return QString();
    }
    return reply.value();
}

QString RegistryPrivate::textWithBoundary(const AccessibleObject &object, int offset, AccessibleObject::TextBoundary boundary, int *startOffset, int *endOffset) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Text"), QLatin1String("GetTextAtOffset"));
    message.setArguments(QVariantList() << offset << static_cast<AtspiTextBoundaryType>(boundary));
    QDBusMessage reply = conn.connection().call(message);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.signature() != QLatin1String("sii")) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access text." << reply.errorMessage();
        if (startOffset)
            *startOffset = 0;
        if (endOffset)
            *endOffset = 0;
        return QString();
    }
    if (startOffset)
        *startOffset = reply.arguments().at(1).toInt();
    if (endOffset)
        *endOffset = reply.arguments().at(2).toInt();
    return reply.arguments().first().toString();;
}

bool RegistryPrivate::setText(const AccessibleObject &object, const QString &text)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.EditableText"), QLatin1String("SetTextContents"));
    message.setArguments(QVariantList() << text);
    QDBusReply<bool> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not set text." << reply.error().message();
        return false;
    }
    return reply.value();
}

bool RegistryPrivate::insertText(const AccessibleObject &object, const QString &text, int position, int length)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.EditableText"), QLatin1String("InsertText"));
    message.setArguments(QVariantList() << position << text << length);
    QDBusReply<bool> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not insert text." << reply.error().message();
        return false;
    }
    return reply.value();
}

bool RegistryPrivate::copyText(const AccessibleObject &object, int startPos, int endPos)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.EditableText"), QLatin1String("CopyText"));
    message.setArguments(QVariantList() << startPos << endPos);
    conn.connection().call(message);
    return true;
}

bool RegistryPrivate::cutText(const AccessibleObject &object, int startPos, int endPos)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.EditableText"), QLatin1String("CutText"));
    message.setArguments(QVariantList() << startPos << endPos);
    QDBusReply<bool> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not cut text." << reply.error().message();
        return false;
    }
    return reply.value();
}

bool RegistryPrivate::deleteText(const AccessibleObject &object, int startPos, int endPos)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.EditableText"), QLatin1String("DeleteText"));
    message.setArguments(QVariantList() << startPos << endPos);
    QDBusReply<bool> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not delete text." << reply.error().message();
        return false;
    }
    return reply.value();
}

bool RegistryPrivate::pasteText(const AccessibleObject &object, int position)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.EditableText"), QLatin1String("PasteText"));
    message.setArguments(QVariantList() << position);
    QDBusReply<bool> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not paste text." << reply.error().message();
        return false;
    }
    return reply.value();
}

AccessibleObject RegistryPrivate::application(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetApplication"));
    QDBusReply<QSpiObjectReference> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access application." << reply.error().message();
        return AccessibleObject();
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
    // some apps misbehave and claim to be the service, but on :1.0 we have the atspi service which doesn't reply anything sensible here
    if (object.d->service == QLatin1String(":1.0"))
        return QString();

    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Application"), QLatin1String("GetLocale"));

    QVariantList args;
    args.append(lctype);
    message.setArguments(args);

    QDBusReply<QString> reply = conn.connection().call(message, QDBus::Block, 500);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access appLocale." << reply.error().message();
        return QString();
    }
    return reply.value();
}

QString RegistryPrivate::appBusAddress(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Application"), QLatin1String("GetApplicationBusAddress"));
    QDBusReply<QString> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << Q_FUNC_INFO << "Could not access application bus address. Error: " << reply.error().message() << " in response to: " << message;
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

bool RegistryPrivate::setCurrentValue(const AccessibleObject &object, double value)
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Value"), QLatin1String("SetCurrentValue"));

    QVariantList arguments;
    arguments << QLatin1String("org.a11y.atspi.Value") <<  QLatin1String("CurrentValue");
    arguments << QVariant::fromValue(QDBusVariant(value));
    message.setArguments(arguments);

    QDBusReply<bool> reply = conn.connection().call(message);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not set text." << reply.error().message();
        return false;
    }
    return reply.value();
}

QList<AccessibleObject> RegistryPrivate::selection(const AccessibleObject &object) const
{
    QList<AccessibleObject> result;
    int count = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Selection"), QLatin1String("CurrentValue")).toInt();
    for(int i = 0; i < count; ++i) {
        QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Selection"), QLatin1String("GetSelectedChild"));
        QDBusReply<QSpiObjectReference> reply = conn.connection().call(message);
        if (!reply.isValid()) {
            qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access selection." << reply.error().message();
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access imageDescription." << reply.error().message();
        return QString();
    }
    return reply.value();
}

QString RegistryPrivate::imageLocale(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Image"), QLatin1String("ImageLocale"));
    QDBusReply<QString> reply = conn.connection().call(message, QDBus::Block, 500);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access imageLocale." << reply.error().message();
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
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access imageRect." << reply.error().message();
        return QRect();
    }
    return QRect( reply.value() );
}

QVector< QSharedPointer<QAction> > RegistryPrivate::actions(const AccessibleObject &object)
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Action"), QLatin1String("GetActions"));

    QDBusReply<QSpiActionArray> reply = conn.connection().call(message, QDBus::Block, 500);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not access actions." << reply.error().message();
        return QVector< QSharedPointer<QAction> >();
    }

    QSpiActionArray actionArray = reply.value();
    QVector< QSharedPointer<QAction> > list;
    for(int i = 0; i < actionArray.count(); ++i) {
        const QSpiAction &a = actionArray[i];
        QAction *action = new QAction();
        QString id = QString(QLatin1String("%1;%2;%3")).arg(object.d->service).arg(object.d->path).arg(i);
        action->setObjectName(id);
        action->setText(a.name);
        action->setWhatsThis(a.description);
        QKeySequence shortcut(a.keyBinding);
        action->setShortcut(shortcut);
        m_actionMapper.setMapping(action, id);
        connect(action, SIGNAL(triggered()), &m_actionMapper, SLOT(map()));
        list.append(QSharedPointer<QAction>(action));
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

    QDBusReply<bool> reply = conn.connection().call(message, QDBus::Block, 500);
    if (!reply.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Could not execute action=" << action << reply.error().message();
        return;
    }

    if (reply.value()) {
        qDebug() << "Successful executed action=" << action;
    } else {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Failed to execute action=" << action;
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
    QDBusMessage reply = conn.connection().call(message, QDBus::Block, 500);
    if (reply.arguments().isEmpty())
        return QVariant();

    QDBusVariant v = reply.arguments().at(0).value<QDBusVariant>();
    return v.variant();
}

AccessibleObject RegistryPrivate::accessibleFromPath(const QString &service, const QString &path) const
{
    return AccessibleObject(const_cast<RegistryPrivate*>(this), service, path);
}

AccessibleObject RegistryPrivate::accessibleFromReference(const QSpiObjectReference &reference) const
{
    return accessibleFromPath(reference.service, reference.path.path());
}

AccessibleObject RegistryPrivate::accessibleFromContext() const
{
    return accessibleFromPath(QDBusContext::message().service(), QDBusContext::message().path());
}

void RegistryPrivate::slotWindowCreate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &)
{
    Q_EMIT q->windowCreated(accessibleFromContext());
}

void RegistryPrivate::slotWindowDestroy(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowDestroyed(accessibleFromContext());
}

void RegistryPrivate::slotWindowClose(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowClosed(accessibleFromContext());
}

void RegistryPrivate::slotWindowReparent(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowReparented(accessibleFromContext());
}

void RegistryPrivate::slotWindowMinimize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowMinimized(accessibleFromContext());
}

void RegistryPrivate::slotWindowMaximize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowMaximized(accessibleFromContext());
}

void RegistryPrivate::slotWindowRestore(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowRestored(accessibleFromContext());
}

void RegistryPrivate::slotWindowActivate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowActivated(accessibleFromContext());
}

void RegistryPrivate::slotWindowDeactivate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowDeactivated(accessibleFromContext());
}

void RegistryPrivate::slotWindowDesktopCreate(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowDesktopCreated(accessibleFromContext());
}

void RegistryPrivate::slotWindowDesktopDestroy(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowDesktopDestroyed(accessibleFromContext());
}

void RegistryPrivate::slotWindowRaise(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowRaised(accessibleFromContext());
}

void RegistryPrivate::slotWindowLower(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowLowered(accessibleFromContext());
}

void RegistryPrivate::slotWindowMove(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowMoved(accessibleFromContext());
}

void RegistryPrivate::slotWindowResize(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowResized(accessibleFromContext());
}

void RegistryPrivate::slotWindowShade(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowShaded(accessibleFromContext());
}

void RegistryPrivate::slotWindowUnshade(const QString &state, int detail1, int detail2, const QDBusVariant &/*args*/, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->windowUnshaded(accessibleFromContext());
}

void RegistryPrivate::slotPropertyChange(const QString &property, int detail1, int detail2, const QDBusVariant &args, const QSpiObjectReference &reference)
{
#ifdef ATSPI_DEBUG
    qDebug() << Q_FUNC_INFO << property << detail1 << detail2 << args.variant() << reference.path.path();
#endif
    if (property == QLatin1String("accessible-name")) {
        Q_EMIT q->accessibleNameChanged(accessibleFromContext());
    } else if (property == QLatin1String("accessible-description")) {
        Q_EMIT q->accessibleDescriptionChanged(accessibleFromContext());
    }
}

void RegistryPrivate::slotStateChanged(const QString &state, int detail1, int detail2, const QDBusVariant &object, const QSpiObjectReference &reference)
{
    //qDebug() << Q_FUNC_INFO << state << detail1 << detail2 << reference.service << reference.path.path() << QDBusContext::message();

    if (state == QLatin1String("defunct") && (detail1 == 1)) {
        QSpiObjectReference removed;
        removed.service = QDBusContext::message().service();
        removed.path = QDBusObjectPath(QDBusContext::message().path());
        removeAccessibleObject(removed);
        return;
    }

    AccessibleObject accessible = accessibleFromContext();
    if (m_cache) {
        m_cache->cleanState(accessible);
    }

    if (state == QLatin1String("focused") && (detail1 == 1) &&
            (q->subscribedEventListeners().testFlag(Registry::Focus))) {
        Q_EMIT q->focusChanged(accessible);
    }

    if (q->subscribedEventListeners().testFlag(Registry::StateChanged)) {
        Q_EMIT q->stateChanged(accessible, state, detail1 == 1);
    }
}

// void RegistryPrivate::slotLinkSelected(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
// {
//     Q_EMIT q->linkSelected(accessibleFromContext());
// }

bool RegistryPrivate::removeAccessibleObject(const QAccessibleClient::AccessibleObject &accessible)
{
    Q_ASSERT(accessible.isValid());
    if (m_cache) {
        const QString id = accessible.id();
        if (m_cache->remove(id)) {
            Q_EMIT q->removed(accessible);
        }
    } else {
        Q_EMIT q->removed(accessible);
    }
    if (accessible.d)
        accessible.d->setDefunct();
    return true;
}

bool RegistryPrivate::removeAccessibleObject(const QAccessibleClient::QSpiObjectReference &reference)
{
    QAccessibleClient::AccessibleObject acc = accessibleFromReference(reference);
    if (acc.isValid()) {
        if (removeAccessibleObject(acc))
            return true;
    }
    return false;
}

void RegistryPrivate::slotChildrenChanged(const QString &state, int detail1, int detail2, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
//    qDebug() << Q_FUNC_INFO << state << detail1 << detail2 << args.variant() << reference.path.path();
    QAccessibleClient::AccessibleObject parentAccessible = accessibleFromContext();
    if (!parentAccessible.isValid()) {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << Q_FUNC_INFO << "Children change with invalid parent." << reference.path.path();
        return;
    }

    int index = detail1;
    if (state == QLatin1String("add")) {
        Q_EMIT q->childAdded(parentAccessible, index);
    } else if (state == QLatin1String("remove")) {
        Q_EMIT q->childRemoved(parentAccessible, index);
    } else {
        qCWarning(LIBQACCESSIBILITYCLIENT_LOG) << "Invalid state in ChildrenChanged." << state;
    }
}

void RegistryPrivate::slotVisibleDataChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->visibleDataChanged(accessibleFromContext());
}

void RegistryPrivate::slotSelectionChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->selectionChanged(accessibleFromContext());
}

void RegistryPrivate::slotModelChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &args, const QAccessibleClient::QSpiObjectReference &reference)
{
    Q_EMIT q->modelChanged(accessibleFromContext());
}

void RegistryPrivate::slotTextCaretMoved(const QString &/*state*/, int detail1, int /*detail2*/, const QDBusVariant &/*args*/, const QSpiObjectReference &reference)
{
    Q_EMIT q->textCaretMoved(accessibleFromContext(), detail1);
}

void RegistryPrivate::slotTextSelectionChanged(const QString &/*state*/, int /*detail1*/, int /*detail2*/, const QDBusVariant &/*args*/, const QSpiObjectReference &reference)
{
    Q_EMIT q->textSelectionChanged(accessibleFromContext());
}

void RegistryPrivate::slotTextChanged(const QString &change, int start, int end, const QDBusVariant &textVariant, const QSpiObjectReference &reference)
{
    AccessibleObject object(accessibleFromContext());
    QString text = textVariant.variant().toString();

    if (change == QLatin1String("insert")) {
        Q_EMIT q->textInserted(object, text, start, end);
    } else if (change == QLatin1String("remove")) {
        Q_EMIT q->textRemoved(object, text, start, end);
    } else {
        Q_EMIT q->textChanged(object, text, start, end);
    }
}

#include "moc_registry_p.cpp"
