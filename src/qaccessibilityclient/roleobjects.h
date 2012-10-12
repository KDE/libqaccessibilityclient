/*
    Copyright 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

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

#ifndef QACCESSIBILITYCLIENT_ROLEOBJECTS_H
#define QACCESSIBILITYCLIENT_ROLEOBJECTS_H

#include <QObject>
#include <QRect>

#include "qaccessibilityclient_export.h"
#include "accessibleobject.h"

namespace QAccessibleClient {

/**
    Base class for role objects.

    The role objects offer a high level API to deal with accessible
    objects. Every accessible objects has a role assigned and that
    role is used with the role objects to create one specialized
    role object instance per accessible object. The API given is
    Qt-ified using the QMetaObject system to allow introspection.
*/
class QACCESSIBILITYCLIENT_EXPORT ObjectRole : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AccessibleObject accessible READ accessible)
    Q_PROPERTY(AccessibleObject::Role role READ role)
    Q_PROPERTY(QString roleName READ roleName)
    Q_PROPERTY(QString localizedRoleName READ localizedRoleName)
    Q_PROPERTY(QString url READ url)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QRect boundingRect READ boundingRect)
public:
    virtual ~ObjectRole();

    AccessibleObject accessible() const;
    AccessibleObject::Role role() const;
    QString roleName() const;
    QString localizedRoleName() const;
    QString url() const;
    QString name() const;
    QString description() const;
    QRect boundingRect() const;

    static ObjectRole* create(const AccessibleObject &acc);

protected:
    class Private;
    Private *d;

    ObjectRole(const AccessibleObject &acc);
    ObjectRole(Private *dd);
};

/**
    CheckBox role object.
*/
class CheckBoxRole : public ObjectRole
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text)
public:
    explicit CheckBoxRole(const AccessibleObject &acc);
    QString text() const;
    //bool isChecked() const;
    //void setChecked(bool checked);
};

// CheckableMenuItem
// ColumnHeader

/**
    ComboBox role object.
*/
class ComboBoxRole : public ObjectRole
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text)
public:
    explicit ComboBoxRole(const AccessibleObject &acc);
    QString text() const;
    //void setText(const QString &text);
    //QStringList items() const;
    //int currentIndex() const;
    //void setCurrentIndex(int index);
};

// DesktopFrame
// Dial
// Dialog
// Filler
// Frame
// Icon

/**
    Label role object.
*/
class LabelRole : public ObjectRole
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text)
public:
    explicit LabelRole(const AccessibleObject &acc);
    QString text() const;
};

// ListView
// ListItem
// Menu
// MenuBar
// MenuItem
// Tab
// TabContainer
// PasswordText
// PopupMenu
// ProgressBar

/**
    Button role object.
*/
class ButtonRole : public ObjectRole
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text)
public:
    explicit ButtonRole(const AccessibleObject &acc);
    QString text() const;
    //void activate();
};

/**
    RadioButton role object.
*/
class RadioButtonRole : public ObjectRole
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text)
public:
    explicit RadioButtonRole(const AccessibleObject &acc);
    QString text() const;
    //bool isChecked() const;
    //void setChecked(bool checked);
};

// RadioMenuItem
// RowHeader
// ScrollBar
// ScrollArea
// Separator
// Slider
// SpinButton
// StatusBar
// TableView
// TableCell
// TableColumnHeader
// TableColumn
// TableRowHeader
// TableRow
// Terminal

/**
    Text role object.
*/
class TextRole : public ObjectRole
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool isEditable READ isEditable)
public:
    explicit TextRole(const AccessibleObject &acc);
    QString text() const;
    bool isEditable() const;
public slots:
    void setText(const QString &text);
};

/**
    ToggleButton role object.
*/
class ToggleButtonRole : public ObjectRole
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text)
public:
    explicit ToggleButtonRole(const AccessibleObject &acc);
    QString text() const;
    //void toggle(bool toggled);
};

// ToolBar
// ToolTip
// TreeView
// Window
// TreeItem

}

//Q_DECLARE_METATYPE(QAccessibleClient::ObjectRole)

#endif

