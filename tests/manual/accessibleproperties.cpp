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

#include "kdeaccessibilityclient/accessibleobject.h"
#include "kdeaccessibilityclient/registry.h"

using namespace KAccessibleClient;

ObjectProperties::ObjectProperties(QObject *parent)
    : QStandardItemModel(parent)
{
}

ObjectProperties::~ObjectProperties()
{
}

void ObjectProperties::setAccessibleObject(const KAccessibleClient::AccessibleObject &acc)
{
    beginResetModel();

    clear();
    setColumnCount(2);
    setHorizontalHeaderLabels( QStringList() << QString("Property") << QString("Value") );

    if (!acc.isValid()) {
        endResetModel();
        return;
    }

    KAccessibleClient::AccessibleObject::Interfaces interfaces = acc.supportedInterfaces();
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Accessible)) {
        QStandardItem *item = append(QString("Accessible"));
        append(QString("Name"), acc.name(), item);
        append(QString("Description"), acc.description(), item);
        append(QString("Role"), acc.roleName(), item);
        append(QString("LocalizedRole"), acc.localizedRoleName(), item);
        append(QString("Visible"), acc.isVisible(), item);
        append(QString("Default"), acc.isDefault(), item);
        append(QString("State"), stateString(acc), item);
        //GetAttributes
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Component)) {
        QStandardItem *item = append(QString("Component"));
        append(QString("BoundingRect"), acc.boundingRect(), item);
        append(QString("Layer"), acc.layer(), item);
        append(QString("MDIZOrder"), acc.mdiZOrder(), item);
        append(QString("Alpha"), acc.alpha(), item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Collection)) {
        QStandardItem *item = append(QString("Collection"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Application)) {
        QStandardItem *item = append(QString("Application"));
        append(QString("ToolkitName"), acc.appToolkitName(), item);
        append(QString("Version"), acc.appVersion(), item);
        append(QString("Id"), acc.appId(), item);
        append(QString("Locale"), acc.appLocale(), item);
        append(QString("BusAddress"), acc.appBusAddress(), item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Document)) {
        QStandardItem *item = append(QString("Document"));
        Q_UNUSED(item);
        //GetLocale
        //GetAttributeValue
        //GetAttributes
    }

    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::EditableText)) {
        QStandardItem *item = append(QString("EditableText"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Hyperlink)) {
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
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Hypertext)) {
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
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Image)) {
        QStandardItem *item = append(QString("Image"));
        append(QString("Description"), acc.imageDescription(), item);
        append(QString("Locale"), acc.imageLocale(), item);
        append(QString("Rect"), acc.imageRect(), item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Selection)) {
        QStandardItem *item = append(QString("Selection"));
        Q_FOREACH(const KAccessibleClient::AccessibleObject &s, acc.selection()) {
            append(s.name(), s.role(), item);
        }
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Table)) {
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
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Text)) {
        QStandardItem *item = append(QString("Text"));
        append(QString("CharacterRect"), acc.characterRect(), item);
        append(QString("CaretOffset"), acc.caretOffset(), item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Value)) {
        QStandardItem *item = append(QString("Value"));
        append(QString("Current"), acc.currentValue(), item);
        append(QString("Minimum"), acc.minimumValue(), item);
        append(QString("Maximum"), acc.maximumValue(), item);
        append(QString("Increment"), acc.minimumValueIncrement(), item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Socket)) {
        QStandardItem *item = append(QString("Socket"));
        Q_UNUSED(item);
    }

    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::EventKeyboard)) {
        QStandardItem *item = append(QString("EventKeyboard"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::EventMouse)) {
        QStandardItem *item = append(QString("EventMouse"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::EventObject)) {
        QStandardItem *item = append(QString("EventObject"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::EventWindow)) {
        QStandardItem *item = append(QString("EventWindow"));
        Q_UNUSED(item);
    }
    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::EventFocus)) {
        QStandardItem *item = append(QString("EventFocus"));
        Q_UNUSED(item);
    }

    if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Action)) {
        QStandardItem *item = append(QString("Action"));
        Q_FOREACH(QAction *a, acc.actions()) {
            QStandardItem *nameItem = new QStandardItem(a->text());
            QStandardItem *valueItem = new QStandardItem(a->whatsThis());
            item->appendRow(QList<QStandardItem*>() << nameItem << valueItem);
        }
    }

    endResetModel();
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

QString ObjectProperties::stateString(const KAccessibleClient::AccessibleObject &acc)
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
