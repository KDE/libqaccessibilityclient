/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef QACCESSIBILITYCLIENT_REGISTRYCACHE_P_H
#define QACCESSIBILITYCLIENT_REGISTRYCACHE_P_H

namespace QAccessibleClient {

// Private API. May be gone or changed anytime soon.
class QACCESSIBILITYCLIENT_EXPORT RegistryPrivateCacheApi
{
public:
    enum CacheType {
        NoCache, ///< Disable any caching.
        WeakCache, ///< Cache only objects in use and free them as long as no-one holds a reference to them any longer.
    };

    explicit RegistryPrivateCacheApi(Registry *registry) : m_registry(registry) {}

    CacheType cacheType() const { return static_cast<CacheType>(m_registry->cacheType()); }
    void setCacheType(CacheType type) { m_registry->setCacheType(static_cast<Registry::CacheType>(type)); }

    AccessibleObject clientCacheObject(const QString &id) const { return m_registry->clientCacheObject(id); }
    QStringList clientCacheObjects() const { return m_registry->clientCacheObjects(); }
    void clearClientCache() { m_registry->clearClientCache(); }

private:
    Registry *m_registry;
};

}

#endif
