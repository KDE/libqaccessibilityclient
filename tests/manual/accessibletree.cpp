/*
 *  Copyright 2012 Frederik Gladhorn <gladhorn@kde.org>
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "accessibletree.h"

#include <qdebug.h>
#include <qstack.h>

using namespace KAccessibleClient;

AccessibleWrapper* AccessibleWrapper::parent()
{
    return m_parent;
}

AccessibleWrapper* AccessibleWrapper::child(int index)
{
    if (m_children.isEmpty()) {
        QList<AccessibleObject> children = acc.children();
        foreach (const AccessibleObject &c, children) {
            m_children.append(new AccessibleWrapper(c, this));
        }
    }
    if (index >= 0 && index < m_children.count()) {
        return m_children.at(index);
    }
    return 0;
}

int AccessibleWrapper::childCount()
{
    if (m_children.isEmpty())
        return acc.childCount();
    return m_children.count();
}

AccessibleTree::AccessibleTree(QObject* parent)
    : QAbstractItemModel(parent), m_registry(0)
{
}

AccessibleTree::~AccessibleTree()
{
}

QVariant AccessibleTree::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return "Accessible";
        } else if (section == 1) {
            return "Role";
        }
    }
    return QVariant();
}

int AccessibleTree::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant AccessibleTree::data(const QModelIndex& index, int role) const
{
    if (!m_registry || !index.isValid())
        return QVariant();

    AccessibleObject acc = static_cast<AccessibleWrapper*>(index.internalPointer())->acc;

    switch (role) {
        case Qt::DisplayRole:
            if (index.column() == 0) {
                return acc.name();
            } else if (index.column() == 1) {
                return acc.roleName();
            }

        default:
            return QVariant();
    }
    return QVariant();
}

QModelIndex AccessibleTree::index(int row, int column, const QModelIndex& parent) const
{
    if (!m_registry || (column < 0) || (column > 1) || (row < 0))
        return QModelIndex();

//     qDebug() << "index:" << row << column << parent;
    if (!parent.isValid()) {
        if (row < m_apps.count()) {
            return createIndex(row, column, m_apps.at(row));
        }
    } else {
        AccessibleWrapper *wraper = static_cast<AccessibleWrapper*>(parent.internalPointer());
        if (row < wraper->childCount()) {
            QModelIndex newIndex = createIndex(row, column, wraper->child(row));
            if (newIndex.parent() != parent) {
                qWarning() << "Broken navigation: " << parent << row;
                emit navigationError(parent);
            }
            return newIndex;
        } else {
            qWarning() << "Could not access child: " << wraper->acc.name() << wraper->acc.roleName();
        }
    }

    return QModelIndex();
}

QModelIndex AccessibleTree::parent(const QModelIndex& child) const
{
//     qDebug() << "Parent: " << child;
    if (child.isValid()) {
        AccessibleWrapper *wraper = static_cast<AccessibleWrapper*>(child.internalPointer());
        AccessibleWrapper *parent = wraper->parent();
        if (parent) {
            // if this is a top-level item, it has no parent
            if (parent->parent()) {
                return createIndex(parent->acc.indexInParent(), 0, parent);
            } else {
                return createIndex(m_apps.indexOf(parent), 0, parent);
            }
        }
    }

    return QModelIndex();
}

int AccessibleTree::rowCount(const QModelIndex& parent) const
{
    if (!m_registry || parent.column() > 0)
        return 0;

//     qDebug() << "row count:" << parent << parent.internalPointer();
    if (!parent.isValid()) {
        return m_apps.count();
    } else {
        if (!parent.internalPointer())
            return 0;

        AccessibleWrapper *wraper = static_cast<AccessibleWrapper*>(parent.internalPointer());
//         qDebug() << "     row count:" << wraper->acc.name() << wraper->acc.roleName() << wraper->childCount();
        return wraper->childCount();
    }

    return 0;
}

void AccessibleTree::setRegistry(KAccessibleClient::Registry* registry)
{
    m_registry = registry;
    resetModel();
}

AccessibleWrapper* AccessibleTree::addHierachyForObject(const AccessibleObject &object)
{
    bool isApp = object.supportedInterfaces().testFlag(KAccessibleClient::AccessibleObject::Application);

    AccessibleObject parent = isApp ? AccessibleObject() : object.parent();
    AccessibleWrapper *wraper = 0;
    if (parent.isValid()) {
//         AccessibleWrapper *parentWraper = static_cast<AccessibleWrapper*>(parent.internalPointer());
//         Q_ASSERT(parentWraper);
        AccessibleWrapper *parentWraper = addHierachyForObject(parent);
        wraper = parentWraper->child(object.indexInParent());
        Q_ASSERT(wraper);
    } else {
        wraper = new AccessibleWrapper(object, 0);
    }
    return wraper;
}

void AccessibleTree::resetModel()
{
    beginResetModel();
    qDeleteAll(m_apps);
    m_apps.clear();
    if (m_registry) {
        QList<AccessibleObject> children = m_registry->applications();
        foreach (const AccessibleObject &c, children) {
            AccessibleWrapper* wraper = addHierachyForObject(c);
            while(AccessibleWrapper* p = wraper->parent())
                wraper = p;
            m_apps.append(wraper);
        }
    }
    endResetModel();
}

QModelIndex AccessibleTree::indexForAccessible(const AccessibleObject& object)
{
    if (!object.isValid())
        return QModelIndex();

    if (object.supportedInterfaces().testFlag(KAccessibleClient::AccessibleObject::Application)) {
        // top level
        for (int i = 0; i < m_apps.size(); ++i) {
            if (m_apps.at(i)->acc == object)
                return createIndex(i, 0, m_apps.at(i));
        }
    } else {
        AccessibleObject parent = object.parent();
        if (parent.isValid()) {
            QModelIndex parentIndex = indexForAccessible(parent);
            QModelIndex in = index(object.indexInParent(), 0, parentIndex);
            //qDebug() << "indexForAccessible: " << object.name() << data(in).toString()  << " parent: " << data(parentIndex).toString();//" row: " << object.indexInParent() << "parentIndex: " << parentIndex;
            return in;
        } else {
            qWarning() << Q_FUNC_INFO << "Invalid indexForAccessible: " << object;
//Q_ASSERT(!object.supportedInterfaces().testFlag(KAccessibleClient::AccessibleObject::Application));
//return indexForAccessible(object.application());

            Q_FOREACH(const KAccessibleClient::AccessibleObject &child, object.children()) {
                if (child.supportedInterfaces().testFlag(KAccessibleClient::AccessibleObject::Application)) {
                    for (int i = 0; i < m_apps.size(); ++i) {
                        if (m_apps.at(i)->acc == object)
                            return createIndex(i, 0, m_apps.at(i));
                    }
                }
            }
        }
    }
    return QModelIndex();
}

bool AccessibleTree::addAccessible(const KAccessibleClient::AccessibleObject &object)
{
    qDebug()<<Q_FUNC_INFO<<object;
    KAccessibleClient::AccessibleObject parent = object.parent();
    QModelIndex parentIndex = indexForAccessible(parent);
    if (parentIndex.isValid()) {
        int idx = object.indexInParent();
        //int idx = parent.children().indexOf(object);
        Q_ASSERT(idx >= 0);
        QModelIndex objectIndex = index(idx, 0, parentIndex);
        if (objectIndex.isValid() && static_cast<AccessibleWrapper*>(objectIndex.internalPointer())->acc == object) {
            emit dataChanged(objectIndex, objectIndex);
        } else {
            beginInsertRows(parentIndex, idx, idx);
            AccessibleWrapper *parentWrapper = static_cast<AccessibleWrapper*>(parentIndex.internalPointer());
            Q_ASSERT(parentWrapper);
            parentWrapper->m_children.insert(idx, new AccessibleWrapper(object, parentWrapper));
            endInsertRows();
        }
    } else { // top-level
        QList<KAccessibleClient::AccessibleObject> newChildren;
        if (object.supportedInterfaces().testFlag(KAccessibleClient::AccessibleObject::Application)) {
            QModelIndex objectIndex = indexForAccessible(object);
            if (objectIndex.isValid()) {
                emit dataChanged(objectIndex, objectIndex);
            } else {
                newChildren.append(object);
            }
        } else {
            // This is for the case there was a new desktop widget created on the top of one ore several new apps.
            Q_FOREACH(const KAccessibleClient::AccessibleObject &child, object.children()) {
                if (child.supportedInterfaces().testFlag(KAccessibleClient::AccessibleObject::Application)) {
                    bool alreadyKnown = false;
                    for (int i = 0; i < m_apps.size() && !alreadyKnown; ++i)
                        if (m_apps.at(i)->acc == child)
                            alreadyKnown = true;
                    if (alreadyKnown)
                        continue;
                    QModelIndex childIndex = indexForAccessible(child);
                    if (childIndex.isValid()) {
                        emit dataChanged(childIndex, childIndex);
                    } else {
                        newChildren.append(child);
                    }
                }
            }
        }

        if (!newChildren.isEmpty()) {
            int idx = m_apps.count();
            beginInsertRows(QModelIndex(), idx, idx + newChildren.count() - 1);
            Q_FOREACH(const KAccessibleClient::AccessibleObject &child, newChildren) {
                m_apps.append(new AccessibleWrapper(child, 0));
            }
            endInsertRows();
        }
    }
    return true;
}

bool AccessibleTree::removeAccessible(const KAccessibleClient::AccessibleObject &object)
{
    qDebug() << Q_FUNC_INFO << object;
    QModelIndex index = indexForAccessible(object);
    if (!index.isValid())
        return false;
    return removeAccessible(index);
}

bool AccessibleTree::removeAccessible(const QModelIndex &index)
{
    qDebug() << Q_FUNC_INFO << index;
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.model() == this);
    QModelIndex parent = index.parent();
    int row = index.row();
    beginRemoveRows(parent, row, row);
    if (parent.isValid()) {
        AccessibleWrapper *wraper = static_cast<AccessibleWrapper*>(parent.internalPointer());
        Q_ASSERT(wraper);
        delete wraper->m_children.takeAt(row);
    } else {
        AccessibleWrapper *wraper = static_cast<AccessibleWrapper*>(index.internalPointer());
        Q_ASSERT(wraper);
        Q_ASSERT(m_apps[row] == wraper);
        if (m_apps[row] == wraper) {
            qDebug() << Q_FUNC_INFO << "Delete application accessible object! indexRow=" << row;
            delete m_apps.takeAt(row);
        }
    }
    endRemoveRows();
}

