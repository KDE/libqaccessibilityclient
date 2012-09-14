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
#include <QSharedPointer>
#include <qaction.h>

#include <atspi/atspi-constants.h>

#include "libkdeaccessibilityclient_export.h"

namespace KAccessibleClient {

class AccessibleObjectPrivate;
class RegistryPrivate;

/**
    This class represents an accessible object.

    An accessible object equals usually a visible widget or some kind
    of other element the user can interact with but can also present
    a not visible object that offers certain functionality like for
    example actions which can be triggered.

    It is implicitly shared and only created by the library.
*/
class LIBKDEACCESSIBILITYCLIENT_EXPORT AccessibleObject
{
public:

    enum Interface {
        NoInterface = 0x0,
        Cache = 0x1,
        Accessible = 0x2,
        Action = 0x4,
        Application = 0x8,
        Collection = 0x10,
        Component = 0x20,
        Document = 0x40,
        EditableText = 0x80,
        EventKeyboard = 0x100,
        EventMouse = 0x200,
        EventObject = 0x400,
        Hyperlink = 0x800,
        Hypertext = 0x1000,
        Image = 0x2000,
        Selection = 0x4000,
        Table = 0x8000,
        Text = 0x10000,
        Value = 0x20000,
        Socket = 0x40000,
        EventWindow = 0x80000,
        EventFocus = 0x100000
    };

    Q_DECLARE_FLAGS(Interfaces, Interface)

    /**
        \brief Construct an invalid AccessibleObject.
     */
    AccessibleObject();

    /**
        \brief Copy constructor.
     */
    AccessibleObject(const AccessibleObject &other);
    ~AccessibleObject();
    AccessibleObject &operator=(const AccessibleObject &other);
    bool operator==(const AccessibleObject &other) const;

    /**
        \brief Returns a unique identifier for the object.
     */
    QString id() const;

    /**
        \brief Returns a QUrl that references the AccessibleObject.

        This can be used to serialize/unserialize an AccessibleObject
        to pass it around as string and restore the AccessibleObject
        later on.

        The returned QUrl returns a scheme of "AccessibleObject", the
        dbus path as url path and the dbus service as url fragment.
     */
    QUrl url() const;

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

    /**
        \brief The ComponentLayer in which this object resides.
     */
    int layer() const;

    /**
        \brief Obtain the relative stacking order (i.e. 'Z' order) of an object.

        Larger values indicate that an object is on "top" of the stack, therefore
        objects with smaller MDIZOrder may be obscured by objects with a larger
        MDIZOrder, but not vice-versa.
     */
    int mdiZOrder() const;

    /**
        \brief  Obtain the alpha value of the component.

        An alpha value of 1.0 or greater indicates that the object is fully opaque,
        and an alpha value of 0.0 indicates that the object is fully transparent.
        Negative alpha values have no defined meaning at this time.

        Alpha values are used in conjunction with Z-order calculations to determine
        whether an object wholly or partially obscures another object's visual
        intersection, in the event that their bounds intersect.
     */
    double alpha() const;

    /**
        \brief Returns a bounding rectangle for the accessible.

        It returns a QRect that bounds the accessible. This can be used to get the focus coordinates.

        \return QRect that bounds the accessible.
    */
    QRect boundingRect() const;

    /**
        \brief Returns a bounding rectangle for the characters present in the accessible.

        boundingRect might give a large rectangle for a large text area that has little text.
        This function returns a bounding QRect for the characters present in the text area.

        \return QRect that bounds the text in an accessible.
    */
    QRect characterRect() const;

    /**
        \brief Returns List of interfaces supported by the accessible.

        This function provides a list of accessibile interfaces that are implemented
        by an accessible object. This can be used to avoid calling functions that
        are not supported by the accessible.

        \return QStringList that contains list of supported interfaces
    */
    Interfaces supportedInterfaces() const;

    /**
        \brief Returns the offset of the caret from the beginning of the text.

        This function provides the current offset of the caret from the beginning of
        the text in an accessible that implements org.a11y.atspi.Text.

        \return Caret Offset as an integer
    */
    int caretOffset() const;

    /**
        \brief Returns focus-point of the object

        \return The Focus Point of the object
    */
    QPoint focusPoint() const;

    /**
        \brief Returns the application object.

        \return The top-level application object that expose an
        org.a11y.atspi.Application accessibility interface.
    */
    AccessibleObject application() const;

    /**
        \brief Returns the toolkit name.

        \return The tookit name. This can be for example "Qt"
        or "gtk".
    */
    QString appToolkitName() const;

    /**
        \brief Returns the toolkit version.

        \return The tookit version. This can be for example "4.8.3"
        for Qt 4.8.3.
    */
    QString appVersion() const;

    /**
        \brief Returns the unique application identifier.

        \return The app id. The identifier will not last over session
        and everytime the app quits and restarts it gets another
        identifier that persists as long as the application is running.
    */
    int appId() const;

    enum LocaleType {
        LocaleTypeMessages,
        LocaleTypeCollate,
        LocaleTypeCType,
        LocaleTypeMonetary,
        LocaleTypeNumeric,
        LocaleTypeTime
    };

    /**
        \brief The application locale.

        \param  lctype The \a LocaleType for which the locale is queried.
        \return A string compliant with the POSIX standard for locale description.
    */
    QString appLocale(LocaleType lctype = LocaleTypeMessages) const;

    /**
        \brief The application dbus address.
    */
    QString appBusAddress() const;

    /**
        \brief The minimum value allowed by this valuator.

        If both, the \a minimumValue and \a maximumValue, are zero then
        there is no minimum or maximum values. The \a currentValue has
        no range restrictions.
    */
    double minimumValue() const;

    /**
        \brief The maximum value allowed by this valuator.

        If both, the \a minimumValue and \a maximumValue, are zero then
        there is no minimum or maximum values. The \a currentValue has
        no range restrictions.
    */
    double maximumValue() const;

    /**
        \brief The smallest incremental change which this valuator allows.

        This is a helper value to know in what steps the \a currentValue
        is incremented or decremented.

        If 0, the incremental changes to the valuator are limited only by
        the precision of a double precision value on the platform.
    */
    double minimumValueIncrement() const;

    /**
        \brief The current value of the valuator.

        This is the value the org.a11y.atspi.Value accessibility interface has.
    */
    double currentValue() const;

    /**
        \brief Returns the selection of accessible objects.
    */
    QList<AccessibleObject> selection() const;

    /**
        \brief A description text of the image.

        It is recommended that imageDescription be the shorter of the available image
        descriptions, for instance "alt text" in HTML images, and a longer description
        be provided in Accessible::accessible-description, if available. A short, one
        or two word label for the image should be provided in Accessible::accessible-name.

        \return A UTF-8 string providing a textual description of what is visually
        depicted in the image.
    */
    QString imageDescription() const;

    /**
        \brief The locale of the image.

        \return A string corresponding to the POSIX LC_MESSAGES locale used by the
        imageDescription.
    */
    QString imageLocale() const;

    /**
        \brief The image boundaries.

        Obtain a bounding box which entirely contains the image contents, as
        displayed on screen.

        The bounds returned do not account for any viewport clipping or the fact that
        the image may be partially or wholly obscured by other onscreen content.

        This method returns the bounds of the current onscreen view, and not the
        nominal size of the source data in the event that the original image has
        been rescaled.\

        \return A BoundingBox enclosing the image's onscreen representation.
    */
    QRect imageRect() const;

    /**
        \brief Returns a list of actions supported by this accessible.

        Just trigger() the action to execute the underlying method at the accessible.
    */
    QList<QAction*> actions() const;

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

public:

    /**
        \internal getter to fetch the \a RegistryPrivate instance that
        created this AccessibleObject or NULL if this AccessibleObject
        is invalid.
     */
    RegistryPrivate* registryPrivate() const;

    /**
        \internal getter to fetch the \a AccessibleObjectPrivate d-pointer
        instance or NULL if this AccessibleObject is invalid.
     */
    QSharedPointer<AccessibleObjectPrivate> objectPrivate() const;

private:
    AccessibleObject(RegistryPrivate *reg, const QString &service, const QString &path);
    AccessibleObject(const QSharedPointer<AccessibleObjectPrivate> &dd);
    QSharedPointer<AccessibleObjectPrivate> d;

    friend class Registry;
    friend class RegistryPrivate;
    friend QDebug KAccessibleClient::operator<<(QDebug, const AccessibleObject &);
};

#ifndef QT_NO_DEBUG_STREAM
LIBKDEACCESSIBILITYCLIENT_EXPORT QDebug operator<<(QDebug, const AccessibleObject &);
#endif

}

Q_DECLARE_METATYPE(KAccessibleClient::AccessibleObject)

#endif
