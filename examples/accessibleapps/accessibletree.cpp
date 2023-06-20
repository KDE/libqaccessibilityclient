/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibletree.h"

#include <QDebug>
#include <QStack>

using namespace QAccessibleClient;

AccessibleWrapper* AccessibleWrapper::parent()
{
    return m_parent;
}

AccessibleWrapper* AccessibleWrapper::child(int index)
{
    if (m_children.isEmpty()) {
        const QList<AccessibleObject> children = acc.children();
        for (const AccessibleObject &c : children) {
            m_children.append(new AccessibleWrapper(c, this));
        }
    }
    if (index >= 0 && index < m_children.count()) {
        return m_children.at(index);
    }
    return nullptr;
}

int AccessibleWrapper::childCount()
{
    if (m_children.isEmpty())
        return acc.childCount();
    return m_children.count();
}

AccessibleTree::AccessibleTree(QObject* parent)
    : QAbstractItemModel(parent), m_registry(nullptr)
{
}

AccessibleTree::~AccessibleTree()
{
}

QVariant AccessibleTree::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return QStringLiteral("Accessible");
        } else if (section == 1) {
            return QStringLiteral("Role");
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
                QString name = acc.name();
                if (name.isEmpty())
                    name = QStringLiteral("[%1]").arg(acc.roleName());
                return name;
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
                Q_EMIT navigationError(parent);
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

void AccessibleTree::setRegistry(QAccessibleClient::Registry* registry)
{
    m_registry = registry;
    resetModel();
}

void AccessibleTree::resetModel()
{
    beginResetModel();
    qDeleteAll(m_apps);
    m_apps.clear();
    if (m_registry) {
        const QList<AccessibleObject> children = m_registry->applications();
        for (const AccessibleObject &c : children) {
            m_apps.append(new AccessibleWrapper(c, nullptr));
        }
    }
    endResetModel();
}

void AccessibleTree::updateTopLevelApps()
{
    QList<AccessibleObject> topLevelApps = m_registry->applications();
    for (int i = m_apps.count() - 1; i >= 0; --i) {
        AccessibleObject app = m_apps.at(i)->acc;
        int indexOfApp = topLevelApps.indexOf(app);
        if (indexOfApp < 0) {
            removeAccessible(index(i, 0, QModelIndex()));
        } else {
            topLevelApps.takeAt(i);
        }
    }

    for (const AccessibleObject &newApp : std::as_const(topLevelApps)) {
        addAccessible(newApp);
    }
}

QModelIndex AccessibleTree::indexForAccessible(const AccessibleObject& object)
{
    if (!object.isValid())
        return QModelIndex();

    if (object.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface)) {
        // top level
        for (int i = 0; i < m_apps.size(); ++i) {
            if (m_apps.at(i)->acc == object)
                return createIndex(i, 0, m_apps.at(i));
        }
        int lastIndex = m_apps.size();
        if (addAccessible(object) && m_apps.at(lastIndex)->acc == object)
            return createIndex(lastIndex, 0, m_apps.at(lastIndex));

    } else {
        AccessibleObject parent = object.parent();
        if (parent.isValid()) {
            QModelIndex parentIndex = indexForAccessible(parent);
            if (!parentIndex.isValid()) {
                if (object.isValid() && object.application().isValid())
                    qWarning() << Q_FUNC_INFO << object.application().name() << object.name() << object.roleName() << "Parent model index is invalid: " << object;

                return QModelIndex();
            }
            int indexInParent = object.indexInParent();
            if (indexInParent < 0) {
                qWarning() << Q_FUNC_INFO << "indexInParent is invalid: " << object;
                return QModelIndex();
            }
            QModelIndex in = index(indexInParent, 0, parentIndex);
            //qDebug() << "indexForAccessible: " << object.name() << data(in).toString()  << " parent: " << data(parentIndex).toString();//" row: " << object.indexInParent() << "parentIndex: " << parentIndex;
            return in;
        } else {
            qWarning() << Q_FUNC_INFO << "Invalid indexForAccessible: " << object;
//Q_ASSERT(!object.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::Application));
//return indexForAccessible(object.application());

            for (const QAccessibleClient::AccessibleObject &child : object.children()) {
                if (child.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface)) {
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

bool AccessibleTree::addAccessible(const QAccessibleClient::AccessibleObject &object)
{
    // qDebug() << Q_FUNC_INFO << object;
    QAccessibleClient::AccessibleObject parent = object.parent();

    // We have no parent -> top level.
    if (!parent.isValid()) {
        if (!object.supportedInterfaces().testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface))
            qWarning() << Q_FUNC_INFO << "Found top level accessible that does not implement the application interface" << object;

        beginInsertRows(QModelIndex(), m_apps.count(), m_apps.count());
        m_apps.append(new AccessibleWrapper(object, nullptr));
        endInsertRows();
        return true;
    }

    // If the parent is not known, add it too.
    QModelIndex parentIndex = indexForAccessible(parent);
    if (!parentIndex.isValid()) {
        if (!addAccessible(parent)) {
            qWarning() << Q_FUNC_INFO << "Could not add accessible (invalid parent): " << object;
            return false;
        }
        parentIndex = indexForAccessible(parent);
        Q_ASSERT(parentIndex.isValid());
    }

    // Add this item (or emit dataChanged, if it's there already).
    int idx = object.indexInParent();
    if (idx < 0) {
            qWarning() << Q_FUNC_INFO << "Could not add accessible (invalid index in parent): " << object;
            return false;
    }
    QModelIndex objectIndex = index(idx, 0, parentIndex);
    if (objectIndex.isValid() && static_cast<AccessibleWrapper*>(objectIndex.internalPointer())->acc == object) {
        Q_EMIT dataChanged(objectIndex, objectIndex);
        return false;
    }

    beginInsertRows(parentIndex, idx, idx);
    AccessibleWrapper *parentWrapper = static_cast<AccessibleWrapper*>(parentIndex.internalPointer());
    Q_ASSERT(parentWrapper);
    parentWrapper->m_children.insert(idx, new AccessibleWrapper(object, parentWrapper));
    endInsertRows();
    return true;
}

bool AccessibleTree::removeAccessible(const QAccessibleClient::AccessibleObject &object)
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
    bool removed = false;
    beginRemoveRows(parent, row, row);
    if (parent.isValid()) {
        AccessibleWrapper *wraper = static_cast<AccessibleWrapper*>(parent.internalPointer());
        Q_ASSERT(wraper);
        delete wraper->m_children.takeAt(row);
        removed = true;
    } else {
        AccessibleWrapper *wraper = static_cast<AccessibleWrapper*>(index.internalPointer());
        Q_ASSERT(wraper);
        Q_ASSERT(m_apps[row] == wraper);
        if (m_apps[row] == wraper) {
            qDebug() << Q_FUNC_INFO << "Delete application accessible object! indexRow=" << row;
            delete m_apps.takeAt(row);
            removed = true;
        }
    }
    endRemoveRows();
    return removed;
}

void AccessibleTree::updateAccessible(const QAccessibleClient::AccessibleObject &object)
{
    QModelIndex index = indexForAccessible(object);
    Q_EMIT dataChanged(index, index);
}

#include "moc_accessibletree.cpp"
