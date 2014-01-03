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

using namespace QAccessibleClient;

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

bool Registry::isScreenReaderEnabled() const
{
    return d->isScreenReaderEnabled();
}

void Registry::setScreenReaderEnabled(bool enable)
{
    d->setScreenReaderEnabled(enable);
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

AccessibleObject Registry::accessibleFromUrl(const QUrl &url) const
{
    return d->fromUrl(url);
}

Registry::CacheType Registry::cacheType() const
{
    if (dynamic_cast<CacheWeakStrategy*>(d->m_cache))
        return WeakCache;
    if (dynamic_cast<CacheStrongStrategy*>(d->m_cache))
        return StrongCache;
    return NoCache;
}

void Registry::setCacheType(Registry::CacheType type)
{
    //if (cacheType() == type) return;
    delete d->m_cache;
    d->m_cache = 0;
    switch (type) {
        case NoCache:
            break;
        case WeakCache:
            d->m_cache = new CacheWeakStrategy();
            break;
        case StrongCache:
            d->m_cache = new CacheStrongStrategy();
            break;
    }
}

AccessibleObject Registry::clientCacheObject(const QString &id) const
{
    if (d->m_cache) {
        QSharedPointer<AccessibleObjectPrivate> p = d->m_cache->get(id);
        if (p)
            return AccessibleObject(p);
    }
    return AccessibleObject();
}

QStringList Registry::clientCacheObjects() const
{
    QStringList result;
    if (d->m_cache)
        return d->m_cache->ids();
    return QStringList();
}

void Registry::clearClientCache()
{
    if (d->m_cache)
        d->m_cache->clear();
}

#include "registry.moc"
