/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef QACCESSIBILITYCLIENT_REGISTRY_H
#define QACCESSIBILITYCLIENT_REGISTRY_H

#include <QObject>

#include "qaccessibilityclient_export.h"
#include "accessibleobject.h"
#include <QUrl>

#define accessibleRegistry (QAccessibleClient::Registry::instance())

namespace QAccessibleClient {

class RegistryPrivate;
class RegistryPrivateCacheApi;

/*!
    \inmodule QAccessibilityClient
    \class QAccessibleClient::Registry
    \brief This class represents the global accessibility registry.

    It provides information about running applications.
    All updates of accessible objects will result in signals emitted by this class.
*/
class QACCESSIBILITYCLIENT_EXPORT Registry : public QObject
{
    Q_OBJECT

public:

    /*!
     * \enum QAccessibleClient::Registry::EventListener
     * This enum describes the different types of events that can be observed.
     * \value NoEventListeners
     *        No event listeners registered or wanted.
     * \value Window
     *        Window changes, such as new applications being started.
     * \value Focus
     *        Focus listener reacts to focus changes. See signal focusChanged.
     * \value StateChanged
     *        State of the accessible changed. See signal stateChanged.
     * \value ChildrenChanged
     *        Children changed. See signal childrenChanged.
     * \value VisibleDataChanged
     *        Visibility of the accessible changed. See signal visibleDataChanged.
     * \value SelectionChanged
     *        Selection changed. See signal selectionChanged.
     * \value ModelChanged
     *        The model changed. See signal modelChanged.
     * \value TextCaretMoved
     *        The text caret moved its position. See signal textCaretMoved.
     * \value TextChanged
     *        The text changed. See signal textChanged.
     * \value TextSelectionChanged
     *        The text selection changed. See signal textSelectionChanged.
     * \value PropertyChanged
     *        A property changed. See signals accessibleNameChanged and accessibleDescriptionChanged.
     * \value AllEventListeners
     *        All possible event listeners.
     */
    enum EventListener {
        NoEventListeners = 0x0,
        Window = 0x1,
        Focus = 0x2,
        //FocusPoint = 0x4,

        //BoundsChanged = 0x8,
        //LinkSelected = 0x10,
        StateChanged = 0x20,
        ChildrenChanged = 0x40,
        VisibleDataChanged = 0x80,
        SelectionChanged = 0x100,
        ModelChanged = 0x200,

        TextCaretMoved = 0x400,
        TextChanged = 0x800,
        TextSelectionChanged = 0x1000,
        PropertyChanged = 0x2000,
        //TextBoundsChanged = 0x2000,
        //TextAttributesChanged = 0x4000,
        //AttributesChanged = 0x8000,

        AllEventListeners = 0xffffffff
    };
    Q_ENUM(EventListener)
    Q_DECLARE_FLAGS(EventListeners, EventListener)

    /*!
      Construct a Registry object with \a parent as QObject parent.
     */
    explicit Registry(QObject *parent = nullptr);
    /*!
      Destroys this Registry.
     */
    ~Registry() override;

public Q_SLOTS:

    /*!
        Returns \c true if the accessibility stack is enabled.

        This means that an atspi registry daemon is running and available as
        org.a11y.bus at the session D-Bus.

        The org.a11y.bus implements the
        org.a11y.Status dbus interface that offers the IsEnabled property.

        The isEnabled and setEnabled methods do read/write the boolean value
        of that org.a11y.Status.IsEnabled D-Bus property.
    */
    bool isEnabled() const;
    /*!
      Activates accessibility with paremeter \a enable and tells
      toolkits to provide updates such as focus changes.
     */
    void setEnabled(bool enable);

    /*!
        Returns \c true if the screen reader is enabled.

        This means that there is potentially a screen reader, if installed,
        that is enabled or disabled. This allows to enable system wide a
        screen reader with just one switch.
    */
    bool isScreenReaderEnabled() const;
    /*!
      Activates the screen reader with parameter \a enable.
     */
    void setScreenReaderEnabled(bool enable);

    /*!
        In order to get notified of changes in accessible applications
        it is neccessary to subscribe to the listeners that are relevant.

        This will unsubscribe all previously subscribed event \a listeners.
    */
    void subscribeEventListeners(const EventListeners &listeners) const;
    /*!
      Returns the active event listeners.
     */
    EventListeners subscribedEventListeners() const;

    /*!
        List of all currently running applications that
        expose an accessibility interface.
    */
    QList<AccessibleObject> applications() const;

    /*!
        Creates the AccessibleObject for the \a url.

        This can be used to deserialize an AccessibleObject
        after passing it around as string.

        The returned QUrl returns a scheme of "accessibleobject".
    */
    AccessibleObject accessibleFromUrl(const QUrl &url) const;

Q_SIGNALS:

    /*!
        Emitted if the isEnabled state changed.

        If the accessibility daemon got \a enabled or disabled either by us or
        someone else then this signal is emitted.
    */
    void enabledChanged(bool enabled);

    /*!
        Emitted if the isScreenReaderEnabled state changed.

        If the screen reader enabled property got \a enabled or disabled either
        by us or by someone else then this signal is emitted.
    */
    void screenReaderEnabledChanged(bool enabled);

    /*!
        Emitted if an AccessibleObject \a object is created.

        This signal indicates that the AccessibleObject instance was
        created and is going to be added/attached.
    */
    void added(const QAccessibleClient::AccessibleObject &object);

    /*!
        Emitted if an AccessibleObject \a object is destroyed.

        This signal indicates that the AccessibleObject instance was
        destroyed and ended its life-time.
    */
    void removed(const QAccessibleClient::AccessibleObject &object);

    /*!
        Emitted if an AccessibleObject \a object is marked defunct.

        This signal indicates that the AccessibleObject became invalid
        and does not point any longer to any valid accessible object.
    */
    void defunct(const QAccessibleClient::AccessibleObject &object);

    /*! Emitted when a window is created */
    void windowCreated(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is destroyed */
    void windowDestroyed(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is closed */
    void windowClosed(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is reparented */
    void windowReparented(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is minimized */
    void windowMinimized(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is maximized */
    void windowMaximized(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is restored to normal size */
    void windowRestored(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is activated */
    void windowActivated(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is deactivated */
    void windowDeactivated(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a desktop window is created */
    void windowDesktopCreated(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a desktop window is destroyed */
    void windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is raised */
    void windowRaised(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is lowered */
    void windowLowered(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is moved */
    void windowMoved(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is resized */
    void windowResized(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is shaded */
    void windowShaded(const QAccessibleClient::AccessibleObject &object);
    /*! Emitted when a window is unshaded */
    void windowUnshaded(const QAccessibleClient::AccessibleObject &object);

    //void boundsChanged(const QAccessibleClient::AccessibleObject &object);
    //void linkSelected(const QAccessibleClient::AccessibleObject &object);

    /*!
        \brief Notifies about a state change in an object.

        The \a state of the given \a object has changed.
        If the state is now set \a active is \c true, otherwise the state was removed.
     */
    void stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active);

    /*!
        \brief Notifies about a new AccessibleObject.

        The childAdded signal is emitted when a new accessible object is created.
        This signal depends on the implementation of the server side and is not
        reliable for all applications.
        The parameter \a childIndex is the index of the child that has been added.
        \sa AccessibleObject::child(), childRemoved()
     */
    void childAdded(const QAccessibleClient::AccessibleObject &parent, int childIndex);

    /*!
        \brief Notifies that an AccessibleObject has been removed.

        The parameter \a childIndex is the index of the child that has been removed.
        \sa AccessibleObject::child(), childAdded()
     */
    void childRemoved(const QAccessibleClient::AccessibleObject &parent, int childIndex);

    /*!
      \brief Notifies that the \a object's visible data changed.
     */
    void visibleDataChanged(const QAccessibleClient::AccessibleObject &object);

    /*!
      \brief Notifies that the \a object's selection changed.
     */
    void selectionChanged(const QAccessibleClient::AccessibleObject &object);

    /*!
      \brief Notifies that the \a object's table model changed.
     */
    void modelChanged(const QAccessibleClient::AccessibleObject &object);

    /*!
        \brief Emitted when the focus changed.

        When subscribed to the Focus EventListener then this signal is emitted
        every time the focus changes.

        The \a object is the newly focused AccessibleObject.
    */
    void focusChanged(const QAccessibleClient::AccessibleObject &object);

    /*!
        \brief Emitted when the text caret moved.

        When subscribed to the TextCaretMoved EventListener then this signal
        is emitted every time the caret in an accessible \a object that implements
        a text-interface (like QLineEdit, QTextArea and QComboBox) moved to
        another position \a pos.
    */
    void textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos);

    /*!
        \brief Emitted when the text selection changed.

        When subscribed to the TextSelectionChanged EventListener then this signal
        is emitted every time the selection in an accessible \a object that implements
        a text-interface (like QLineEdit, QTextArea and QComboBox) changed.
    */
    void textSelectionChanged(const QAccessibleClient::AccessibleObject &object);


    /*!
        \brief Emitted when an accessible name changes.

        When subscribed to the PropertyChanged EventListener, this signal is emitted
        whenever the accessible-name property of \a object changes its value.
    */
    void accessibleNameChanged(const QAccessibleClient::AccessibleObject &object);

    /*!
        \brief Emitted when an accessible description changes

        When subscribed to the PropertyChanged EventListener, this signal is emitted
        whenever the accessible-description property of \a property changes its value.
    */
    void accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object);

    /*!
        \brief Emitted when an object's text was changed.

        The new text will be \a text. The change starts at \a startOffset and goes to \a endOffset.

        \sa textInserted, textRemoved
    */
    void textChanged(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset);

    /*!
        \brief Emitted when text was inserted in an object's text.

        The new text will be \a text. The change starts at \a startOffset and goes to \a endOffset.

        \sa textInserted, textRemoved
    */
    void textInserted(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset);

    /*!
        \brief Emitted when an object's text was removed.

        The removed text will be \a text, but this string may be emtpy for some implementations.
        The removal starts at \a startOffset and goes to \a endOffset.

        \sa textInserted, textRemoved
    */
    void textRemoved(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset);

    //void textBoundsChanged(const QAccessibleClient::AccessibleObject &object);
    //void textAttributesChanged(const QAccessibleClient::AccessibleObject &object);
    //void attributesChanged(const QAccessibleClient::AccessibleObject &object);

private:
    Q_DISABLE_COPY(Registry)
    RegistryPrivate *d;
    friend class RegistryPrivate;
    friend class RegistryPrivateCacheApi;

    enum CacheType { NoCache, WeakCache};
    QACCESSIBILITYCLIENT_NO_EXPORT CacheType cacheType() const;
    QACCESSIBILITYCLIENT_NO_EXPORT void setCacheType(CacheType type);
    QACCESSIBILITYCLIENT_NO_EXPORT AccessibleObject clientCacheObject(const QString &id) const;
    QACCESSIBILITYCLIENT_NO_EXPORT QStringList clientCacheObjects() const;
    QACCESSIBILITYCLIENT_NO_EXPORT void clearClientCache();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Registry::EventListeners)

}

#endif
