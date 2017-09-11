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
