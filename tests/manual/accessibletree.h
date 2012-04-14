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

#ifndef ACCESSIBLETREE_H
#define ACCESSIBLETREE_H

#include <qabstractitemmodel.h>

#include <kdeaccessibilityclient/registry.h>

class AccessibleWrapper
{
public:
    AccessibleWrapper(const KAccessibleClient::AccessibleObject &object, AccessibleWrapper *parent)
    : acc(object), m_parent(parent)
    {}
    KAccessibleClient::AccessibleObject acc;

    ~AccessibleWrapper() {
        qDeleteAll(m_children);
    }

    int childCount();
    AccessibleWrapper *child(int index);
    AccessibleWrapper *parent();

private:
    AccessibleWrapper *m_parent;
    QList<AccessibleWrapper*> m_children;
};

class AccessibleTree :public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AccessibleTree(QObject* parent = 0);
    ~AccessibleTree();
    void setRegistry(KAccessibleClient::Registry *registry);

public Q_SLOTS:
    void resetModel();

Q_SIGNALS:
    void navigationError(const QModelIndex &) const;

protected:
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    KAccessibleClient::Registry *m_registry;
    QList<AccessibleWrapper*> m_apps;
};

#endif // ACCESSIBLETREE_H
