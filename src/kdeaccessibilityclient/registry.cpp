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

using namespace KAccessibleClient;

Registry::Registry(QObject *parent)
    : QObject(parent), d(new RegistryPrivate(this))
{
    registerDBusTypes();
    d->init();
}

Registry::~Registry()
{
    delete d;
}

void Registry::subscribeEventListeners(const EventListeners &listeners) const
{
    d->subscribeEventListeners(listeners);
    bool s = connect(d, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)), this, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)));
    Q_ASSERT(s);
}

Registry::EventListeners Registry::subscribedEventListeners() const
{
    return d->eventListeners();
}

QList<AccessibleObject> Registry::applications() const
{
    return d->topLevelAccessibles();
}

#include "registry.moc"
