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

#include <qdialog.h>
#include <qdockwidget.h>
#include <qmenubar.h>
#include <qpointer.h>
#include <qsettings.h>
#include <qurl.h>

#include "qaccessibilityclient/registry.h"
#include "qaccessibilityclient/registrycache_p.h"
#include "qaccessibilityclient/accessibleobject.h"

#include "accessibleproperties.h"
#include "accessibletree.h"
#include "eventview.h"
#include "uiview.h"

using namespace QAccessibleClient;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QLatin1String("Randamizer"));
    m_registry = new QAccessibleClient::Registry(this);

    initUi();
    initActions();
    initMenu();

    QSettings settings("kde.org", "kdea11yapp");
    QAccessibleClient::RegistryPrivateCacheApi cache(m_registry);
    cache.setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(settings.value("cacheStrategy", cache.cacheType()).toInt()));
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    m_eventsWidget->loadSettings(settings);

    connect(m_registry, &Registry::added, this, &MainWindow::added);
    connect(m_registry, &Registry::removed, this, &MainWindow::removed);
    connect(m_registry, &Registry::defunct, this, &MainWindow::defunct);

    connect(m_registry, &Registry::windowCreated, this, &MainWindow::windowCreated);
    connect(m_registry, &Registry::windowDestroyed, this, &MainWindow::windowDestroyed);
    connect(m_registry, &Registry::windowClosed, this, &MainWindow::windowClosed);
    connect(m_registry, &Registry::windowReparented, this, &MainWindow::windowReparented);
    connect(m_registry, &Registry::windowMinimized, this, &MainWindow::windowMinimized);
    connect(m_registry, &Registry::windowMaximized, this, &MainWindow::windowMaximized);
    connect(m_registry, &Registry::windowRestored, this, &MainWindow::windowRestored);
    connect(m_registry, &Registry::windowActivated, this, &MainWindow::windowActivated);
    connect(m_registry, &Registry::windowDeactivated, this, &MainWindow::windowDeactivated);
    connect(m_registry, &Registry::windowDesktopCreated, this, &MainWindow::windowDesktopCreated);
    connect(m_registry, &Registry::windowDesktopDestroyed, this, &MainWindow::windowDesktopDestroyed);
    connect(m_registry, &Registry::windowRaised, this, &MainWindow::windowRaised);
    connect(m_registry, &Registry::windowLowered, this, &MainWindow::windowLowered);
    connect(m_registry, &Registry::windowMoved, this, &MainWindow::windowMoved);
    connect(m_registry, &Registry::windowResized, this, &MainWindow::windowResized);
    connect(m_registry, &Registry::windowShaded, this, &MainWindow::windowShaded);
    connect(m_registry, &Registry::windowUnshaded, this, &MainWindow::windowUnshaded);

    connect(m_registry, &Registry::stateChanged, this, &MainWindow::stateChanged);
    connect(m_registry, &Registry::childAdded, this, &MainWindow::childAdded);
    connect(m_registry, &Registry::childRemoved, this, &MainWindow::childRemoved);
    connect(m_registry, &Registry::visibleDataChanged, this, &MainWindow::visibleDataChanged);
    connect(m_registry, &Registry::selectionChanged, this, &MainWindow::selectionChanged);
    connect(m_registry, &Registry::modelChanged, this, &MainWindow::modelChanged);

    connect(m_registry, &Registry::focusChanged, this, &MainWindow::focusChanged);
    connect(m_registry, &Registry::textCaretMoved, this, &MainWindow::textCaretMoved);
    connect(m_registry, &Registry::textSelectionChanged, this, &MainWindow::textSelectionChanged);

    connect(m_registry, &Registry::textInserted, this, &MainWindow::textInserted);
    connect(m_registry, &Registry::textRemoved, this, &MainWindow::textRemoved);
    connect(m_registry, &Registry::textChanged, this, &MainWindow::textChanged);

    connect(m_registry, &Registry::accessibleNameChanged, this, &MainWindow::accessibleNameChanged);
    connect(m_registry, &Registry::accessibleDescriptionChanged, this, &MainWindow::accessibleDescriptionChanged);

    m_registry->subscribeEventListeners(QAccessibleClient::Registry::AllEventListeners);
}

MainWindow::~MainWindow()
{
    delete m_registry;
}

void MainWindow::MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("kde.org", "kdea11yapp");
    settings.setValue("cacheStrategy", int(QAccessibleClient::RegistryPrivateCacheApi(m_registry).cacheType()));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    m_eventsWidget->saveSettings(settings);

    settings.sync();

    QMainWindow::closeEvent(event);
}

void MainWindow::MainWindow::initActions()
{
    m_resetTreeAction = new QAction(this);
    m_resetTreeAction->setText(QString("Reset Tree"));
    m_resetTreeAction->setShortcut(QKeySequence(QKeySequence::Refresh));
    connect(m_resetTreeAction, &QAction::triggered, m_accessibleObjectTreeModel, &AccessibleTree::resetModel);

    m_followFocusAction = new QAction(this);
    m_followFocusAction->setText(QString("Follow Focus"));
    m_followFocusAction->setCheckable(true);
    m_followFocusAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));

    m_showClientCacheAction = new QAction(this);
    m_showClientCacheAction->setText(QString("Cache..."));
    connect(m_showClientCacheAction, SIGNAL(triggered()), this, SLOT(showClientCache()));

    m_enableA11yAction = new QAction(this);
    m_enableA11yAction->setText(QString("Enable Accessibility"));
    m_enableA11yAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    m_enableA11yAction->setCheckable(true);
    m_enableA11yAction->setChecked(m_registry->isEnabled());
    connect(m_registry, SIGNAL(enabledChanged(bool)), m_enableA11yAction, SLOT(setChecked(bool)));
    connect(m_enableA11yAction, SIGNAL(toggled(bool)), m_registry, SLOT(setEnabled(bool)));

    m_enableScreenReaderAction = new QAction(this);
    m_enableScreenReaderAction->setText(QString("Enable Screen Reader"));
    m_enableScreenReaderAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    m_enableScreenReaderAction->setCheckable(true);
    m_enableScreenReaderAction->setChecked(m_registry->isScreenReaderEnabled());
    connect(m_registry, SIGNAL(screenReaderEnabledChanged(bool)), m_enableScreenReaderAction, SLOT(setChecked(bool)));
    connect(m_enableScreenReaderAction, SIGNAL(toggled(bool)), m_registry, SLOT(setScreenReaderEnabled(bool)));

    m_quitAction = new QAction(tr("&Quit"), this);
    m_quitAction->setShortcuts(QKeySequence::Quit);
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::MainWindow::initMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(QString("Tree"));
    fileMenu->addAction(m_resetTreeAction);
    fileMenu->addAction(m_followFocusAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_quitAction);

    QMenu *settingsMenu = menuBar()->addMenu(QString("Settings"));
    QMenu *dockerMenu = settingsMenu->addMenu(QString("Docker"));
    Q_FOREACH(const QDockWidget *docker, findChildren<QDockWidget*>()) {
        dockerMenu->addAction(docker->toggleViewAction());
    }
    settingsMenu->addAction(m_showClientCacheAction);
    settingsMenu->addSeparator();
    settingsMenu->addAction(m_enableA11yAction);
    settingsMenu->addAction(m_enableScreenReaderAction);
}

void MainWindow::MainWindow::initUi()
{
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
    //setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    QDockWidget *treeDocker = new QDockWidget(QString("Tree"), this);
    treeDocker->setObjectName("tree");
    treeDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_accessibleObjectTreeView = new QTreeView(treeDocker);
    m_accessibleObjectTreeView->setAccessibleName(QString("Tree of accessibles"));
    m_accessibleObjectTreeView->setAccessibleDescription(QString("Displays a hierachical tree of accessible objects"));
    m_accessibleObjectTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_accessibleObjectTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_accessibleObjectTreeView->setAlternatingRowColors(true);
    treeDocker->setWidget(m_accessibleObjectTreeView);

    m_accessibleObjectTreeModel = new AccessibleTree(this);
    m_accessibleObjectTreeModel->setRegistry(m_registry);
    m_accessibleObjectTreeView->setModel(m_accessibleObjectTreeModel);
    m_accessibleObjectTreeView->setColumnWidth(0, 240);
    m_accessibleObjectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_accessibleObjectTreeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::treeSelectionChanged);
    connect(m_accessibleObjectTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::treeCustomContextMenuRequested);

    QDockWidget *propertyDocker = new QDockWidget(QString("Properties"), this);
    propertyDocker->setObjectName("properties");
    propertyDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_propertyView = new QTreeView(propertyDocker);
    propertyDocker->setWidget(m_propertyView);
    m_propertyView->setAccessibleName(QString("List of properties"));
    m_propertyView->setAccessibleDescription(QString("Displays a the properties of the selected accessible object"));
    m_propertyView->setRootIsDecorated(false);
    m_propertyView->setItemsExpandable(true);
    m_propertyView->setExpandsOnDoubleClick(false);
    m_propertyView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_propertyModel = new ObjectProperties(this);
    m_propertyView->setModel(m_propertyModel);
    m_propertyView->setAlternatingRowColors(true);
    connect(m_propertyView, SIGNAL(doubleClicked(QModelIndex)), m_propertyModel, SLOT(doubleClicked(QModelIndex)));

    QDockWidget *uiDocker = new QDockWidget(QString("Boundaries"), this);
    uiDocker->setObjectName("boundaries");
    uiDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_uiview = new UiView(uiDocker);
    m_uiview->setAccessibleName(QLatin1String("Boundaries"));
    m_uiview->setAccessibleDescription(QString("Visualize the component boundaries"));
    uiDocker->setWidget(m_uiview);

    QDockWidget *eventsDocker = new QDockWidget(QString("Events"), this);
    eventsDocker->setObjectName("events");
    eventsDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_eventsWidget = new EventsWidget(m_registry, eventsDocker);
    connect(m_eventsWidget, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));
    eventsDocker->setWidget(m_eventsWidget);

    addDockWidget(Qt::LeftDockWidgetArea, treeDocker);
    addDockWidget(Qt::RightDockWidgetArea, propertyDocker);
    addDockWidget(Qt::RightDockWidgetArea, uiDocker);
    addDockWidget(Qt::RightDockWidgetArea, eventsDocker);
    tabifyDockWidget(uiDocker, eventsDocker);
    tabifyDockWidget(uiDocker, propertyDocker);

    resize(minimumSize().expandedTo(QSize(760,520)));
}

void MainWindow::anchorClicked(const QUrl &url)
{
    AccessibleObject *object = m_registry->accessibleFromUrl(url);
    setCurrentObject(object);
}

void MainWindow::showClientCache()
{
    QPointer<ClientCacheDialog> dlg(new ClientCacheDialog(m_registry, this));
    dlg->setWindowTitle(m_showClientCacheAction->text());
    if (dlg->exec() == QDialog::Accepted && dlg) {
    }
    if (dlg)
        dlg->deleteLater();
}

void MainWindow::setCurrentObject(QAccessibleClient::AccessibleObject *object)
{
    QModelIndex index = m_accessibleObjectTreeModel->indexForAccessible(object);
    if (index.isValid()) {
        QModelIndex other = m_accessibleObjectTreeModel->index(index.row(), index.column()+1, index.parent());
        Q_ASSERT(other.isValid());
        m_accessibleObjectTreeView->selectionModel()->select(QItemSelection(index, other), QItemSelectionModel::SelectCurrent);
        m_accessibleObjectTreeView->scrollTo(index);

        // Unlike calling setCurrentIndex the select call aboves doe not emit the selectionChanged signal. So, do explicit.
        treeSelectionChanged(index, QModelIndex());
    } else {
        qWarning() << "No such indexForAccessible=" << object;
    }
}

void MainWindow::updateDetails(AccessibleObject *object, bool force)
{
    if (!force && object != m_propertyModel->currentObject())
        return;

    m_propertyModel->setAccessibleObject(object);
    for(int r = m_propertyModel->rowCount() - 1; r >= 0; --r)
        m_propertyView->setExpanded(m_propertyModel->indexFromItem(m_propertyModel->item(r, 0)), true);
    m_propertyView->resizeColumnToContents(0);
}

void MainWindow::stateChanged(QAccessibleClient::AccessibleObject *object, const QString &state, bool active)
{
    if (state == QLatin1String("focused")) {
        m_eventsWidget->addLog(object, EventsWidget::Focus, (active ? QString("true") : QString("false")));
    } else {
        QString s = state + QString(": ") + (active ? QString("true") : QString("false"));
        m_eventsWidget->addLog(object, EventsWidget::StateChanged, s);
    }
    updateDetails(object);
}

void MainWindow::childAdded(QAccessibleClient::AccessibleObject *object, int childIndex)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildAdded ") + QString::number(childIndex));
}

void MainWindow::childRemoved(QAccessibleClient::AccessibleObject *object, int childIndex)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildRemoved ") + QString::number(childIndex));
}

void MainWindow::visibleDataChanged(QAccessibleClient::AccessibleObject *object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, "VisibleDataChanged");
}

void MainWindow::selectionChanged(QAccessibleClient::AccessibleObject *object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, "SelectionChanged");
}

void MainWindow::modelChanged(QAccessibleClient::AccessibleObject *object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, "ModelChanged");
}

void MainWindow::treeSelectionChanged(const QModelIndex& current, const QModelIndex&)
{
    QAccessibleClient::AccessibleObject *acc;
    if (current.isValid() && current.internalPointer()) {
        acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    }
    m_uiview->setAccessibleObject(acc);
    updateDetails(acc, true);
}

void MainWindow::treeCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex current = m_accessibleObjectTreeView->currentIndex();
    if (!current.isValid())
        return;
    QAccessibleClient::AccessibleObject *acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    QMenu *menu = new QMenu(this);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    Q_FOREACH(const QSharedPointer<QAction> &a, acc->actions()) {
        menu->addAction(a.data());
    }
    menu->popup(m_accessibleObjectTreeView->mapToGlobal(pos));
}

void MainWindow::added(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, QString("Add Object"));
    m_accessibleObjectTreeModel->addAccessible(object);
}

void MainWindow::removed(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, "RemoveObject");
    m_accessibleObjectTreeModel->removeAccessible(object);
}

void MainWindow::defunct(QAccessibleClient::AccessibleObject *object)
{
    removed(object);
}

void MainWindow::windowCreated(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, "Create");
    m_accessibleObjectTreeModel->addAccessible(object);
}

void MainWindow::windowDestroyed(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("Destroy"));
    if (!m_accessibleObjectTreeModel->removeAccessible(object)) {
        // assume the app has gone
        m_accessibleObjectTreeModel->updateTopLevelApps();
    }
}

void MainWindow::windowClosed(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowClose"));
}

void MainWindow::windowReparented(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowReparent"));
}

void MainWindow::windowMinimized(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowMinimize"));
}

void MainWindow::windowMaximized(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowMaximize"));
}

void MainWindow::windowRestored(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowRestore"));
}

void MainWindow::windowActivated(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowActivate"));
}

void MainWindow::windowDeactivated(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowDeactivate"));
}

void MainWindow::windowDesktopCreated(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowDesktopCreate"));
}

void MainWindow::windowDesktopDestroyed(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowDesktopDestroy"));
}

void MainWindow::windowRaised(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowRaise"));
}

void MainWindow::windowLowered(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowLower"));
}

void MainWindow::windowMoved(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowMove"));
}

void MainWindow::windowResized(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowResize"));
}

void MainWindow::windowShaded(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowShade"));
}

void MainWindow::windowUnshaded(QAccessibleClient::AccessibleObject *object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowUnshade"));
}

void MainWindow::focusChanged(QAccessibleClient::AccessibleObject *object)
{
    if (m_followFocusAction->isChecked()) {
        // We need to block the focus for the treeView while setting the current item
        // to prevent that setting that item would change focus to the treeView.
        Qt::FocusPolicy prevFocusPolicy = m_accessibleObjectTreeView->focusPolicy();
        m_accessibleObjectTreeView->setFocusPolicy(Qt::NoFocus);

        setCurrentObject(object);

        m_accessibleObjectTreeView->setFocusPolicy(prevFocusPolicy);
    } else {
        updateDetails(object);
    }
}

void MainWindow::MainWindow::textCaretMoved(QAccessibleClient::AccessibleObject *object, int pos)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, QString("Text caret moved (%1)").arg(pos));
}

void MainWindow::MainWindow::textSelectionChanged(QAccessibleClient::AccessibleObject *object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, QString("TextSelectionChanged"));
}

QString descriptionForText(const QString& type, const QString& text, int startOffset, int endOffset)
{
    QString shortText = text;
    if (shortText.length() > 50) {
        shortText.truncate(50);
        shortText.append(QLatin1String("..."));
    }
    QString desc = QString("Text %1 (%2, %3): \"%4\"").arg(type).arg(QString::number(startOffset)).arg(QString::number(endOffset)).arg(shortText);
    return desc;
}

void MainWindow::textChanged(QAccessibleClient::AccessibleObject *object, const QString& text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("changed"), text, startOffset, endOffset));
}

void MainWindow::textInserted(QAccessibleClient::AccessibleObject *object, const QString& text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("inserted"), text, startOffset, endOffset));
}

void MainWindow::textRemoved(QAccessibleClient::AccessibleObject *object, const QString& text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("removed"), text, startOffset, endOffset));
}

void MainWindow::accessibleNameChanged(QAccessibleClient::AccessibleObject *object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::NameChanged);
    m_accessibleObjectTreeModel->updateAccessible(object);
}

void MainWindow::accessibleDescriptionChanged(QAccessibleClient::AccessibleObject *object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::DescriptionChanged);
    m_accessibleObjectTreeModel->updateAccessible(object);
}

