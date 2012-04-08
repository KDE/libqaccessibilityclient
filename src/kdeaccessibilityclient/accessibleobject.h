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
        \brief Returns true if AccessibleObject is valid.

        Invalid objects are for example returned when asking for the
        parent of the top most item, or for a child that is out of range.
     */
    bool isValid() const;

    /**
        \brief Returns the parent AccessibleObject.
        \return The parent AccessibleObject
     */
    AccessibleObject parent() const;

    /**
        \brief Returns this accessible's index in it's parent's child list.
        \return index
     */
    int indexInParent() const;

    /**
        \brief Returns this accessible's children in a list.
        \return children
     */
    QList<AccessibleObject> children() const;

    /**
        \brief Returns the number of children for this AccessibleObject.
        \return number of children
     */
    int childCount() const;

    /**
        \brief Returns a specific child at \a index position.

        The list of children is 0-based.
        \return number of children
     */
    AccessibleObject child(int index) const;

    /**
        \brief Returns the name of this AccessibleObject.

        The name is a short descriptive one or two words.
        It is localized.
     */
    QString name() const;

    /**
        \brief Returns the description for this AccessibleObject.

        The description is more of an explanation than the name.
        This can be a sentence. The string is localized.
     */
    QString description() const;

    /**
        \brief Returns the role as integer value of this AccessibleObject.
        FIXME: this is currently the ATSPI_ROLE_xxx constant.
        Either include the atspi-constants.h or figure out what else to do with this.
     */
    int role() const;

    /**
        \brief Returns the name of the role of this AccessibleObject.

        This name is not localized to allow tools to work with the english string.
     */
    QString roleName() const;

    /**
        \brief Returns the name of the role of this AccessibleObject.

        This name is localized and can be presented to the user.
     */
    QString localizedRoleName() const;

private:
    AccessibleObject(RegistryPrivate *reg, const QString &service, const QString &path);
    QSharedDataPointer<AccessibleObjectPrivate> d;

    friend class RegistryPrivate;
    friend class AtSpiDBus;
};


}

#endif
