/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef ACCESSIBLETREE_H
#define ACCESSIBLETREE_H

#include <QAbstractItemModel>

#include <qaccessibilityclient/registry.h>

class AccessibleTree;

class AccessibleWrapper
{
public:
    AccessibleWrapper(const QAccessibleClient::AccessibleObject &object, AccessibleWrapper *parent)
    : acc(object), m_parent(parent)
    {}

    QAccessibleClient::AccessibleObject acc;

    ~AccessibleWrapper() {
        qDeleteAll(m_children);
    }

    int childCount();
    AccessibleWrapper *child(int index);
    AccessibleWrapper *parent();

private:
    friend class AccessibleTree;

    AccessibleWrapper *m_parent;
    QList<AccessibleWrapper*> m_children;
};

class AccessibleTree :public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AccessibleTree(QObject* parent = nullptr);
    ~AccessibleTree() override;

    void setRegistry(QAccessibleClient::Registry *registry);

    QModelIndex indexForAccessible(const QAccessibleClient::AccessibleObject &object);
    bool addAccessible(const QAccessibleClient::AccessibleObject &object);
    bool removeAccessible(const QAccessibleClient::AccessibleObject &object);
    bool removeAccessible(const QModelIndex &index);
    void updateAccessible(const QAccessibleClient::AccessibleObject &object);

    QList<AccessibleWrapper*> apps() const { return m_apps; }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public Q_SLOTS:
    void resetModel();
    /*!
        \brief Updates all applications in the tree.

        Removes and adds top level applications. This is less invasive then modelReset.
    */
    void updateTopLevelApps();

Q_SIGNALS:
    void navigationError(const QModelIndex &) const;

private:
    QAccessibleClient::Registry *m_registry;
    QList<AccessibleWrapper*> m_apps;
};

#endif // ACCESSIBLETREE_H
