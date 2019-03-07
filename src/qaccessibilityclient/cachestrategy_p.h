/*
    Copyright 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

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

#ifndef QACCESSIBILITYCLIENT_CACHESTRATEGY_P_H
#define QACCESSIBILITYCLIENT_CACHESTRATEGY_P_H

#include "accessibleobject.h"

#include <QPair>

namespace QAccessibleClient {

class ObjectCache
{
public:
    virtual QStringList ids() const = 0;
    virtual QSharedPointer<AccessibleObjectPrivate> get(const QString &id) const = 0;
    virtual void add(const QString &id, const QSharedPointer<AccessibleObjectPrivate> &objectPrivate) = 0;
    virtual bool remove(const QString &id) = 0;
    virtual void clear() = 0;
    virtual AccessibleObject::Interfaces interfaces(const AccessibleObject &object) = 0;
    virtual void setInterfaces(const AccessibleObject &object, AccessibleObject::Interfaces interfaces) = 0;
    virtual quint64 state(const AccessibleObject &object) = 0;
    virtual void setState(const AccessibleObject &object, quint64 state) = 0;
    virtual void cleanState(const AccessibleObject &object) = 0;
    virtual ~ObjectCache() {}
    static const quint64 StateNotFound = ~0;
};

class CacheWeakStrategy : public ObjectCache
{
public:
    QStringList ids() const override
    {
        return accessibleObjectsHash.keys();
    }
    QSharedPointer<AccessibleObjectPrivate> get(const QString &id) const override
    {
        return accessibleObjectsHash[id].first;
    }
    void add(const QString &id, const QSharedPointer<AccessibleObjectPrivate> &objectPrivate) override
    {
        accessibleObjectsHash[id] = QPair<QWeakPointer<AccessibleObjectPrivate>, AccessibleObjectPrivate*>(objectPrivate, objectPrivate.data());
    }
    bool remove(const QString &id) override
    {
        QPair<QWeakPointer<AccessibleObjectPrivate>, AccessibleObjectPrivate*> data = accessibleObjectsHash.take(id);
        return (interfaceHash.remove(data.second) >= 1) || (stateHash.remove(data.second) >= 1);
    }
    void clear() override
    {
        accessibleObjectsHash.clear();
        stateHash.clear();
        interfaceHash.clear();
    }
    AccessibleObject::Interfaces interfaces(const AccessibleObject &object) override
    {
        if (!interfaceHash.contains(object.d.data()))
            return AccessibleObject::InvalidInterface;
        return interfaceHash.value(object.d.data());
    }
    void setInterfaces(const AccessibleObject &object, AccessibleObject::Interfaces interfaces) override
    {
        interfaceHash.insert(object.d.data(), interfaces);
    }
    quint64 state(const AccessibleObject &object) override
    {
        if (!stateHash.contains(object.d.data()))
            return ObjectCache::StateNotFound;

        return stateHash.value(object.d.data());
    }
    void setState(const AccessibleObject &object, quint64 state) override
    {
        stateHash[object.d.data()] = state;
    }
    void cleanState(const AccessibleObject &object) override
    {
        stateHash.remove(object.d.data());
    }

private:
    QHash<QString, QPair<QWeakPointer<AccessibleObjectPrivate>, AccessibleObjectPrivate*> > accessibleObjectsHash;
    QHash<AccessibleObjectPrivate*, AccessibleObject::Interfaces> interfaceHash;
    QHash<AccessibleObjectPrivate*, qint64> stateHash;
};

}

#endif

