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
#include <qstring.h>
#include <qtreeview.h>
#include <qheaderview.h>
#include <qstandarditemmodel.h>
#include <qdockwidget.h>
#include <qmenubar.h>
#include <qtextdocument.h>
#include <qtextcursor.h>
#include <QTextBlock>
#include <qscrollbar.h>
#include <qsettings.h>

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
            //GetApplication
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
    m_registry = new KAccessibleClient::Registry(this);

    initUi();
    initActions();
    initMenu();

    // The ultimate model verificaton helper :p
    //new ModelTest(m_treeModel, this);

    connect(m_registry, SIGNAL(windowCreated(KAccessibleClient::AccessibleObject)), this, SLOT(windowCreated(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDestroyed(KAccessibleClient::AccessibleObject)), this, SLOT(windowDestroyed(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowClosed(KAccessibleClient::AccessibleObject)), this, SLOT(windowClosed(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowReparented(KAccessibleClient::AccessibleObject)), this, SLOT(windowReparented(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowMinimized(KAccessibleClient::AccessibleObject)), this, SLOT(windowMinimized(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowMaximized(KAccessibleClient::AccessibleObject)), this, SLOT(windowMaximized(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowRestored(KAccessibleClient::AccessibleObject)), this, SLOT(windowRestored(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowActivated(KAccessibleClient::AccessibleObject)), this, SLOT(windowActivated(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDeactivated(KAccessibleClient::AccessibleObject)), this, SLOT(windowDeactivated(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDesktopCreated(KAccessibleClient::AccessibleObject)), this, SLOT(windowDesktopCreated(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDesktopDestroyed(KAccessibleClient::AccessibleObject)), this, SLOT(windowDesktopDestroyed(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowRaised(KAccessibleClient::AccessibleObject)), this, SLOT(windowRaised(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowLowered(KAccessibleClient::AccessibleObject)), this, SLOT(windowLowered(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowMoved(KAccessibleClient::AccessibleObject)), this, SLOT(windowMoved(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowResized(KAccessibleClient::AccessibleObject)), this, SLOT(windowResized(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowShaded(KAccessibleClient::AccessibleObject)), this, SLOT(windowShaded(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowUnshaded(KAccessibleClient::AccessibleObject)), this, SLOT(windowUnshaded(KAccessibleClient::AccessibleObject)));

    connect(m_registry, SIGNAL(stateChanged(KAccessibleClient::AccessibleObject,QString,int,int,QVariant)), this, SLOT(stateChanged(KAccessibleClient::AccessibleObject,QString,int,int,QVariant)));
    connect(m_registry, SIGNAL(childrenChanged(KAccessibleClient::AccessibleObject)), this, SLOT(childrenChanged(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(visibleDataChanged(KAccessibleClient::AccessibleObject)), this, SLOT(visibleDataChanged(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(selectionChanged(KAccessibleClient::AccessibleObject)), this, SLOT(selectionChanged(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(modelChanged(KAccessibleClient::AccessibleObject)), this, SLOT(modelChanged(KAccessibleClient::AccessibleObject)));

    connect(m_registry, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)), this, SLOT(focusChanged(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(textCaretMoved(KAccessibleClient::AccessibleObject,int)), this, SLOT(textCaretMoved(KAccessibleClient::AccessibleObject,int)));
    connect(m_registry, SIGNAL(textSelectionChanged(KAccessibleClient::AccessibleObject)), this, SLOT(textSelectionChanged(KAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(textChanged(KAccessibleClient::AccessibleObject)), this, SLOT(textChanged(KAccessibleClient::AccessibleObject)));

    //m_registry->subscribeEventListeners(KAccessibleClient::Registry::Focus);
    m_registry->subscribeEventListeners(KAccessibleClient::Registry::AllEventListeners);

    QSettings settings("kde.org", "kdea11yapp");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("kde.org", "kdea11yapp");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::MainWindow::initActions()
{
    m_resetTreeAction = new QAction(this);
    m_resetTreeAction->setText(QString("Reset Tree"));
    m_resetTreeAction->setShortcut(QKeySequence(QKeySequence::Refresh));
    connect(m_resetTreeAction, SIGNAL(triggered()), m_treeModel, SLOT(resetModel()));

    m_followFocusAction = new QAction(this);
    m_followFocusAction->setText(QString("Follow Focus"));
    m_followFocusAction->setCheckable(true);
    m_followFocusAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

    m_quitAction = new QAction(tr("&Quit"), this);
    m_quitAction->setShortcuts(QKeySequence::Quit);
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::MainWindow::initMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(QString("File"));
    fileMenu->addAction(m_resetTreeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_quitAction);

    QMenu *settingsMenu = menuBar()->addMenu(QString("Settings"));
    QMenu *dockerMenu = settingsMenu->addMenu(QString("Docker"));
    Q_FOREACH(const QDockWidget *docker, findChildren<QDockWidget*>()) {
        dockerMenu->addAction(docker->toggleViewAction());
    }
    settingsMenu->addAction(m_followFocusAction);
}

void MainWindow::MainWindow::initUi()
{
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

    QDockWidget *treeDocker = new QDockWidget(QString("Tree"), this);
    treeDocker->setObjectName("tree");
    treeDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_treeView = new QTreeView(treeDocker);
    m_treeView->setAccessibleName(QString("Tree of accessibles"));
    m_treeView->setAccessibleDescription(QString("Displays a hierachical tree of accessible objects"));
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeDocker->setWidget(m_treeView);
    addDockWidget(Qt::LeftDockWidgetArea, treeDocker);

    m_treeModel = new AccessibleTree(this);
    m_treeModel->setRegistry(m_registry);
    m_treeView->setModel(m_treeModel);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChanged(QModelIndex,QModelIndex)));
    connect(m_treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeCustomContextMenuRequested(QPoint)));

    QDockWidget *propertyDocker = new QDockWidget(QString("Properties"), this);
    propertyDocker->setObjectName("properties");
    propertyDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_propertyView = new QTreeView(propertyDocker);
    propertyDocker->setWidget(m_propertyView);
    m_propertyView->setAccessibleName(QString("List of properties"));
    m_propertyView->setAccessibleDescription(QString("Displays a the properties of the selected accessible object"));
    m_propertyView->setRootIsDecorated(false);
    m_propertyView->setItemsExpandable(false);
    m_propertyView->setExpandsOnDoubleClick(false);
    m_propertyView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_propertyModel = new ObjectProperties(this);
    m_propertyView->setModel(m_propertyModel);
    addDockWidget(Qt::RightDockWidgetArea, propertyDocker);

    QDockWidget *eventsDocker = new QDockWidget(QString("Events"), this);
    eventsDocker->setObjectName("events");
    eventsDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_eventsEdit = new QTextBrowser(eventsDocker);
    eventsDocker->setWidget(m_eventsEdit);
    addDockWidget(Qt::RightDockWidgetArea, eventsDocker);

    resize(QSize(760,520));
}

void MainWindow::MainWindow::addLog(const KAccessibleClient::AccessibleObject &object, const QString &eventName, const QString &text)
{
    QTextDocument *doc = m_eventsEdit->document();
    doc->blockSignals(true); // to prevent infinte TextCaretMoved events
    QTextCursor cursor(doc->lastBlock());
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    QString s = QString("%1: %2 (%3)").arg(eventName).arg(object.name()).arg(object.roleName());
    cursor.insertText(s);
    if (!text.isEmpty())
        cursor.insertText(" " + text);
    cursor.insertBlock();
    doc->blockSignals(false);
//     m_eventsEdit->ensureCursorVisible();
    m_eventsEdit->verticalScrollBar()->setValue(m_eventsEdit->verticalScrollBar()->maximum());
}

void MainWindow::stateChanged(const KAccessibleClient::AccessibleObject &object, const QString &state, int detail1, int detail2, const QVariant &args)
{
    QString s = QString("%1").arg(state);
    addLog(object, QString("StateChanged"), s);
}

void MainWindow::childrenChanged(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("ChildrenChanged"));
}

void MainWindow::visibleDataChanged(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("VisibleDataChanged"));
}

void MainWindow::selectionChanged(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("SelectionChanged"));
}

void MainWindow::modelChanged(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("ModelChanged"));
}

void MainWindow::MainWindow::selectionChanged(const QModelIndex& current, const QModelIndex&)
{
    KAccessibleClient::AccessibleObject acc;
    if (current.isValid() && current.internalPointer()) {
        acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    }
    m_propertyModel->setAccessibleObject(acc);
    m_propertyView->expandAll();
    m_propertyView->resizeColumnToContents(0);
}

void MainWindow::MainWindow::treeCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex current = m_treeView->currentIndex();
    if (!current.isValid())
        return;
    KAccessibleClient::AccessibleObject acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    QMenu *menu = new QMenu(this);
    connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));
    Q_FOREACH(QAction *a, acc.actions()) {
        menu->addAction(a);
    }
    menu->popup(m_treeView->mapToGlobal(pos));
}

void MainWindow::windowCreated(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowCreate"));
}

void MainWindow::windowDestroyed(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowDestroy"));
}

void MainWindow::windowClosed(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowClose"));
}

void MainWindow::windowReparented(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowReparent"));
}

void MainWindow::windowMinimized(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowMinimize"));
}

void MainWindow::windowMaximized(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowMaximize"));
}

void MainWindow::windowRestored(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowRestore"));
}

void MainWindow::windowActivated(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowActivate"));
}

void MainWindow::windowDeactivated(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowDeactivate"));
}

void MainWindow::windowDesktopCreated(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowDesktopCreate"));
}

void MainWindow::windowDesktopDestroyed(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowDesktopDestroy"));
}

void MainWindow::windowRaised(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowRaise"));
}

void MainWindow::windowLowered(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowLower"));
}

void MainWindow::windowMoved(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowMove"));
}

void MainWindow::windowResized(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowResize"));
}

void MainWindow::windowShaded(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowShade"));
}

void MainWindow::windowUnshaded(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("WindowUnshade"));
}

void MainWindow::focusChanged(const KAccessibleClient::AccessibleObject &object)
{
    if (m_followFocusAction->isChecked()) {
        QModelIndex index = m_treeModel->indexForAccessible(object);
        if (index.isValid()) {
            m_treeView->scrollTo(index);
            m_treeView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
        } else {
            qWarning() << "No such indexForAccessible=" << object;
        }
    }
    //QPoint fpoint = object.focusPoint();
    //ui.statusbar->showMessage(QString("Current Focus : ( %1 , %2 )").arg(fpoint.x()).arg(fpoint.y()));
    //addLog(object, QString("Focus"));
}

void MainWindow::MainWindow::textCaretMoved(const KAccessibleClient::AccessibleObject &object, int pos)
{
    addLog(object, QString("TextCaretMoved"));
}

void MainWindow::MainWindow::textSelectionChanged(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("TextSelectionChanged"));
}

void MainWindow::textChanged(const KAccessibleClient::AccessibleObject &object)
{
    addLog(object, QString("TextChanged"));
}
