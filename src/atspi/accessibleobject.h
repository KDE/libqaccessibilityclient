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

#ifndef ACCESSIBLEOBJECT_H
#define ACCESSIBLEOBJECT_H


#include "qt-atspi.h"

#include <atspi/atspi-constants.h>

#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVector>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusContext>

#include <qsharedpointer.h>


class AtspiAction;
class AtspiWatcher;

class AccessibleObject : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    explicit AccessibleObject(const QString &service, const QString &path);
    ~AccessibleObject();

    //getter functions
    QString name() const;
    QString description() const;
    QString service() const;
    qreal value() const;
    AccessibleObject *getParent() const;
    QString path() const;
    int indexInParent() const;
    int childCount() const;
    int role() const;
    quint64 state() const;
    bool hasActions() const;
    QList<AtspiAction*> actions() const;
    bool isVisible() const;
    QSharedPointer<AccessibleObject> getChild(int index) const;

    //info functions (expensive)
    QString roleName() const;

    //actions
    /**
   * \return List of currently visible commands
   */
    QStringList traverseObject();
    bool trigger(const QString& name) const;

    //monitoring
    void resetChildren();

private:
    //set in constructor
    AccessibleObject *m_parent;
    QString m_service;
    QString m_path;
    QDBusConnection m_conn;

    AtspiWatcher *m_watcher;

    //fetched in constructor
    QString m_name;
    QString m_description;
    int m_role;
    quint64 m_state;
    int m_indexInParent;
    int m_childCount;

    //fetch on demand
    mutable QList<AccessibleObject*> m_children;
    mutable QList<AtspiAction*> m_actions;

    //helper functions
    QVariant getProperty(const QString &service, const QString &path, const QString &interface, const QString &name) const;

    //fetching data
    void fetchActions() const;

    void fetchState();
    void fetchName();
    void fetchRole();
    void fetchIndexInParent();
    void fetchChildCount();

//    AccessibleObject* findChild(const QString& path);
//    AccessibleObject* findOrCreateChild(const QString& path);

//private slots:
//    void slotPropertyChange(const QString& change, int, int, QDBusVariant, QSpiObjectReference);
//    void slotStateChanged(const QString& change, int, int, QDBusVariant, QSpiObjectReference);
//    void slotChildrenChanged(const QString& change, int, int, QDBusVariant, QSpiObjectReference);
};


#endif // ACCESSIBLEOBJECT_H
