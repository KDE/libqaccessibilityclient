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

#ifndef LIBKDEACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_H
#define LIBKDEACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_H

#include <qlist.h>
#include <qshareddata.h>

#include <atspi/atspi-constants.h>

#include "libkdeaccessibilityclient_export.h"

namespace KAccessibleClient {

class AccessibleObjectPrivate;
class RegistryPrivate;

/**
    This class represents an accessible object.

    It is implicitly shared and only created by the library.
*/
class LIBKDEACCESSIBILITYCLIENT_EXPORT AccessibleObject
{
public:
    /**
        \brief Copy constructor.
     */
    AccessibleObject(const AccessibleObject &other);
    ~AccessibleObject();
    AccessibleObject &operator=(const AccessibleObject &other);
    bool operator==(const AccessibleObject &other) const;

    /**
        \brief Returns true if this object is valid.

        Invalid objects are for example returned when asking for the
        parent of the top most item, or for a child that is out of range.
     */
    bool isValid() const;

    /**
        \brief Returns this object's parent.
        \return The parent AccessibleObject
     */
    AccessibleObject parent() const;

    /**
        \brief Returns this accessible's index in it's parent's list of children.
        \return index
     */
    int indexInParent() const;

    /**
        \brief Returns this accessible's children in a list.
        \return children
     */
    QList<AccessibleObject> children() const;

    /**
        \brief Returns the number of children for this accessible.
        \return number of children
     */
    int childCount() const;

    /**
        \brief Returns a specific child at position \a index.

        The list of children is 0-based.
        \return number of children
     */
    AccessibleObject child(int index) const;

    /**
        \brief Returns the name of this accessible.

        The name is a short descriptive one or two words.
        It is localized.
     */
    QString name() const;

    /**
        \brief Returns the description for this accessible.

        The description is more of an explanation than the name.
        This can be a sentence. The string is localized.
     */
    QString description() const;

    /**
        \brief Returns the role as integer value of this accessible.
     */
    AtspiRole role() const;

    /**
        \brief Returns the name of the role of this accessible.

        This name is not localized to allow tools to work with the english string.
     */
    QString roleName() const;

    /**
        \brief Returns the name of the role of this accessible.

        This name is localized and can be presented to the user.
     */
    QString localizedRoleName() const;

    // states
    bool isActive() const;
    bool isCheckable() const;
    bool isChecked() const;
//    bool isDefunct() const;
    bool isEditable() const;
    bool isExpandable() const;
    bool isExpanded() const;
    bool isFocusable() const;
    bool isFocused() const;
    bool isMultiLine() const;
    bool isSelectable() const;
    bool isSelected() const;
    bool isSensitive() const;
    bool isSingleLine() const;

    /*
     * \internal
     * \brief isTransient marks an object as being unreliable in that it can quickly disappear or change
     *
     * This is mostly a hint that the object should not be cached.
     * \return true if the object is transient
     */
//    bool isTransient() const;

    bool isVisible() const;

    /*
     * \internal
     * \brief managesDescendants marks an object as being responsible for its children
     *
     * This is to notify that this object handles signals for it's children.
     * The property is typically used for tables and lists or other collection objects.
     * \return true if the object is transient
     */
//    bool managesDescendants() const;
//    bool isRequired() const;
//    bool isAnimated() const;
//    bool isInvalidEntry() const;
    bool isDefault() const;
//    bool isVisited() const;

    bool hasSelectableText() const;
    bool hasToolTip() const;
    bool supportsAutocompletion() const;

private:
    AccessibleObject(RegistryPrivate *reg, const QString &service, const QString &path);
    QSharedDataPointer<AccessibleObjectPrivate> d;

    friend class RegistryPrivate;
    friend class AtSpiDBus;
};


}

#endif
