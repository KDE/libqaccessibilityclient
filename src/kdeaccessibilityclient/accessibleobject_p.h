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

#ifndef LIBKDEACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_P_H
#define LIBKDEACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_P_H

#include <qstring.h>
#include <qdbusargument.h>

namespace KAccessibleClient {

class RegistryPrivate;

class AccessibleObjectPrivate :public QSharedData
{
public:
    AccessibleObjectPrivate(RegistryPrivate *reg, const QString &service_, const QString &path_)
        : QSharedData()
        , registryPrivate(reg)
        , service(service_)
        , path(path_)
        , actionsFetched(false)
    {
    }
    AccessibleObjectPrivate(const AccessibleObjectPrivate &other)
        : QSharedData(other)
        , registryPrivate(other.registryPrivate)
        , service(other.service)
        , path(other.path)
        , actionsFetched(false)
    {
    }
    ~AccessibleObjectPrivate()
    {
        qDeleteAll(actions);
    }

    RegistryPrivate *registryPrivate;
    QString service;
    QString path;

    mutable QList<QAction*> actions;
    mutable bool actionsFetched;

    bool operator==(const AccessibleObjectPrivate &other) const
    {
        return registryPrivate == other.registryPrivate &&
               service == other.service &&
               path == other.path;
    }
};

}

#endif
