// SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "registrycache_p.h"
#include "registry.h"

using namespace QAccessibleClient;

RegistryPrivateCacheApi::RegistryPrivateCacheApi(Registry *registry)
    : m_registry(registry)
{}

RegistryPrivateCacheApi::CacheType RegistryPrivateCacheApi::cacheType() const
{
    return static_cast<CacheType>(m_registry->cacheType());
}

void RegistryPrivateCacheApi::setCacheType(CacheType type)
{
    m_registry->setCacheType(static_cast<Registry::CacheType>(type));
}

AccessibleObject RegistryPrivateCacheApi::clientCacheObject(const QString &id) const
{
    return m_registry->clientCacheObject(id);
}

QStringList RegistryPrivateCacheApi::clientCacheObjects() const
{
    return m_registry->clientCacheObjects();
}
void RegistryPrivateCacheApi::clearClientCache()
{
    m_registry->clearClientCache();
}
