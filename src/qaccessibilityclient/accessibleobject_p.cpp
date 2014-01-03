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

#include "accessibleobject_p.h"
#include "registry_p.h"

#include <qaction.h>
#include <qdebug.h>

using namespace QAccessibleClient;

AccessibleObjectPrivate::AccessibleObjectPrivate(RegistryPrivate *reg, const QString &service_, const QString &path_)
    : registryPrivate(reg)
    , service(service_)
    , path(path_)
    , defunct(false)
    , actionsFetched(false)
{
    //qDebug() << Q_FUNC_INFO;
}

AccessibleObjectPrivate::~AccessibleObjectPrivate()
{
    //qDebug() << Q_FUNC_INFO;

    if (registryPrivate->m_cache) {
        const QString id = path + service;
        registryPrivate->m_cache->remove(id);
    }
}

bool AccessibleObjectPrivate::operator==(const AccessibleObjectPrivate &other) const
{
    return registryPrivate == other.registryPrivate &&
            service == other.service &&
            path == other.path;
}

void AccessibleObjectPrivate::setDefunct()
{
    defunct = true;

    for(int i = 0; i < actions.count(); ++i) {
        const QSharedPointer<QAction> &action = actions[i];
        action->setEnabled(false);
    }
}
