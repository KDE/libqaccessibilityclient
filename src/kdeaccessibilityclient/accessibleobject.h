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

    Q_DECLARE_FLAGS(Interfaces,Interface)

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

private:
    AccessibleObject(RegistryPrivate *reg, const QString &service, const QString &path);
    QSharedDataPointer<AccessibleObjectPrivate> d;

    friend class RegistryPrivate;
    friend class AtSpiDBus;
    friend QDebug KAccessibleClient::operator<<(QDebug, const AccessibleObject &);
};

#ifndef QT_NO_DEBUG_STREAM
LIBKDEACCESSIBILITYCLIENT_EXPORT QDebug operator<<(QDebug, const AccessibleObject &);
#endif

}

#endif
