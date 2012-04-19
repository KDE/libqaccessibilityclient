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

AccessibleObject::AccessibleObject()
    :d(0)
{
}

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
    return d && d->registryPrivate
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

int AccessibleObject::indexInParent() const
{
    return d->registryPrivate->indexInParent(*this);
}

QString AccessibleObject::name() const
{
    return d->registryPrivate->name(*this);
}

QString AccessibleObject::description() const
{
    return d->registryPrivate->description(*this);
}

AtspiRole AccessibleObject::role() const
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

bool AccessibleObject::hasSelectableText() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SELECTABLE_TEXT);
}

bool AccessibleObject::hasToolTip() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_HAS_TOOLTIP);
}

bool AccessibleObject::isActive() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_ACTIVE);
}

bool AccessibleObject::isCheckable() const
{
    // FIXME
    //return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_);
    qWarning() << "IMPLEMENT: AccessibleObject::isCheckable";
    return 0;
}

bool AccessibleObject::isChecked() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_CHECKED);
}

bool AccessibleObject::isDefault() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_IS_DEFAULT);
}

bool AccessibleObject::isEditable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_EDITABLE);
}

bool AccessibleObject::isExpandable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_EXPANDABLE);
}

bool AccessibleObject::isExpanded() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_EXPANDED);
}

bool AccessibleObject::isFocusable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_FOCUSABLE);
}

bool AccessibleObject::isFocused() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_FOCUSED);
}

bool AccessibleObject::isMultiLine() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_MULTI_LINE);
}

bool AccessibleObject::isSelectable() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SELECTABLE);
}

bool AccessibleObject::isSelected() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SELECTED);
}

bool AccessibleObject::isSensitive() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SENSITIVE);
}

bool AccessibleObject::isSingleLine() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SINGLE_LINE);
}

bool AccessibleObject::isVisible() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_VISIBLE);
}

bool AccessibleObject::supportsAutocompletion() const
{
    return d->registryPrivate->state(*this) & (quint64(1) << ATSPI_STATE_SUPPORTS_AUTOCOMPLETION);
}


