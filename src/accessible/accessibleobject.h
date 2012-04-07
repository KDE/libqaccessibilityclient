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
class AtSpiDBus;

/**
    This class represents an accessible object.

    It is implicitly shared and only created by the library.
*/
class LIBKDEACCESSIBILITYCLIENT_EXPORT AccessibleObject
{
public:
    AccessibleObject(const AccessibleObject &other);
    ~AccessibleObject();

    AccessibleObject &operator=(const AccessibleObject &other);
    bool operator==(const AccessibleObject &other) const;

    bool isValid() const;

    AccessibleObject parent() const;
    int indexInParent() const;

    QList<AccessibleObject> children() const;
    int childCount() const;
    AccessibleObject child(int index) const;

    QString name() const;
    QString localizedName() const;
    QString description() const;

    int role() const;
    QString roleName() const;
    QString localizedRoleName() const;

private:
    AccessibleObject(AtSpiDBus *bus, const QString &service, const QString &path);
    QSharedDataPointer<AccessibleObjectPrivate> d;

    friend class RegistryPrivate;
    friend class AtSpiDBus;
};


}

#endif
