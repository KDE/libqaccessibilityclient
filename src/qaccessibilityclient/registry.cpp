/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    return NoCache;
}

void Registry::setCacheType(Registry::CacheType type)
{
    //if (cacheType() == type) return;
    delete d->m_cache;
    d->m_cache = nullptr;
    switch (type) {
        case NoCache:
            break;
        case WeakCache:
            d->m_cache = new CacheWeakStrategy();
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

#include "moc_registry.cpp"
