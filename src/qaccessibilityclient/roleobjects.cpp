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

#include "roleobjects_p.h"

using namespace QAccessibleClient;

namespace QAccessibleClient {

class ObjectRole::Private
{
public:
    AccessibleObject m_acc;
    explicit Private(const AccessibleObject &acc)
        : m_acc(acc)
    {
    }
};

}

ObjectRole::ObjectRole(const AccessibleObject &acc
    : QObject()
    , d(new Private(acc))
{
}

ObjectRole::ObjectRole(Private *dd)
    : QObject()
    , d(dd)
{
}

ObjectRole::~ObjectRole()
{
    delete d;
}

AccessibleObject ObjectRole::accessible() const
{
    return d->m_acc;
}

AccessibleObject::Role ObjectRole::role() const
{
    return d->m_acc.role();
}

QString ObjectRole::roleName() const
{
    return d->m_acc.roleName();
}

QString ObjectRole::localizedRoleName() const
{
    return d->m_acc.localizedRoleName();
}

QString ObjectRole::url() const
{
    return d->m_acc.url().toString();
}

QString ObjectRole::name() const
{
    return d->m_acc.name();
}

QString ObjectRole::description() const
{
    return d->m_acc.description();
}

QRect ObjectRole::boundingRect() const
{
    return d->m_acc.boundingRect();
}

ObjectRole* ObjectRole::create(const AccessibleObject &acc)
{
    switch (acc.role()) {
        case AccessibleObject::CheckBox: return new CheckBoxRole(acc);
        // case AccessibleObject::CheckableMenuItem:
        // case AccessibleObject::ColumnHeader:
        case AccessibleObject::ComboBox: return new ComboBoxRole(acc);
        // case AccessibleObject::DesktopFrame:
        // case AccessibleObject::Dial:
        // case AccessibleObject::Dialog:
        // case AccessibleObject::Filler:
        // case AccessibleObject::Frame:
        // case AccessibleObject::Icon:
        case AccessibleObject::Label: return new LabelRole(acc);
        // case AccessibleObject::ListView:
        // case AccessibleObject::ListItem:
        // case AccessibleObject::Menu:
        // case AccessibleObject::MenuBar:
        // case AccessibleObject::MenuItem:
        // case AccessibleObject::Tab:
        // case AccessibleObject::TabContainer:
        // case AccessibleObject::PasswordText:
        // case AccessibleObject::PopupMenu:
        // case AccessibleObject::ProgressBar:
        case AccessibleObject::Button: return new ButtonRole(acc);
        case AccessibleObject::RadioButton: return new RadioButtonRole(acc);
        // case AccessibleObject::RadioMenuItem:
        // case AccessibleObject::RowHeader:
        // case AccessibleObject::ScrollBar:
        // case AccessibleObject::ScrollArea:
        // case AccessibleObject::Separator:
        // case AccessibleObject::Slider:
        // case AccessibleObject::SpinButton:
        // case AccessibleObject::StatusBar:
        // case AccessibleObject::TableView:
        // case AccessibleObject::TableCell:
        // case AccessibleObject::TableColumnHeader:
        // case AccessibleObject::TableColumn:
        // case AccessibleObject::TableRowHeader:
        // case AccessibleObject::TableRow:
        // case AccessibleObject::Terminal:
        case AccessibleObject::Text: return new TextRole(acc);
        case AccessibleObject::ToggleButton: return new ToggleRole(acc);
        // case AccessibleObject::ToolBar:
        // case AccessibleObject::ToolTip:
        // case AccessibleObject::TreeView:
        // case AccessibleObject::Window:
        // case AccessibleObject::TreeItem:
    }
    return 0;
}

CheckBoxRole::CheckBoxRole(const AccessibleObject &acc)
    : ObjectRole(acc)
{
}

QString CheckBoxRole::text() const
{
    return accessible().name();
}

ComboBoxRole::ComboBoxRole(const AccessibleObject &acc)
    : ObjectRole(acc)
{
}

QString ComboBoxRole::text() const
{
    return accessible().name();
}

LabelRole::LabelRole(const AccessibleObject &acc)
    : ObjectRole(acc)
{
}

QString LabelRole::text() const
{
    return accessible().name();
}

ButtonRole::ButtonRole(const AccessibleObject &acc)
    : ObjectRole(acc)
{
}

QString ButtonRole::text() const
{
    return accessible().name();
}

RadioButtonRole::RadioButtonRole(const AccessibleObject &acc)
    : ObjectRole(acc)
{
}

QString RadioButtonRole::text() const
{
    return accessible().name();
}

TextRole::TextRole(const AccessibleObject &acc)
    : ObjectRole(acc)
{
}

QString TextRole::text() const
{
    if (accessible().supportedInterfaces().testFlag(AccessibleObject::TextInterface))
        return accessible().text();
    return accessible().name();
}

void TextRole::setText(const QString &text)
{
    if (isEditable())
        accessible().setText(text);
    else
        qWarning() << "Cannot TextRole::setText on none editable text object";
}

bool TextRole::isEditable() const
{
    return accessible().supportedInterfaces().testFlag(AccessibleObject::EditableTextInterface);
}

ToggleButtonRole::ToggleButtonRole(const AccessibleObject &acc)
    : ObjectRole(acc)
{
}

QString ToggleButtonRole::text() const
{
    return accessible().name();
}
