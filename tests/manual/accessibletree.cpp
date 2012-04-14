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
    qDeleteAll(m_apps);
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
    QList<AccessibleObject> children = m_registry->applications();
    foreach (const AccessibleObject &c, children) {
        m_apps.append(new AccessibleWrapper(c, 0));
    }
}

void AccessibleTree::resetModel()
{
    qDebug() << "reset model...";
    qDeleteAll(m_apps);
    m_apps.clear();
    if (m_registry) {
        QList<AccessibleObject> children = m_registry->applications();
        foreach (const AccessibleObject &c, children) {
            m_apps.append(new AccessibleWrapper(c, 0));
        }
    }
    reset();
}

QModelIndex AccessibleTree::indexForAccessible(const AccessibleObject& object)
{
    if (object.isValid()) {
        if (object.parent().isValid()) {
            QModelIndex parent = indexForAccessible(object.parent());
            QModelIndex in = index(object.indexInParent(), 0, parent);
            qDebug() << "indexForAccessible: " << object.name() << data(in).toString()  << " parent: " << data(parent).toString();//" row: " << object.indexInParent() << "parent: " << parent;
            return in;

        } else {
            // top level
            for (int i = 0; i < m_apps.size(); ++i) {
                if (m_apps.at(i)->acc == object)
                    return createIndex(i, 0, m_apps.at(i));
            }
            // not found? try again...
            // FIXME
            resetModel();
            for (int i = 0; i < m_apps.size(); ++i) {
                if (m_apps.at(i)->acc == object)
                    return createIndex(i, 0, m_apps.at(i));
            }
        }
    }
    return QModelIndex();
}


