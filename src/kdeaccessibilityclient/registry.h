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
class LIBKDEACCESSIBILITYCLIENT_EXPORT Registry : public QObject
{
    Q_OBJECT
    Q_ENUMS(EventListener)

public:

    enum EventListener {
        NoEventListeners = 0x0,             /*!< No event listeners registered or wanted */
        Window = 0x1,                       /*!< Window changes, such as new applications being started */
        Focus = 0x2,                        /*!< Focus listener reacts to focus changes - see signal \sa focusChanged */
        //FocusPoint = 0x4,

        //BoundsChanged = 0x8,
        //LinkSelected = 0x10,
        StateChanged = 0x20,                /*!< State of the accessible changed - see signal \sa stateChanged */
        ChildrenChanged = 0x40,             /*!< Children changed - see signal \sa childrenChanged */
        VisibleDataChanged = 0x80,          /*!< Visibility of the accessible changed - see signal \sa visibleDataChanged */
        SelectionChanged = 0x100,           /*!< Selection changed  - see signal \sa selectionChanged */
        ModelChanged = 0x200,               /*!< The model changed - see signal \sa modelChanged */

        TextCaretMoved = 0x400,             /*!< The text caret moved its position - see signal \sa textCaretMoved */
        TextChanged = 0x800,                /*!< The text changed - see signal \sa textChanged */
        TextSelectionChanged = 0x1000,      /*!< The text selection changed - see signal \sa textSelectionChanged */
        //TextBoundsChanged = 0x2000,
        //TextAttributesChanged = 0x4000,
        //AttributesChanged = 0x8000,

        AllEventListeners = 0xffffffff      /*!< All possible event listeners */
    };
    Q_DECLARE_FLAGS(EventListeners, EventListener)

    Registry(QObject *parent = 0);
    ~Registry();

public slots:

    /**
        Returns true if the accessibility stack is enabled.

        This means that an atspi registry daemon is running and available as
        org.a11y.bus at the session dbus. The org.a11y.bus implments the
        org.a11y.Status dbus interface that offers the IsEnabled property. The
        \a isEnabled and \a setEnabled methods do read/write the boolean value
        of that org.a11y.Status.IsEnabled dbus property..
    */
    bool isEnabled() const;
    void setEnabled(bool enable);

    /**
        In order to get notified of changes in accessible applications
        it is neccessary to subscribe to the listeners that are relevant.
    */
    void subscribeEventListeners(const EventListeners &listeners) const;
    EventListeners subscribedEventListeners() const;

    /**
        List of all currently running applications that
        expose an accessibility interface.
    */
    QList<AccessibleObject> applications() const;

    /**
        Returns a QUrl that references the AccessibleObject.

        This can be used to serialize/unserialize an AccessibleObject
        to pass it around as string and restore the AccessibleObject
        later on.

        The returned QUrl returns a scheme of "AccessibleObject", the
        dbus path as url path and the dbus service as url fragment.
    */
    QUrl toUrl(const AccessibleObject &object) const;
    AccessibleObject fromUrl(const QUrl &url) const;

Q_SIGNALS:

    /**
        Emitted if the \a isEnabled state changed.

        If the accessibility daemon got enabled or disabled either by us or
        ny someone else then this signal is emitted.
    */
    void enabledChanged(bool enabled);

    void windowCreated(const KAccessibleClient::AccessibleObject &object);
    void windowDestroyed(const KAccessibleClient::AccessibleObject &object);
    void windowClosed(const KAccessibleClient::AccessibleObject &object);
    void windowReparented(const KAccessibleClient::AccessibleObject &object);
    void windowMinimized(const KAccessibleClient::AccessibleObject &object);
    void windowMaximized(const KAccessibleClient::AccessibleObject &object);
    void windowRestored(const KAccessibleClient::AccessibleObject &object);
    void windowActivated(const KAccessibleClient::AccessibleObject &object);
    void windowDeactivated(const KAccessibleClient::AccessibleObject &object);
    void windowDesktopCreated(const KAccessibleClient::AccessibleObject &object);
    void windowDesktopDestroyed(const KAccessibleClient::AccessibleObject &object);
    void windowRaised(const KAccessibleClient::AccessibleObject &object);
    void windowLowered(const KAccessibleClient::AccessibleObject &object);
    void windowMoved(const KAccessibleClient::AccessibleObject &object);
    void windowResized(const KAccessibleClient::AccessibleObject &object);
    void windowShaded(const KAccessibleClient::AccessibleObject &object);
    void windowUnshaded(const KAccessibleClient::AccessibleObject &object);

    //void propertyChanged(const KAccessibleClient::AccessibleObject &object);
    //void boundsChanged(const KAccessibleClient::AccessibleObject &object);
    //oid linkSelected(const KAccessibleClient::AccessibleObject &object);
    void stateChanged(const KAccessibleClient::AccessibleObject &object, const QString &state, int detail1, int detail2, const QVariant &args);
    void childrenChanged(const KAccessibleClient::AccessibleObject &object);
    void visibleDataChanged(const KAccessibleClient::AccessibleObject &object);
    void selectionChanged(const KAccessibleClient::AccessibleObject &object);
    void modelChanged(const KAccessibleClient::AccessibleObject &object);

    /**
        \brief Emitted when the focus changed.

        When subscribed to the Focus EventListener then this signal is emitted
        every time the focus changes.
    */
    void focusChanged(const KAccessibleClient::AccessibleObject &);

    /**
        \brief Emitted when the text cared moved.

        When subscribed to the TextCaretMoved EventListener then this signal
        is emitted every time the caret in an accessible object that implements
        a text-interface (like QLineEdit, QTextArea and QComboBox) moved to
        another position.
    */
    void textCaretMoved(const KAccessibleClient::AccessibleObject &object, int pos);

    /**
        \brief Emitted when the text selection changed.

        When subscribed to the TextSelectionChanged EventListener then this signal
        is emitted every time the selection in an accessible object that implements
        a text-interface (like QLineEdit, QTextArea and QComboBox) changed.
    */
    void textSelectionChanged(const KAccessibleClient::AccessibleObject &object);

    //void textBoundsChanged(const KAccessibleClient::AccessibleObject &object);
    void textChanged(const KAccessibleClient::AccessibleObject &object);
    //void textAttributesChanged(const KAccessibleClient::AccessibleObject &object);
    //void attributesChanged(const KAccessibleClient::AccessibleObject &object);

private:
    Q_DISABLE_COPY(Registry)
    RegistryPrivate *d;
    friend class RegistryPrivate;
};

}

#endif
