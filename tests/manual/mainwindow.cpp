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

#include "mainwindow.h"

#include <qitemselectionmodel.h>
#include <qplaintextedit.h>
#include <qstring.h>
#include <qtreeview.h>
#include <qheaderview.h>
#include <qstandarditemmodel.h>

#include "kdeaccessibilityclient/registry.h"
#include "kdeaccessibilityclient/accessibleobject.h"

#include "accessibletree.h"

#include "tests_auto_modeltest_modeltest.h"

class ObjectProperties : public QStandardItemModel
{
public:
    explicit ObjectProperties(QObject *parent = 0)
        : QStandardItemModel(parent)
    {
    }
    virtual ~ObjectProperties() {}

    void setAccessibleObject(const KAccessibleClient::AccessibleObject &acc)
    {
        //beginResetModel();

        clear();
        setColumnCount(2);
        setHorizontalHeaderLabels( QStringList() << QString("Property") << QString("Value") );

        if (!acc.isValid()) {
            //endResetModel();
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
            // GetPosition
            // GetSize
            // GetLayer
            // GetMDIZOrder
            // GrabFocus
            // GetAlpha
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Collection)) {
            QStandardItem *item = append(QString("Collection"));
            Q_UNUSED(item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Application)) {
            QStandardItem *item = append(QString("Application"));
            Q_UNUSED(item);
            // ToolkitName
            // Version
            // Id
            // GetLocale
            // GetApplicationBusAddress
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Document)) {
            QStandardItem *item = append(QString("Document"));
            Q_UNUSED(item);
        }

        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::EditableText)) {
            QStandardItem *item = append(QString("EditableText"));
            Q_UNUSED(item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Hyperlink)) {
            QStandardItem *item = append(QString("Hyperlink"));
            Q_UNUSED(item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Hypertext)) {
            QStandardItem *item = append(QString("Hypertext"));
            Q_UNUSED(item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Image)) {
            QStandardItem *item = append(QString("Image"));
            Q_UNUSED(item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Selection)) {
            QStandardItem *item = append(QString("Selection"));
            Q_UNUSED(item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Table)) {
            QStandardItem *item = append(QString("Table"));
            Q_UNUSED(item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Text)) {
            QStandardItem *item = append(QString("Text"));
            append(QString("CharacterRect"), acc.characterRect(), item);
            append(QString("CaretOffset"), acc.caretOffset(), item);
        }
        if (interfaces.testFlag(KAccessibleClient::AccessibleObject::Value)) {
            QStandardItem *item = append(QString("Value"));
            Q_UNUSED(item);
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
                //QStandardItem *valueItem = new QStandardItem(a->objectName());
                item->appendRow(QList<QStandardItem*>() << nameItem << valueItem);
            }
        }

        //endResetModel();
    }

private:
    QStandardItem* append(const QString &name, const QVariant &value = QVariant(), QStandardItem *parentItem = 0)
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

    QString stateString(const KAccessibleClient::AccessibleObject &acc)
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

};

using namespace KAccessibleClient;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    m_registry = new KAccessibleClient::Registry(this);
    connect(m_registry, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)), this, SLOT(focusChanged(KAccessibleClient::AccessibleObject)));
    m_registry->subscribeEventListeners(KAccessibleClient::Registry::Focus);

    m_treeModel = new AccessibleTree(this);
    ui.treeView->setModel(m_treeModel);
    ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeCustomContextMenuRequested(QPoint)));
    m_treeModel->setRegistry(m_registry);

    // The ultimate model verificaton helper :p
    new ModelTest(m_treeModel, this);

    m_propertyModel = new ObjectProperties(this);
    ui.propertyView->setModel(m_propertyModel);

    connect(ui.treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChanged(QModelIndex,QModelIndex)));
    connect(ui.action_Reset_tree, SIGNAL(triggered()), m_treeModel, SLOT(resetModel()));
    ui.action_Reset_tree->setShortcut(QKeySequence(QKeySequence::Refresh));
}

void MainWindow::focusChanged(const KAccessibleClient::AccessibleObject &object)
{
    //ui.label->setText(QString("Focus changed to: %1 - %2 (%3)").arg(object.name()).arg(object.roleName()).arg(object.role()));

    if (ui.action_Follow_Focus->isChecked()) {
        QModelIndex index = m_treeModel->indexForAccessible(object);
        if (index.isValid()) {
            ui.treeView->scrollTo(index);
            ui.treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
        }
    }
    QPoint fpoint = object.focusPoint();
    //ui.statusbar->showMessage(QString("Current Focus : ( %1 , %2 )").arg(fpoint.x()).arg(fpoint.y()));
}

void MainWindow::selectionChanged(const QModelIndex& current, const QModelIndex&)
{
    KAccessibleClient::AccessibleObject acc;
    if (current.isValid() && current.internalPointer()) {
        acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    }
    m_propertyModel->setAccessibleObject(acc);
    ui.propertyView->expandAll();
    ui.propertyView->resizeColumnToContents(0);
}

void MainWindow::treeCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex current = ui.treeView->currentIndex();
    if (!current.isValid())
        return;
    KAccessibleClient::AccessibleObject acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    QMenu *menu = new QMenu(this);
    connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));
    Q_FOREACH(QAction *a, acc.actions()) {
        menu->addAction(a);
    }
    menu->popup(ui.treeView->mapToGlobal(pos));
}
