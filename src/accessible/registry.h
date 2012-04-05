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

#ifndef LIBKDEACCESSIBILITYCLIENT_REGISTRY_H
#define LIBKDEACCESSIBILITYCLIENT_REGISTRY_H

#include <qobject.h>

#include "libkdeaccessibilityclient_export.h"
#include "accessibleobject.h"

#define accessibleRegistry (KAccessibleClient::Registry::instance())

namespace KAccessibleClient {

class RegistryPrivate;

/**
    This class represents the global accessibility registry.

    It provides information about running applications.
*/
class LIBKDEACCESSIBILITYCLIENT_EXPORT Registry :public QObject
{
    Q_OBJECT

public:
    ~Registry();
    static Registry *instance();

    /**
        List of all currently running applications that
        expose and accessibility interface.
      */
    QList<AccessibleObject> applications();

    Registry();
private:
    Q_DISABLE_COPY(Registry)
    static Registry *self;
    RegistryPrivate *d;
};


}

#endif
