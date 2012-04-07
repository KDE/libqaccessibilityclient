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
#include "accessibleobject_p.h"

#include "atspi/atspidbus.h"

#include <qstring.h>

using namespace KAccessibleClient;

AccessibleObject::AccessibleObject(AtSpiDBus *bus, const QString &service, const QString &path)
    :d(new AccessibleObjectPrivate(bus, service, path))
{
}

AccessibleObject::AccessibleObject(const AccessibleObject &other)
{
    d = new AccessibleObjectPrivate(other.d->bus, other.d->service, other.d->path);
}

AccessibleObject::~AccessibleObject()
{
}

bool AccessibleObject::isValid() const
{
    return (!d->path.isEmpty()) && (!d->service.isEmpty());
}

AccessibleObject &AccessibleObject::operator=(const AccessibleObject &other)
{
    d = other.d;
    return *this;
}

AccessibleObject AccessibleObject::parent() const
{
    return d->bus->parent(*this);
}

QList<AccessibleObject> AccessibleObject::children() const
{
    return d->bus->children(*this);
}

//int AccessibleObject::indexInParent() const
//{
//}

//int AccessibleObject::childCount() const
//{
//}

//AccessibleObject AccessibleObject::getChild(int index) const
//{
//}

QString AccessibleObject::name() const
{
    return d->name();
}

//QString AccessibleObject::localizedName() const
//{
//}

//QString AccessibleObject::description() const
//{
//}

//int AccessibleObject::role() const
//{
//}

//QString AccessibleObject::roleName() const
//{
//}

//QString AccessibleObject::localizedRoleName() const
//{
//}

