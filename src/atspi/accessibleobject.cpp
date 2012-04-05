/*
    Copyright 2011 Peter Grasch <grasch@simon-listens.org>
    Copyright 2011-2012 Frederik Gladhorn <gladhorn@kde.org>

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

#include "accessibleobject.h"
#include "atspiaction.h"
#include "atspiwatcher.h"
#include <atspi/atspi-constants.h>

#include <QtDBus/QDBusVariant>
#include <QtDBus/QDBusArgument>
#include <QStringList>
#include <QDebug>

AccessibleObject::AccessibleObject(const QString &service, const QString &path)
    : m_service(service), m_path(path), m_conn(AtspiWatcher::instance()->connection())
{
    fetchName();
    fetchRole();
    fetchState();
    fetchIndexInParent();
    fetchChildCount();

//    //setting up monitoring
//    if (!parent) {
//        m_conn.connect(m_service, "", "org.a11y.atspi.Event.Object", "StateChanged", this,
//                       SLOT(slotStateChanged(QString, int, int, QDBusVariant, QSpiObjectReference)));
//        m_conn.connect(m_service, "", "org.a11y.atspi.Event.Object", "ChildrenChanged", this,
//                       SLOT(slotChildrenChanged(QString, int, int, QDBusVariant, QSpiObjectReference)));
//        m_conn.connect(m_service, "", "org.a11y.atspi.Event.Object", "PropertyChanged", this,
//                       SLOT(slotPropertyChange(QString, int, int, QDBusVariant, QSpiObjectReference)));
//    }

//    qDebug() << "Created new accessible object: " << m_name << m_path << role() << roleName();
}

AccessibleObject::~AccessibleObject()
{
    qDeleteAll(m_actions);
    qDeleteAll(m_children);
}

// Monitoring functions
//////////////////////////////

//AccessibleObject* AccessibleObject::findChild(const QString& path)
//{
//    if (m_path == path)
//        return this;

//    for (int i=0; i < childCount(); i++) {
//        AccessibleObject *child = getChild(i);
//        if (!child) {
//            resetChildren();
//            i=0; //restart
//            continue;
//        }
//        AccessibleObject *c = child->findChild(path);
//        if (c) return c;
//    }

//    return 0;
//}

//AccessibleObject* AccessibleObject::findOrCreateChild(const QString& pathOfChild)
//{
//    AccessibleObject *changedObject = findChild(pathOfChild);
//    if (!changedObject) {
//        //find parent object
//        QString parentPath = pathOfChild;
//        do
//        {
//            qDebug() << "Fetching parent of " << parentPath;
//            QDBusMessage parentReply = m_conn.call (QDBusMessage::createMethodCall(
//                                                        m_service, parentPath, "org.a11y.atspi.Accessible", "GetParent"));
//            if (parentReply.arguments().isEmpty() ||
//                    (!parentReply.arguments().first().isValid() || parentReply.arguments().first().isNull())) {
//                return 0;
//            }

//            QSpiObjectReference parentRef;
//            QDBusArgument parentArg(parentReply.arguments().first().value<QDBusArgument>());

//            parentArg >> parentRef;

//            parentPath = parentRef.path.path();

//            if (parentPath == QSPI_OBJECT_PATH_ACCESSIBLE_NULL)
//                return 0;

//            changedObject = findChild(parentPath);
//        }
//        while (!changedObject);

//        changedObject->resetChildren();
//        changedObject = findChild(pathOfChild);
//    }
//    return changedObject;
//}

//void AccessibleObject::slotStateChanged(const QString& change, int arg1, int arg2, QDBusVariant data, QSpiObjectReference reference)
//{
//    Q_UNUSED(data);
//    Q_UNUSED(reference);
//    qDebug() << "State changed of " << message().path() << ": " << change << arg1 << arg2;
//    AccessibleObject *o = findOrCreateChild(message().path());
//    if (o) {
//        o->fetchState();
//        emit changed();
//    }
//}

//void AccessibleObject::slotChildrenChanged(const QString &change, int arg1, int arg2, QDBusVariant data, QSpiObjectReference reference)
//{
//    Q_UNUSED(reference);
//    QDBusArgument dataArg(data.variant().value<QDBusArgument>());
//    QSpiObjectReference ref;
//    dataArg >> ref;

//    qDebug() << "Children changed of " << ref.path.path() << ": " << change << arg1 << arg2;
//    AccessibleObject *o = findOrCreateChild(ref.path.path());
//    if (o) {
//        if ((o == this) && (change == "remove")) {
//            emit serviceRemoved(this);
//        } else {
//            o->getParent()->resetChildren(); //because o != this there will be a parent
//            emit changed();
//        }
//    } else {
//        //can't do anything else but re-fresh everything at this point...
//        resetChildren();
//        emit changed();
//    }
//}

//void AccessibleObject::slotPropertyChange(const QString& change, int arg1, int arg2, QDBusVariant data, QSpiObjectReference reference)
//{
//    Q_UNUSED(data);
//    Q_UNUSED(reference);
//    if (change != "accessible-name")
//        return; //everything is irrelevant for us

//    qDebug() << "Property changed of " << message().path() << ": " << change << arg1 << arg2;
//    AccessibleObject *o = findOrCreateChild(message().path());
//    if (o) {
//        o->fetchName();
//        emit changed();
//    }
//}

// Fetching functions
//////////////////////////////

void AccessibleObject::fetchChildCount()
{
    m_childCount = getProperty ( m_service, m_path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("ChildCount")).toInt();
}

void AccessibleObject::fetchIndexInParent()
{
//    QDBusMessage indexInParentReply = m_conn.call (QDBusMessage::createMethodCall(
//                                                       m_service, m_path, "org.a11y.atspi.Accessible", "GetIndexInParent"));
//    m_indexInParent = (indexInParentReply.arguments().isEmpty()) ? -1 : indexInParentReply.arguments().at ( 0 ).toInt();
}

void AccessibleObject::fetchName()
{
    m_name = getProperty ( m_service, m_path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("Name")).toString();
    m_description = getProperty ( m_service, m_path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("Description")).toString();

    m_name.remove(QRegExp(QLatin1String("<[^>]*>"))); //strip html tags
    m_name.replace(QLatin1Char('\n'), QLatin1Char(' ')); //remove linebreaks

    //TODO: NUMBERS, Abbreviations, etc.!
//    m_name.remove(QRegExp("[^a-z A-Z 0-9]")); //strip everything except characters and spaces
    m_name.replace(QRegExp(QLatin1String("  +")), QLatin1String(" ")); //eliminate double spacing
    m_name = m_name.trimmed();
}

void AccessibleObject::fetchRole()
{
    QDBusMessage roleReply = m_conn.call ( QDBusMessage::createMethodCall (
                                               m_service, m_path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetRole" ) ));
    if (roleReply.arguments().isEmpty())
        m_role = -1;
    else
        m_role = roleReply.arguments().at ( 0 ).toInt();
}

void AccessibleObject::fetchState()
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                m_service, m_path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetState"));

    QDBusMessage reply = m_conn.call ( message );
    if (reply.arguments().isEmpty()) {
        m_state = 0;
        return;
    }

    const QDBusArgument returnVal = reply.arguments().at ( 0 ).value<QDBusArgument>();

    QSpiUIntList state;

    returnVal.beginArray();
    while (!returnVal.atEnd()) {
        int arg;
        returnVal >> arg;
        state << arg;
    }
    returnVal.endArray();

    Q_ASSERT(state.count() == 2);

    m_state = state.at(1);
    m_state = m_state << 32;
    m_state += state.at(0);
}

void AccessibleObject::fetchActions() const
{
//    //fetch actions
//    QDBusMessage message = QDBusMessage::createMethodCall (
//                m_service, m_path, "org.a11y.atspi.Action", "GetActions" );

//    QDBusMessage reply = m_conn.call ( message );

//    if (reply.arguments().isEmpty())
//        return; //no actions
    
//    const QDBusArgument returnVal = reply.arguments().at (0).value<QDBusArgument>();

//    returnVal.beginArray();
//    while (!returnVal.atEnd()) {
//        returnVal.beginStructure();
//        QString name, description, comment;
//        returnVal >> name;
//        returnVal >> description;
//        returnVal >> comment;
//        m_actions.append(new AtspiAction(name, description, comment));
//        returnVal.endStructure();
//    }
//    returnVal.endArray();
}

// Getter functions (lazy)
//////////////////////////////

bool AccessibleObject::hasActions() const
{
    if (m_actions.isEmpty())
        fetchActions();

    return !m_actions.isEmpty();
}

QList< AtspiAction* > AccessibleObject::actions() const
{
    if (m_actions.isEmpty())
        fetchActions();

    return m_actions;
}

QSharedPointer<AccessibleObject> AccessibleObject::getChild ( int index ) const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                m_service, m_path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetChildAtIndex"));

    message.setArguments ( QVariantList() << index );

    QDBusMessage reply = m_conn.call ( message );
    QString service;
    QString path;
    if (reply.arguments().isEmpty())
        return QSharedPointer<AccessibleObject>();
    const QDBusArgument arg = reply.arguments().at ( 0 ).value<QDBusArgument>();

    arg.beginStructure();
    arg >> service;
    arg >> path;
    arg.endStructure();

    return QSharedPointer<AccessibleObject>(new AccessibleObject(service, path));
}


// Helper functions
//////////////////////////////

QString AccessibleObject::roleName() const
{
    QDBusMessage message = QDBusMessage::createMethodCall (
                m_service, m_path, QLatin1String("org.a11y.atspi.Accessible"), QLatin1String("GetRoleName") );

    QDBusMessage reply = m_conn.call ( message );
    return (reply.arguments().isEmpty()) ? QString() : reply.arguments().at ( 0 ).toString();
}


QVariant AccessibleObject::getProperty ( const QString &service, const QString &path, const QString &interface, const QString &name ) const
{
    QVariantList args;
    args.append ( interface );
    args.append ( name );

    QDBusMessage message = QDBusMessage::createMethodCall (
                service, path, QLatin1String("org.freedesktop.DBus.Properties"), QLatin1String("Get") );

    message.setArguments ( args );
    QDBusMessage reply = m_conn.call ( message );
    if (reply.arguments().isEmpty()) return QVariant();

    QDBusVariant v = reply.arguments().at ( 0 ).value<QDBusVariant>();
    return v.variant();
}

void AccessibleObject::resetChildren()
{
//    //invalidate children cache to be re-build on next access
//    QHash<QString, AccessibleObject*> oldChildren;
//    foreach (AccessibleObject *o, m_children)
//        if (o)
//            oldChildren.insert(o->path(), o);
//    m_children.clear();
//    fetchChildCount();

//    for (int i=0; i < m_childCount; i++) {
//        AccessibleObject *newObject = getChild(i);
//        if (!newObject) { //children changed again...
//            foreach (AccessibleObject *o, m_children)
//                if (o)
//                    oldChildren.insert(o->path(), o); // move to oldChildren again
//            m_children.clear();
//            fetchChildCount();
//            i=0;
//            continue;
//        }
//        QString childPath = newObject->path();
//        //replace newly created child with old one
//        if (oldChildren.contains(childPath)) {
//            delete m_children.takeAt(i);
//            m_children.insert(i, oldChildren.take(childPath));
//        }
//    }

//    qDeleteAll(oldChildren.values());
}

// Logic functions
//////////////////////////////

bool AccessibleObject::isVisible() const
{
    switch (m_role) {
    case ATSPI_ROLE_APPLICATION:
        return true;
    case ATSPI_ROLE_DIALOG:
    case ATSPI_ROLE_FRAME:
    case ATSPI_ROLE_WINDOW:
        return (m_state & (quint64(1) << ATSPI_STATE_ACTIVE));
    default:
        return ((m_state & (quint64(1) << ATSPI_STATE_VISIBLE)) &&
                (m_state & (quint64(1) << ATSPI_STATE_SHOWING)));
    }
}

QStringList AccessibleObject::traverseObject()
{
    QStringList names;
//    if (isShown()) {
//        if (hasActions() && !m_name.isEmpty())
//            names << m_name;

//        for (int i=0; i < childCount(); i++) {
//            AccessibleObject *child = getChild(i);
//            if (!child) {
//                resetChildren();
//                i=0;
//                continue;
//            }
//            names << child->traverseObject();
//        }
//    } else
//        qDebug() << "Not visible: " << m_name;

    return names;
}

bool AccessibleObject::trigger(const QString& name_) const
{
//    if (isShown()) {
//        if (name() == name_) {
//            QList<AtspiAction*> a = actions();
//            if (!a.isEmpty()) {
//                QVariantList inargs;
//                inargs << 0; //TODO: display list for ambiguous actions

//                QDBusMessage message = QDBusMessage::createMethodCall (
//                            m_service, m_path, "org.a11y.atspi.Action", "DoAction" );
//                message.setArguments ( inargs );

//                QDBusMessage reply = m_conn.call ( message );

//                if (!reply.arguments().isEmpty() && reply.arguments().at ( 0 ).toBool())
//                    return true;
//            }
//        }

//        for (int i=0; i < childCount(); i++)
//            if (getChild(i)->trigger(name_)) return true;
//    }

    return false;
}


QString AccessibleObject::description() const
{
    return m_description;
}

QString AccessibleObject::name() const
{
    return m_name;
}

qreal AccessibleObject::value() const
{
    QVariant value = getProperty(m_service, m_path, QLatin1String("org.a11y.atspi.Value"), QLatin1String("CurrentValue"));
    return value.toReal();
}

QString AccessibleObject::service() const
{
    return m_service;
}

int AccessibleObject::indexInParent() const
{
    return m_indexInParent;
}

int AccessibleObject::childCount() const
{
    return m_childCount;
}

int AccessibleObject::role() const
{
    return m_role;
}

quint64 AccessibleObject::state() const
{
    return m_state;
}

AccessibleObject *AccessibleObject::getParent() const
{
    return m_parent;
}

QString AccessibleObject::path() const
{
    return m_path;
}
