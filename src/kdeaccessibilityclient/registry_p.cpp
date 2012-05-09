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

#include <qdebug.h>
#include <qdbusmessage.h>
#include <qstringlist.h>

#include "atspi/atspi-constants.h"
#include "atspi/qt-atspi.h"
#include "atspi/dbusconnection.h"

#include "accessibleobject_p.h"

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

using namespace KAccessibleClient;

RegistryPrivate::RegistryPrivate(Registry *qq)
    :q(qq)
{
    connect(&m_actionMapper, SIGNAL(mapped(QString)), this, SLOT(actionTriggered(QString)));
}

void RegistryPrivate::init()
{
}

void RegistryPrivate::subscribeEventListeners(const Registry::EventListeners &listeners)
{
    QStringList subscriptions;

    if (listeners & Registry::Focus) {
        subscriptions << QLatin1String("focus:");
        subscriptions << QLatin1String("object:state-changed");
        bool success = conn.connection().connect(
                    QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("StateChanged"),
                    this, SLOT(slotStateChanged(QString,int,int,QDBusVariant,QSpiObjectReference)));
        if (!success) {
            qWarning() << "Could not subscribe to accessibility focus events.";
        }
    }

    Q_FOREACH(const QString &subscription, subscriptions) {
        QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.atspi.Registry"),
                                                        QLatin1String("/org/a11y/atspi/registry"),
                                                        QLatin1String("org.a11y.atspi.Registry"), QLatin1String("RegisterEvent"));
        m.setArguments(QVariantList() << subscription);

        QDBusMessage reply = conn.connection().call(m);
        if (reply.type() == QDBusMessage::ErrorMessage) {
            qWarning() << "Could not subscribe to accessibility event: " << reply.errorMessage();
        }
    }

//    subscriptions << QLatin1String("object:children-changed")
//                  << QLatin1String("object:property-change:accessiblename")
//                  << QLatin1String("object:state-changed")
//                  << QLatin1String("object:bounds-changed")
//                  << QLatin1String("object:visibledata-changed")
//                  << QLatin1String("object:state-changed")
//                  << QLatin1String("object:selection-changed")
//    << QLatin1String("object:")
//    <<
//    << QLatin1String("window:");


//    bool success = conn.connection().connect(QString(), QString(), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Activate"), this,
//                           SLOT(slotWindowActivated(QString,int,int,QDBusVariant,QSpiObjectReference)));
//    Q_ASSERT(success); // for now make sure we connect, else nothing will work
//    conn.connection().connect(QString(), QString(),
//                           QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Create"), this,
//                           SLOT(slotWindowCreated(QString,int,int,QDBusVariant,QSpiObjectReference)));
//    conn.connection().connect(QString(), QString(),
//                           QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Activate"), this,
//                           SLOT(slotWindowActivated(QString,int,int,QDBusVariant,QSpiObjectReference)));

//    conn.connection().connect(QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("ChildrenChanged"), this,
//                                  SLOT(slotChildrenChanged(QString, int, int, QDBusVariant, QSpiObjectReference)));
//    conn.connection().connect(QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("PropertyChanged"), this,
//                                  SLOT(slotPropertyChange(QString, int, int, QDBusVariant, QSpiObjectReference)));


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

}

Registry::EventListeners RegistryPrivate::eventListeners() const
{
    qWarning() << "IMPLEMENT: RegistryPrivate::subscribedEventListeners";
    return Registry::EventListeners();
}

AccessibleObject RegistryPrivate::parentAccessible(const AccessibleObject &object) const
{
    QVariant parent = getProperty(object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("Parent"));

    const QDBusArgument arg = parent.value<QDBusArgument>();
    QSpiObjectReference ref;
    arg >> ref;

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
        qWarning() << "Could not access role." << reply.error().message();
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
        qWarning() << "Could not access role." << reply.error().message();
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
    args << 0;
    args << coords;
    message.setArguments(args);

    QDBusReply< QRect > reply = conn.connection().call(message);
    if(!reply.isValid()){
        qWarning() << "Could not get Character Extents. " << reply.error().message();
        return QRect();
    }

    return reply.value();
}

QStringList RegistryPrivate::supportedInterfaces(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall(
            object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"),
                    QLatin1String("GetInterfaces"));

    QDBusReply<QStringList > reply = conn.connection().call(message);
    if(!reply.isValid()){
        qWarning() << "Could not get Interfaces. " << reply.error().message();
        return QStringList();
    }

    return reply.value();
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
        //action->setText(QString(QLatin1String("%1 (%2)")).arg(a.name).arg(a.description));
        action->setText(a.name);
        action->setWhatsThis(a.description);
        QKeySequence shortcut(a.keyBinding);
        action->setShortcut(shortcut);
        m_actionMapper.setMapping(action, QString(QLatin1String("%1;%2;%3")).arg(object.d->service).arg(object.d->path).arg(i));
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

void RegistryPrivate::slotStateChanged(const QString &state, int detail1, int /*detail2*/, const QDBusVariant &/*args*/, const QSpiObjectReference &reference)
{
    //qDebug() << "State changes: " << state;
    if ((state == QLatin1String("focused")) && (detail1 == 1)) {
        KAccessibleClient::AccessibleObject accessible = accessibleFromPath(reference.service, QDBusContext::message().path());
        emit focusChanged(accessible);
    }
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
     return AccessibleObject(const_cast<RegistryPrivate*>(this), service, path);
}

#include "registry_p.moc"
