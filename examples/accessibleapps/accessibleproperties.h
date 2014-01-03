/*
    Copyright 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

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

#ifndef ACCESSIBLEPROPERTIES_H
#define ACCESSIBLEPROPERTIES_H

#include <qobject.h>
#include <qstandarditemmodel.h>
#include "qaccessibilityclient/accessibleobject.h"

class ObjectProperties : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Role {
        NameRole,
        ValueRole
    };
    explicit ObjectProperties(QObject *parent = 0);
    virtual ~ObjectProperties();

    QHash<int,QByteArray> roleNames() const;
    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);
    QAccessibleClient::AccessibleObject currentObject() const { return m_acc; }

public slots:
    void doubleClicked(const QModelIndex &index);

private:
    QStandardItem* append(const QString &name, const QVariant &value = QVariant(), QStandardItem *parentItem = 0);
    QString stateString(const QAccessibleClient::AccessibleObject &acc);
    QAccessibleClient::AccessibleObject m_acc;
};

#endif
