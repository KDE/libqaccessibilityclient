/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef QACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_H
#define QACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_H


namespace QAccessibleClient {
    class AccessibleObject;
}

#include <QList>
#include <QSharedPointer>
#include <QAction>

#include "qaccessibilityclient_export.h"

namespace QAccessibleClient {

class AccessibleObjectPrivate;
class RegistryPrivate;


#ifndef QT_NO_DEBUG_STREAM
    QACCESSIBILITYCLIENT_EXPORT QDebug operator<<(QDebug, const AccessibleObject &);
#endif

/*!
    \inmodule QAccessibilityClient
    \class QAccessibleClient::AccessibleObject
    \brief This class represents an accessible object.

    An accessible object equals usually a visible widget or some kind
    of other element the user can interact with but can also present
    a not visible object that offers certain functionality like for
    example actions which can be triggered.

    It is implicitly shared and only created by the library.
*/
class QACCESSIBILITYCLIENT_EXPORT AccessibleObject
{
public:

    /*!
      \enum QAccessibleClient::AccessibleObject::Interface
      \brief This enum describes the different interfaces that an
      AccessibleObject can implement.

      Each AccessibleObject must implement the AccessibleInterface, otherwise
      it is invalid. All other interfaces are optional.

      If the ActionInterface is implement the object
      will have a list of actions that can be invoked.

      \value NoInterface
      \value AccessibleInterface
      \value CacheInterface
      \value ActionInterface
      \value ApplicationInterface
      \value CollectionInterface
      \value ComponentInterface
      \value DocumentInterface
      \value EditableTextInterface
      \value EventKeyboardInterface
      \value EventMouseInterface
      \value EventObjectInterface
      \value HyperlinkInterface
      \value HypertextInterface
      \value ImageInterface
      \value SelectionInterface
      \value TableInterface
      \value TextInterface
      \value ValueInterface
      \value SocketInterface
      \value EventWindowInterface
      \value EventFocusInterface
      \value InvalidInterface
    */
    enum Interface {
        NoInterface = 0x0,
        AccessibleInterface = 0x1,
        CacheInterface = 0x2,
        ActionInterface = 0x4,
        ApplicationInterface = 0x8,
        CollectionInterface = 0x10,
        ComponentInterface = 0x20,
        DocumentInterface = 0x40,
        EditableTextInterface = 0x80,
        EventKeyboardInterface = 0x100,
        EventMouseInterface = 0x200,
        EventObjectInterface = 0x400,
        HyperlinkInterface = 0x800,
        HypertextInterface = 0x1000,
        ImageInterface = 0x2000,
        SelectionInterface = 0x4000,
        TableInterface = 0x8000,
        TextInterface = 0x10000,
        ValueInterface = 0x20000,
        SocketInterface = 0x40000,
        EventWindowInterface = 0x80000,
        EventFocusInterface = 0x100000,

        InvalidInterface = 0x80000000
    };
    Q_DECLARE_FLAGS(Interfaces, Interface)

    /*!
      \enum QAccessibleClient::AccessibleObject::Role
      The role indicates the type of UI element that an AccessibleObject
      represents.

      \value NoRole
             The object is invalid and has no role set. This is generally a bug.
      \value CheckBox
      \value CheckableMenuItem
      \value ColumnHeader
      \value ComboBox
      \value DesktopFrame
      \value Dial
      \value Dialog
      \value Filler
      \value Frame
      \value Icon
      \value Label
      \value ListView
      \value ListItem
      \value Menu
      \value MenuBar
      \value MenuItem
      \value Tab
      \value TabContainer
      \value PasswordText
      \value PopupMenu
      \value ProgressBar
      \value Button
      \value RadioButton
      \value RadioMenuItem
      \value RowHeader
      \value ScrollBar
      \value ScrollArea
      \value Separator
      \value Slider
      \value SpinButton
      \value StatusBar
      \value TableView
      \value TableCell
      \value TableColumnHeader
      \value TableColumn
      \value TableRowHeader
      \value TableRow
      \value Terminal
      \value Text
      \value ToggleButton
      \value ToolBar
      \value ToolTip
      \value TreeView
      \value Window
      \value TreeIte
     */
    enum Role {
        NoRole,
        CheckBox,
        CheckableMenuItem,
        ColumnHeader,
        ComboBox,
        DesktopFrame,
        Dial,
        Dialog,
        Filler,
        Frame,
        Icon,
        Label,
        ListView,
        ListItem,
        Menu,
        MenuBar,
        MenuItem,
        Tab,
        TabContainer,
        PasswordText,
        PopupMenu,
        ProgressBar,
        Button,
        RadioButton,
        RadioMenuItem,
        RowHeader,
        ScrollBar,
        ScrollArea,
        Separator,
        Slider,
        SpinButton,
        StatusBar,
        TableView,
        TableCell,
        TableColumnHeader,
        TableColumn,
        TableRowHeader,
        TableRow,
        Terminal,
        Text,
        ToggleButton,
        ToolBar,
        ToolTip,
        TreeView,
        Window,
        TreeItem
// Roles in Qt, I don't think we want those
//    TitleBar       = 0x00000001,
//    Grip           = 0x00000004,
//    Sound          = 0x00000005,
//    Cursor         = 0x00000006,
//    Caret          = 0x00000007,
//    AlertMessage   = 0x00000008,
//    Client         = 0x0000000A,
//    Application    = 0x0000000E,
//    Document       = 0x0000000F,
//    Pane           = 0x00000010,
//    Chart          = 0x00000011,
//    Border         = 0x00000013,
//    Grouping       = 0x00000014,
//    Cell           = 0x0000001D,
//    Link           = 0x0000001E,
//    HelpBalloon    = 0x0000001F,
//    Assistant      = 0x00000020,
//    PageTab        = 0x00000025,
//    PropertyPage   = 0x00000026,
//    Indicator      = 0x00000027,
//    Graphic        = 0x00000028,
//    StaticText     = 0x00000029,
//    EditableText   = 0x0000002A,  // Editable, selectable, etc.
//    HotkeyField    = 0x00000032,
//    SpinBox        = 0x00000034,
//    Canvas         = 0x00000035,
//    Animation      = 0x00000036,
//    Equation       = 0x00000037,
//    ButtonDropDown = 0x00000038,
//    ButtonMenu     = 0x00000039,
//    ButtonDropGrid = 0x0000003A,
//    Whitespace     = 0x0000003B,
//    PageTabList    = 0x0000003C,
//    Clock          = 0x0000003D,
//    Splitter       = 0x0000003E,
//    LayeredPane    = 0x00000080,
    };

    /*!
        \enum QAccessibleClient::AccessibleObject::TextBoundary
        \brief The TextBoundaries enum represents the different boundaries when
        asking for text at a certain offset.
        \value CharBoundary
        \value WordStartBoundary
        \value WordEndBoundary
        \value SentenceStartBoundary
        \value SentenceEndBoundary
        \value LineStartBoundary
        \value LineEndBoundar
     */
    enum TextBoundary {
        CharBoundary,
        WordStartBoundary,
        WordEndBoundary,
        SentenceStartBoundary,
        SentenceEndBoundary,
        LineStartBoundary,
        LineEndBoundary
    };

    /*!
        \brief Construct an invalid AccessibleObject.
     */
    AccessibleObject();

    /*!
        \brief Copy constructor.
     */
    AccessibleObject(const AccessibleObject &other);

    /*!
      Destroys the AccessibleObject.
     */
    ~AccessibleObject();

    /*!
      Assignment operator.
     */
    AccessibleObject &operator=(const AccessibleObject &other);
    /*!
      Comparison operator.
     */
    bool operator==(const AccessibleObject &other) const;
    /*!
      Inequality operator.
     */
    inline bool operator!=(const AccessibleObject &other) const {
        return !operator==(other);
    }

    /*!
        \brief Returns a unique identifier for the object.
     */
    QString id() const;

    /*!
        \brief Returns a QUrl that references the AccessibleObject.

        This can be used to serialize/deserialize an AccessibleObject
        to pass it around as string and restore the AccessibleObject
        by using Registry::accessibleFromUrl later on.

        The returned QUrl returns a scheme of "accessibleobject", the
        D-Bus path as url path and the D-Bus service as url fragment.
     */
    QUrl url() const;

    /*!
        \brief Returns \c true if this object is valid.

        Invalid objects are for example returned when asking for the
        parent of the top most item, or for a child that is out of range.
     */
    bool isValid() const;

    /*!
        \brief Returns this object's parent.
     */
    AccessibleObject parent() const;

    /*!
        \brief Returns this accessible's index in its parent's list of children.
     */
    int indexInParent() const;

    /*!
        \brief Returns this accessible's children in a list.
     */
    QList<AccessibleObject> children() const;

    /*!
        \brief Returns this accessible's children according to their \a roles.

        Returns a vector that contains the children of this object according
        to there roles. The number of vector-items equals to the number and
        sorting of the roles items.

        Example usage:
        \code
        QList<Role> roles;
        roles << Label << CheckBox;
        QVector< QList<AccessibleObject> > c = children(roles);
        Q_ASSERT(c.count() == roles.count());
        Q_ASSERT(c[0].isEmpty() || c[0].first().role() == Label);
        Q_ASSERT(c[1].isEmpty() || c[1].first().role() == CheckBox);
        \endcode
     */
    QVector< QList<AccessibleObject> > children(const QList<Role> &roles) const;

    /*!
        \brief Returns the number of children for this accessible.
     */
    int childCount() const;

    /*!
        \brief Returns a specific child at position \a index.

        The list of children is 0-based.
     */
    AccessibleObject child(int index) const;

   /*!
        \brief Returns the accessible id of this accessible.

        This is an id which is stable over application development.
        It may be empty.
     */
    QString accessibleId() const;

    /*!
        \brief Returns the name of this accessible.

        The name is a short descriptive one or two words.
        It is localized.
     */
    QString name() const;

    /*!
        \brief Returns the description for this accessible.

        The description is more of an explanation than the name.
        This can be a sentence. The string is localized.
     */
    QString description() const;

    /*!
        \brief Returns the role as integer value of this accessible.
     */
    Role role() const;

    /*!
        \brief Returns the name of the role of this accessible.

        This name is not localized to allow tools to work with the English string.
     */
    QString roleName() const;

    /*!
        \brief Returns the name of the role of this accessible.

        This name is localized and can be presented to the user.
     */
    QString localizedRoleName() const;

    /*!
        \brief The ComponentLayer in which this object resides.
     */
    int layer() const;

    /*!
        \brief Obtain the relative stacking order ('Z' order) of an object.

        Larger values indicate that an object is on "top" of the stack, therefore
        objects with smaller MDIZOrder may be obscured by objects with a larger
        MDIZOrder, but not vice-versa.
     */
    int mdiZOrder() const;

    /*!
        \brief  Obtain the alpha value of the component.

        An alpha value of 1.0 or greater indicates that the object is fully opaque,
        and an alpha value of 0.0 indicates that the object is fully transparent.
        Negative alpha values have no defined meaning at this time.

        Alpha values are used in conjunction with Z-order calculations to determine
        whether an object wholly or partially obscures another object's visual
        intersection, in the event that their bounds intersect.
     */
    double alpha() const;

    /*!
        \brief Returns a bounding rectangle for the accessible.

        This can be used to get the focus coordinates.
    */
    QRect boundingRect() const;

    /*!
        \brief Returns a bounding rectangle for the character at position \a offset.

        This function is only supported for accessibles that implement the text interface.
        It will return an empty rectangle for invalid offsets or accessibles.
    */
    QRect characterRect(int offset) const;

    /*!
        \brief Returns a QStringList of interfaces supported by the accessible.

        This function provides a list of accessible interfaces that are implemented
        by an accessible object. This can be used to avoid calling functions that
        are not supported by the accessible.
    */
    Interfaces supportedInterfaces() const;

    /*!
        \brief Returns the offset of the caret from the beginning of the text.

        This function provides the current offset of the caret from the beginning of
        the text in an accessible that implements org.a11y.atspi.Text.
    */
    int caretOffset() const;

    /*!
        \brief Returns the number of characters.
    */
    int characterCount() const;

    /*!
        \brief Returns a list of selections the text has.

        Every item in that list is a pair of integers
        representing startOffset and endOffset of the selection.

        Code to demonstrate usage:
        \code
        QList< QPair<int,int> > sel = acc.textSelections();
        int startOffset = sel[0].first;
        int endOffset = sel[0].second;
        QString allText = acc.text();
        QString selText = allText.mid(startOffset, endOffset - startOffset);
        \endcode
    */
    QList< QPair<int,int> > textSelections() const;

    /*!
      Sets text \a selections.

      Usually only one selection will be set,
      use a list of QPairs with the start and end offsets for that.
     */
    void setTextSelections(const QList< QPair<int,int> > &selections);

    /*!
        \brief Returns the text of the TextInterface.

        This function provides the current text as displayed by the
        org.a11y.atspi.Text TextInterface component.

        \a startOffset The start caret offset to return the text from.

        \a endOffset The end caret offset to return the text from. If -1
        then the endOffset is the end of the string what means all characters
        are included.
    */
    QString text(int startOffset = 0, int endOffset = -1) const;

    /*!
        \brief Returns the text of the TextInterface by boundary.

        Especially for larger text fields it may be more performant and easier to
        query the text at a certain position instead of the full text.

        For example the line where the cursor is currently can be retrieved with this function
        in a convenient way.

        \a offset The position of the requested text.

        \a startOffset
               The beginning of the offset, for example the start of the line when
               asking for line boundaries.

        \a endOffset The end of the text section
    */
    QString textWithBoundary(int offset, TextBoundary boundary, int *startOffset = nullptr, int *endOffset = nullptr) const;

    /*!
        \brief Sets the \a text of the EditableTextInterface.

        Returns \c true on success, \c false otherwise.
    */
    bool setText(const QString &text);

    /*!
        \brief Inserts \a text into the EditableTextInterface
        at caret \a position with the given \a length.
    */
    bool insertText(const QString &text, int position = 0, int length = -1);

    /*!
        \brief Copy the text from the EditableTextInterface into the clipboard
        starting from the caret position \a startPos until \a endPos.

        Returns \c true on success, \c false otherwise.
    */
    bool copyText(int startPos, int endPos);

    /*!
        \brief Cut the text from the EditableTextInterface into the clipboard
        starting from the caret position \a startPos until \a endPos.

        Returns \c true on success, \c false otherwise.
    */
    bool cutText(int startPos, int endPos);

    /*!
        \brief Delete the text from the EditableTextInterface
        starting from the caret position \a startPos until \a endPos.

        Returns \c true on success, \c false otherwise.
    */
    bool deleteText(int startPos, int endPos);

    /*!
        \brief Paste the text from the clipboard into the EditableTextInterface
        at the given caret \a position.

        Returns \c true on success, \c false otherwise.
    */
    bool pasteText(int position);

    /*!
        \brief Returns the focus point of the object.
    */
    QPoint focusPoint() const;

    /*!
        \brief Returns the application object.

        Returns the top-level application object that expose an
        org.a11y.atspi.Application accessibility interface.
    */
    AccessibleObject application() const;

    /*!
        \brief Returns the toolkit name.

        This can be for example "Qt" or "gtk".
    */
    QString appToolkitName() const;

    /*!
        \brief Returns the toolkit version.

        This can be for example "4.8.3" for Qt 4.8.3.
    */
    QString appVersion() const;

    /*!
        \brief Returns the unique application identifier.

        The identifier will not last over the session
        and every time the app quits and restarts it gets another
        identifier that persists as long as the application is running.
    */
    int appId() const;

    /*!
      \enum QAccessibleClient::AccessibleObject::LocaleType
      \brief The type of locale.
      \value LocaleTypeMessages
      \value LocaleTypeCollate
      \value LocaleTypeCType
      \value LocaleTypeMonetary
      \value LocaleTypeNumeric
      \value LocaleTypeTime
     */
    enum LocaleType {
        LocaleTypeMessages,
        LocaleTypeCollate,
        LocaleTypeCType,
        LocaleTypeMonetary,
        LocaleTypeNumeric,
        LocaleTypeTime
    };

    /*!
        \brief The application locale.

        Returns a string compliant with the POSIX standard for the locale description.

        The locale will be queried against the LocaleType \a lctype.
    */
    QString appLocale(LocaleType lctype = LocaleTypeMessages) const;

    /*!
        \brief The application D-Bus address.
    */
    QString appBusAddress() const;

    /*!
        \brief The minimum value allowed by this valuator.

        If both, the minimumValue and maximumValue are zero then
        there are no minimum or maximum values. The currentValue has
        no range restrictions.
    */
    double minimumValue() const;

    /*!
        \brief The maximum value allowed by this valuator.

        If both, the minimumValue and maximumValue, are zero then
        there is no minimum or maximum values. The currentValue has
        no range restrictions.
    */
    double maximumValue() const;

    /*!
        \brief The smallest incremental change which this valuator allows.

        This is a helper value to know in what steps the currentValue
        is incremented or decremented.

        If 0, the incremental changes to the valuator are limited only by
        the precision of a double precision value on the platform.
    */
    double minimumValueIncrement() const;

    /*!
        \brief The current value of the valuator.

        This is the value the org.a11y.atspi.Value accessibility interface has.
    */
    double currentValue() const;

    /*!
        \brief Sets the \a value of the valuator.

        Returns \c true on success, \c false otherwise.
    */
    bool setCurrentValue(const double value);

    /*!
        \brief Returns the selection of accessible objects.
    */
    QList<AccessibleObject> selection() const;

    /*!
        \brief A description text of the image.

        It is recommended that imageDescription be the shorter of the available image
        descriptions, for instance "alt text" in HTML images, and a longer description
        be provided in QAccessible::Description, if available. A short, one
        or two word label for the image should be provided in QAccessible::Name.

        Returns a UTF-8 string providing a textual description of what is visually
        depicted in the image.
    */
    QString imageDescription() const;

    /*!
        \brief The locale of the image.

        Returns a string corresponding to the POSIX LC_MESSAGES locale used by the
        imageDescription.
    */
    QString imageLocale() const;

    /*!
        \brief The image boundaries.

        Obtain a bounding box which entirely contains the image contents, as
        displayed on screen.

        The bounds returned do not account for any viewport clipping or the fact that
        the image may be partially or wholly obscured by other onscreen content.

        This method returns the bounds of the current onscreen view, and not the
        nominal size of the source data in the event that the original image has
        been rescaled.

        Returns a BoundingBox enclosing the image's onscreen representation.
    */
    QRect imageRect() const;

    /*!
        \brief Returns a list of actions supported by this accessible.

        Just trigger() the action to execute the underlying method at the accessible.
    */
    QVector< QSharedPointer<QAction> > actions() const;

    // states
    /*! Returns if the AccessibleObject is currently active. */
    bool isActive() const;
    /*! Returns if the AccessibleObject is checkable (often indicates a check action). */
    bool isCheckable() const;
    /*! Returns if the AccessibleObject is currently checked. */
    bool isChecked() const;
    /*! Returns if the AccessibleObject is defunct. That means it does not properly respond to requests
        and should be ignored for accessibility purposes. */
    bool isDefunct() const;
    /*! Returns if the AccessibleObject is an editable text. */
    bool isEditable() const;
    /*! Returns if the AccessibleObject is currently enabled. */
    bool isEnabled() const;
    /*! Returns if the AccessibleObject can be expanded to show more information. */
    bool isExpandable() const;
    /*! Returns if the AccessibleObject is currently expanded. */
    bool isExpanded() const;
    /*! Returns if the AccessibleObject is focusable. */
    bool isFocusable() const;
    /*! Returns if the AccessibleObject is currently focused. */
    bool isFocused() const;
    /*! Returns if the AccessibleObject is a multiline text edit. */
    bool isMultiLine() const;
    /*! Returns if the AccessibleObject is selectable. */
    bool isSelectable() const;
    /*! Returns if the AccessibleObject is currently selected. */
    bool isSelected() const;
    /*! Returns if the AccessibleObject reacts to input events. */
    bool isSensitive() const;
    /*! Returns if the AccessibleObject is a single line text edit. */
    bool isSingleLine() const;

    /*!
      \brief Return a string representing states of this object.

      This is useful for debugging applications.
    */
    QString stateString() const;

    /*
     * \internal
     * \brief isTransient marks an object as being unreliable in that it can quickly disappear or change
     *
     * This is mostly a hint that the object should not be cached.
     * \return true if the object is transient
     */
//    bool isTransient() const;

    /*! Returns if the AccessibleObject is currently visible (it can still be off the screen,
        but there is nothing preventing the user from seeing it in general). */
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
    /*! Returns if the AccessibleObject is the default widget (e.g. a button in a dialog). */
    bool isDefault() const;
//    bool isVisited() const;

    /*! Returns if the AccessibleObject allows text selections. */
    bool hasSelectableText() const;
    /*! Returns if the AccessibleObject has a tool tip. */
    bool hasToolTip() const;
    /*! Returns if the AccessibleObject supports automatic text completion. */
    bool supportsAutocompletion() const;

private:
    AccessibleObject(RegistryPrivate *reg, const QString &service, const QString &path);
    AccessibleObject(const QSharedPointer<AccessibleObjectPrivate> &dd);
    QSharedPointer<AccessibleObjectPrivate> d;

    friend class Registry;
    friend class RegistryPrivate;
    friend class CacheWeakStrategy;
    friend class CacheStrongStrategy;
#ifndef QT_NO_DEBUG_STREAM
    friend QDebug QAccessibleClient::operator<<(QDebug, const AccessibleObject &);
#endif
    friend uint qHash(const QAccessibleClient::AccessibleObject& object) {
        return qHash(object.d);
    }
};

}

Q_DECLARE_METATYPE(QAccessibleClient::AccessibleObject)

#endif
