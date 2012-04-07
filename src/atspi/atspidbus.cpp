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

void AtSpiDBus::subscribeEventListeners(const Registry::EventListeners &listeners)
{
    QStringList subscriptions;

    if (listeners & Registry::Focus) {
        subscriptions << QLatin1String("focus:");
        bool success = m_connection->connection().connect(
                    QString(), QString(), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("StateChanged"),
                    this, SLOT(slotStateChanged(QString, int, int, QDBusVariant, QSpiObjectReference)));
        if (!success) {
            qWarning() << "Could not subscribe to accessibility focus events.";
        }
    }

    Q_FOREACH(const QString &subscription, subscriptions) {
        QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String("org.a11y.atspi.Registry"),
                                                        QLatin1String("/org/a11y/atspi/registry"),
                                                        QLatin1String("org.a11y.atspi.Registry"), QLatin1String("RegisterEvent"));
        m.setArguments(QVariantList() << subscription);

        QDBusMessage reply = m_connection->connection().call(m);
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


//    bool success = m_connection->connection().connect(QString(), QString(), QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Activate"), this,
//                           SLOT(slotWindowActivated(QString,int,int,QDBusVariant,QSpiObjectReference)));
//    Q_ASSERT(success); // for now make sure we connect, else nothing will work
//    m_connection->connection().connect(QString(), QString(),
//                           QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Create"), this,
//                           SLOT(slotWindowCreated(QString,int,int,QDBusVariant,QSpiObjectReference)));
//    m_connection->connection().connect(QString(), QString(),
//                           QLatin1String("org.a11y.atspi.Event.Window"), QLatin1String("Activate"), this,
//                           SLOT(slotWindowActivated(QString,int,int,QDBusVariant,QSpiObjectReference)));

//    m_connection->connection().connect(QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("ChildrenChanged"), this,
//                                  SLOT(slotChildrenChanged(QString, int, int, QDBusVariant, QSpiObjectReference)));
//    m_connection->connection().connect(QString(), QLatin1String(""), QLatin1String("org.a11y.atspi.Event.Object"), QLatin1String("PropertyChanged"), this,
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

Registry::EventListeners AtSpiDBus::subscribedEventListeners() const
{
    qWarning() << "IMPLEMENT: AtSpiDBus::subscribedEventListeners";
    return Registry::EventListeners();
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

int AtSpiDBus::childCount(const AccessibleObject &object) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetChildCount"));

    QDBusReply<QVariant> reply = m_connection->connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access childCount." << reply.error().message();
        return 0;
    }
    return reply.value().toInt();
}

AccessibleObject AtSpiDBus::child(const AccessibleObject &object, int index) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                object.d->service, object.d->path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetChildAtIndex"));
    QVariantList args;
    args << index;
    message.setArguments(args);

    QDBusReply<QSpiObjectReference> reply = m_connection->connection().call(message);
    if (!reply.isValid()) {
        qWarning() << "Could not access child." << reply.error().message();
        return AccessibleObject(0, QString(), QString());
    }
    const QSpiObjectReference child = reply.value();
    return AccessibleObject(const_cast<AtSpiDBus*>(this), child.service, child.path.path());
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

