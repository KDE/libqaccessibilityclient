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

#include "registry.h"
#include "registry_p.h"

#include <qurl.h>

using namespace KAccessibleClient;

Registry::Registry(QObject *parent)
    : QObject(parent), d(new RegistryPrivate(this))
{
    registerDBusTypes();
}

Registry::~Registry()
{
    delete d;
}

bool Registry::isEnabled() const
{
    return d->isEnabled();
}

void Registry::setEnabled(bool enable)
{
    d->setEnabled(enable);
}

void Registry::subscribeEventListeners(const EventListeners &listeners) const
{
    d->subscribeEventListeners(listeners);
}

Registry::EventListeners Registry::subscribedEventListeners() const
{
    return d->eventListeners();
}

QList<AccessibleObject> Registry::applications() const
{
    return d->topLevelAccessibles();
}

QUrl Registry::url(const AccessibleObject &object) const
{
    return d->url(object);
}

AccessibleObject Registry::fromUrl(const QUrl &url) const
{
    return d->fromUrl(url);
}

AccessibleObject Registry::clientCacheObject(const QString &id) const
{
    RegistryPrivate::AccessibleObjectsHashConstIterator it = d->accessibleObjectsHash.constFind(id);
    if (it == d->accessibleObjectsHash.constEnd() || !it.value())
        return AccessibleObject();
    return AccessibleObject(it.value()->registryPrivate, it.value()->service, it.value()->path);
}

QList<AccessibleObject> Registry::clientCacheObjects() const
{
    QList<AccessibleObject> result;
    RegistryPrivate::AccessibleObjectsHashConstIterator it(d->accessibleObjectsHash.constBegin()), end(d->accessibleObjectsHash.constEnd());
    for(; it != end; ++it)
        if (it.value())
            result.append(AccessibleObject(it.value()->registryPrivate, it.value()->service, it.value()->path));
    return result;
}

int Registry::clientCacheObjectsCount() const
{
    return d->accessibleObjectsHash.count();
}

void Registry::clearClientCache()
{
    d->accessibleObjectsHash.clear();
}

#include "registry.moc"
