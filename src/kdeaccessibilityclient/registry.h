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
    All updates of accessible objects will result in signals emitted by this class.
*/
class LIBKDEACCESSIBILITYCLIENT_EXPORT Registry :public QObject
{
    Q_OBJECT

public:
    enum EventListener {
        NoEventListeners = 0x0,           /*!< No event listeners registered or wanted */
        Focus = 0x1,                      /*!< Focus listener reacts to focus changes - see signal \sa focusChanged */
        Object = 0x2,                     /*!< Object changes, such as checked state etc. */
        // FIXME many more detailed event listeners are possible
        Window = 0x4,                     /*!< Window changes, such as new applications being started */

        AllEventListeners = 0xffff        /*!< All possible event listeners */
    };
    Q_DECLARE_FLAGS(EventListeners, EventListener)


    Registry(QObject *parent = 0);
    ~Registry();

    /**
        In order to get notified of changes in accessible applications
        it is neccessary to subscribe to the listeners that are relevant.
    */
    void subscribeEventListeners(const EventListeners &listeners) const;
    EventListeners subscribedEventListeners() const;

    /**
        List of all currently running applications that
        expose and accessibility interface.
    */
    QList<AccessibleObject> applications() const;

Q_SIGNALS:
    /**
        \brief Emitted when subscribed to Focus EventListener.
    */
    void focusChanged(const KAccessibleClient::AccessibleObject &);

private:
    Q_DISABLE_COPY(Registry)
    RegistryPrivate *d;
    friend class RegistryPrivate;
};

}

#endif
