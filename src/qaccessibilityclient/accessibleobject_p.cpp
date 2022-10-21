/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "accessibleobject_p.h"
#include "registry_p.h"

#include <QDebug>

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
