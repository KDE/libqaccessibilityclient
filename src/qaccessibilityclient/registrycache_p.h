/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef QACCESSIBILITYCLIENT_REGISTRYCACHE_P_H
#define QACCESSIBILITYCLIENT_REGISTRYCACHE_P_H

#include "qaccessibilityclient_export.h"
#include "accessibleobject.h"

namespace QAccessibleClient {

class Registry;

// Private API. May be gone or changed anytime soon.
class QACCESSIBILITYCLIENT_EXPORT RegistryPrivateCacheApi
{
public:
    enum CacheType {
        NoCache, ///< Disable any caching.
        WeakCache, ///< Cache only objects in use and free them as long as no-one holds a reference to them any longer.
    };

    explicit RegistryPrivateCacheApi(Registry *registry);

    CacheType cacheType() const;
    void setCacheType(CacheType type);

    AccessibleObject clientCacheObject(const QString &id) const;
    QStringList clientCacheObjects() const;
    void clearClientCache();

private:
    Registry *const m_registry;
};

}

#endif
