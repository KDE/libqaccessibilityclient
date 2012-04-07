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

#include "accessibleobject.h"

#include <qstring.h>
#include <qdebug.h>

#include "accessibleobject_p.h"
#include "registry_p.h"

using namespace KAccessibleClient;

AccessibleObject::AccessibleObject(RegistryPrivate *registryPrivate, const QString &service, const QString &path)
    :d(new AccessibleObjectPrivate(registryPrivate, service, path))
{
}

AccessibleObject::AccessibleObject(const AccessibleObject &other)
    :d(other.d)
{
}

AccessibleObject::~AccessibleObject()
{
}

bool AccessibleObject::isValid() const
{
    return d->registryPrivate
            && (!d->service.isEmpty())
            && (!d->path.isEmpty())
            && (d->path != QLatin1String("/org/a11y/atspi/null"));
}

AccessibleObject &AccessibleObject::operator=(const AccessibleObject &other)
{
    d = other.d;
    return *this;
}

bool AccessibleObject::operator==(const AccessibleObject &other) const
{
    return (d == other.d) || *d == *other.d;
}

AccessibleObject AccessibleObject::parent() const
{
    return d->registryPrivate->parentAccessible(*this);
}

QList<AccessibleObject> AccessibleObject::children() const
{
    return d->registryPrivate->children(*this);
}

int AccessibleObject::childCount() const
{
    return d->registryPrivate->childCount(*this);
}

AccessibleObject AccessibleObject::child(int index) const
{
    return d->registryPrivate->child(*this, index);
}

//int AccessibleObject::indexInParent() const
//{
//}

QString AccessibleObject::name() const
{
    return d->registryPrivate->name(*this);
}

QString AccessibleObject::description() const
{
    return d->registryPrivate->description(*this);
}

int AccessibleObject::role() const
{
    return d->registryPrivate->role(*this);
}

QString AccessibleObject::roleName() const
{
    return d->registryPrivate->roleName(*this);
}

QString AccessibleObject::localizedRoleName() const
{
    return d->registryPrivate->localizedRoleName(*this);
}

