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

#include "accessibleproperties.h"

#include "qaccessibilityclient/registry.h"

using namespace QAccessibleClient;

ObjectProperties::ObjectProperties(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(2);
    setHorizontalHeaderLabels( QStringList() << QString("Property") << QString("Value") );
}

ObjectProperties::~ObjectProperties()
{
}

QHash<int,QByteArray> ObjectProperties::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    return roles;
}

void ObjectProperties::setAccessibleObject(const QAccessibleClient::AccessibleObject &acc)
{
    beginResetModel();
    m_acc = acc;

    clear();

    if (!acc.isValid()) {
        endResetModel();
        return;
    }

    QAccessibleClient::AccessibleObject::Interfaces interfaces = acc.supportedInterfaces();
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::AccessibleInterface)) {
        QStandardItem *item = append(QString("Accessible"));
        append(QString("Name"), acc.name(), item);
        append(QString("Description"), acc.description(), item);
        append(QString("Role"), acc.roleName(), item);
        append(QString("LocalizedRole"), acc.localizedRoleName(), item);
        append(QString("Visible"), acc.isVisible(), item);
        append(QString("Default"), acc.isDefault(), item);
        append(QString("State"), stateString(acc), item);
        append(tr("Url"), acc.url(), item);
        AccessibleObject parent = acc.parent();
        if (parent.isValid())
            append(tr("Parent"), parent.url(), item);
        int childCount = acc.childCount();
        QStandardItem *children = append(QString("Children"), acc.childCount(), item);
        for (int i = 0; i < childCount; ++i) {
            AccessibleObject child = acc.child(i);
            if (!child.isValid()) {
                append(QLatin1String("Broken child"), QString::number(i), children);
            } else {
                append(child.name().isEmpty() ? tr("[%1]").arg(child.roleName()) : child.name(), child.url(), children);
            }
        }
        //GetAttributes
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ComponentInterface)) {
        QStandardItem *item = append(QString("Component"));
        append(QString("BoundingRect"), acc.boundingRect(), item);
        append(QString("Layer"), acc.layer(), item);
        append(QString("MDIZOrder"), acc.mdiZOrder(), item);
        append(QString("Alpha"), acc.alpha(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::CollectionInterface)) {
        QStandardItem *item = append(QString("Collection"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ApplicationInterface)) {
        QStandardItem *item = append(QString("Application"));
        append(QString("ToolkitName"), acc.appToolkitName(), item);
        append(QString("Version"), acc.appVersion(), item);
        append(QString("Id"), acc.appId(), item);
        append(QString("Locale"), acc.appLocale(), item);
        append(QString("BusAddress"), acc.appBusAddress(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::DocumentInterface)) {
        QStandardItem *item = append(QString("Document"));
        Q_UNUSED(item);
        //GetLocale
        //GetAttributeValue
        //GetAttributes
    }

    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EditableTextInterface)) {
        QStandardItem *item = append(QString("EditableText"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::HyperlinkInterface)) {
        QStandardItem *item = append(QString("Hyperlink"));
        Q_UNUSED(item);
        /*
        <property name="NAnchors" type="n" access="read"/>
        <property name="StartIndex" type="i" access="read"/>
        <property name="EndIndex" type="i" access="read"/>
        <method name="GetObject">
            <arg direction="in" name="i" type="i"/>
            <arg direction="out" type="(so)"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiObjectReference"/>
        </method>
        0<method name="GetURI">
            <arg direction="in" name="i" type="i"/>
            <arg direction="out" type="s"/>
        </method>
        <method name="IsValid">
            <arg direction="out" type="b"/>
        </method>
        */
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::HypertextInterface)) {
        QStandardItem *item = append(QString("Hypertext"));
        Q_UNUSED(item);
        /*
        <method name="GetNLinks">
            <arg direction="out" type="i"/>
        </method>
        <method name="GetLink">
            <arg direction="in" name="linkIndex" type="i"/>
            <arg direction="out" type="(so)"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiObjectReference"/>
        </method>
        <method name="GetLinkIndex">
            <arg direction="in" name="characterIndex" type="i"/>
            <arg direction="out" type="i"/>
        </method>
        */
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ImageInterface)) {
        QStandardItem *item = append(QString("Image"));
        append(QString("Description"), acc.imageDescription(), item);
        append(QString("Locale"), acc.imageLocale(), item);
        append(QString("Rect"), acc.imageRect(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::SelectionInterface)) {
        QStandardItem *item = append(QString("Selection"));
        Q_FOREACH(const QAccessibleClient::AccessibleObject &s, acc.selection()) {
            append(s.name(), s.role(), item);
        }
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::TableInterface)) {
        QStandardItem *item = append(QString("Table"));
        Q_UNUSED(item);
        /*
        <property name="NRows" type="i" access="read"/>
        <property name="NColumns" type="i" access="read"/>
        <property name="Caption" type="(so)" access="read">
            <annotation name="com.trolltech.QtDBus.QtTypeName" value="QSpiObjectReference"/>
        </property>
        <property name="Summary" type="(so)" access="read">
            <annotation name="com.trolltech.QtDBus.QtTypeName" value="QSpiObjectReference"/>
        </property>
        <property name="NSelectedRows" type="i" access="read"/>
        <property name="NSelectedColumns" type="i" access="read"/>
        <method name="GetRowDescription">
            <arg direction="in" name="row" type="i"/>
            <arg direction="out" type="s"/>
        </method>
        <method name="GetColumnDescription">
            <arg direction="in" name="column" type="i"/>
            <arg direction="out" type="s"/>
        </method>
        <method name="GetSelectedRows">
            <arg direction="out" type="ai"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiIntList"/>
        </method>
        <method name="GetSelectedColumns">
            <arg direction="out" type="ai"/>
            <annotation name="com.trolltech.QtDBus.QtTypeName.Out0" value="QSpiIntList"/>
        </method>
        */
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::TextInterface)) {
        QStandardItem *item = append(QString("Text"));
        int offset = acc.caretOffset();
        append(QString("CaretOffset"), offset, item);
        append(QString("CharacterCount"), acc.characterCount(), item);
        append(QString("CharacterRect"), acc.characterRect(offset), item);

        QString text = acc.text();
        append(QString("Text"), text, item);

        QList< QPair<int,int> > selections = acc.textSelections();
        QStandardItem *selectionsItem = append(QString("Selections"), selections.count(), item);
        for (int i = 0; i < selections.count(); ++i) {
            QPair<int,int> sel = selections[i];
            int startOffset = sel.first;
            int endOffset = sel.second;
            Q_ASSERT(startOffset <= endOffset);
            append( QString("%1:%2").arg(startOffset).arg(endOffset),
                    text.mid(startOffset, endOffset - startOffset),
                    selectionsItem );
        }
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ValueInterface)) {
        QStandardItem *item = append(QString("Value"));
        append(QString("Current"), acc.currentValue(), item);
        append(QString("Minimum"), acc.minimumValue(), item);
        append(QString("Maximum"), acc.maximumValue(), item);
        append(QString("Increment"), acc.minimumValueIncrement(), item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::SocketInterface)) {
        QStandardItem *item = append(QString("Socket"));
        Q_UNUSED(item);
    }

    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventKeyboardInterface)) {
        QStandardItem *item = append(QString("EventKeyboard"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventMouseInterface)) {
        QStandardItem *item = append(QString("EventMouse"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventObjectInterface)) {
        QStandardItem *item = append(QString("EventObject"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventWindowInterface)) {
        QStandardItem *item = append(QString("EventWindow"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::EventFocusInterface)) {
        QStandardItem *item = append(QString("EventFocus"));
        Q_UNUSED(item);
    }

    if (interfaces.testFlag(QAccessibleClient::AccessibleObject::ActionInterface)) {
        QStandardItem *item = append(QString("Action"));
        Q_FOREACH(const QSharedPointer<QAction> &a, acc.actions()) {
            QStandardItem *nameItem = new QStandardItem(a->text());
            QStandardItem *valueItem = new QStandardItem(a->whatsThis());
            item->appendRow(QList<QStandardItem*>() << nameItem << valueItem);
        }
    }

    endResetModel();
}

void ObjectProperties::doubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || !index.parent().isValid() || index.parent().data().toString() != QLatin1String("Action"))
        return;

    foreach (const QSharedPointer<QAction> &action, m_acc.actions()) {
        if (action->text() == data(index).toString()) {
            action->trigger();
            return;
        }
    }
}

QStandardItem* ObjectProperties::append(const QString &name, const QVariant &value, QStandardItem *parentItem)
{
    if (!parentItem)
        parentItem = invisibleRootItem();
    QStandardItem *nameItem = new QStandardItem(name);
    QString text;
    switch (value.type()) {
        case QVariant::Point: {
            QPoint p = value.toPoint();
            text = QString("%1:%2").arg(p.x()).arg(p.y());
        } break;
        case QVariant::PointF: {
            QPointF p = value.toPointF();
            text = QString("%1:%2").arg(p.x()).arg(p.y());
        } break;
        case QVariant::Rect: {
            QRect r = value.toRect();
            text = QString("%1:%2 %3x%4").arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height());
        } break;
        case QVariant::RectF: {
            QRectF r = value.toRectF();
            text = QString("%1:%2 %3x%4").arg(r.left()).arg(r.top()).arg(r.width()).arg(r.height());
        } break;
        default:
            text = value.toString();
            break;
    }
    QStandardItem *valueItem = new QStandardItem(text);
    parentItem->appendRow(QList<QStandardItem*>() << nameItem << valueItem);
    return nameItem;
}

QString ObjectProperties::stateString(const QAccessibleClient::AccessibleObject &acc)
{
    QStringList s;
    if (acc.isActive()) s << "Active";
    if (acc.isCheckable()) s << "Checkable";
    if (acc.isChecked()) s << "Checked";
    if (acc.isEditable()) s << "Editable";
    if (acc.isExpandable()) s << "Expandable";
    if (acc.isExpanded()) s << "Expanded";
    if (acc.isFocusable()) s << "Focusable";
    if (acc.isFocused()) s << "Focused";
    if (acc.isMultiLine()) s << "MultiLine";
    if (acc.isSelectable()) s << "Selectable";
    if (acc.isSelected()) s << "Selected";
    if (acc.isSensitive()) s << "Sensitive";
    if (acc.isSingleLine()) s << "SingleLine";
    return s.join(",");
}
