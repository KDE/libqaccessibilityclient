/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "mainwindow.h"

#include <QDialog>
#include <QDockWidget>
#include <QMenuBar>
#include <QPointer>
#include <QSettings>
#include <qurl.h>
#include <QClipboard>

#include "qaccessibilityclient/registrycache_p.h"

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

    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("kdea11yapp"));
    QAccessibleClient::RegistryPrivateCacheApi cache(m_registry);
    cache.setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(settings.value(QStringLiteral("cacheStrategy"), cache.cacheType()).toInt()));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("windowState")).toByteArray());

    m_eventsWidget->loadSettings(settings);

    connect(m_registry, SIGNAL(added(QAccessibleClient::AccessibleObject)), this, SLOT(added(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(removed(QAccessibleClient::AccessibleObject)), this, SLOT(removed(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(defunct(QAccessibleClient::AccessibleObject)), this, SLOT(defunct(QAccessibleClient::AccessibleObject)));

    connect(m_registry, SIGNAL(windowCreated(QAccessibleClient::AccessibleObject)), this, SLOT(windowCreated(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDestroyed(QAccessibleClient::AccessibleObject)), this, SLOT(windowDestroyed(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowClosed(QAccessibleClient::AccessibleObject)), this, SLOT(windowClosed(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowReparented(QAccessibleClient::AccessibleObject)), this, SLOT(windowReparented(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowMinimized(QAccessibleClient::AccessibleObject)), this, SLOT(windowMinimized(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowMaximized(QAccessibleClient::AccessibleObject)), this, SLOT(windowMaximized(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowRestored(QAccessibleClient::AccessibleObject)), this, SLOT(windowRestored(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowActivated(QAccessibleClient::AccessibleObject)), this, SLOT(windowActivated(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDeactivated(QAccessibleClient::AccessibleObject)), this, SLOT(windowDeactivated(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDesktopCreated(QAccessibleClient::AccessibleObject)), this, SLOT(windowDesktopCreated(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowDesktopDestroyed(QAccessibleClient::AccessibleObject)), this, SLOT(windowDesktopDestroyed(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowRaised(QAccessibleClient::AccessibleObject)), this, SLOT(windowRaised(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowLowered(QAccessibleClient::AccessibleObject)), this, SLOT(windowLowered(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowMoved(QAccessibleClient::AccessibleObject)), this, SLOT(windowMoved(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowResized(QAccessibleClient::AccessibleObject)), this, SLOT(windowResized(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowShaded(QAccessibleClient::AccessibleObject)), this, SLOT(windowShaded(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(windowUnshaded(QAccessibleClient::AccessibleObject)), this, SLOT(windowUnshaded(QAccessibleClient::AccessibleObject)));

    connect(m_registry, SIGNAL(stateChanged(QAccessibleClient::AccessibleObject,QString,bool)), this, SLOT(stateChanged(QAccessibleClient::AccessibleObject,QString,bool)));
    connect(m_registry, SIGNAL(childAdded(QAccessibleClient::AccessibleObject,int)), this, SLOT(childAdded(QAccessibleClient::AccessibleObject,int)));
    connect(m_registry, SIGNAL(childRemoved(QAccessibleClient::AccessibleObject,int)), this, SLOT(childRemoved(QAccessibleClient::AccessibleObject,int)));
    connect(m_registry, SIGNAL(visibleDataChanged(QAccessibleClient::AccessibleObject)), this, SLOT(visibleDataChanged(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(selectionChanged(QAccessibleClient::AccessibleObject)), this, SLOT(selectionChanged(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(modelChanged(QAccessibleClient::AccessibleObject)), this, SLOT(modelChanged(QAccessibleClient::AccessibleObject)));

    connect(m_registry, SIGNAL(focusChanged(QAccessibleClient::AccessibleObject)), this, SLOT(focusChanged(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(textCaretMoved(QAccessibleClient::AccessibleObject,int)), this, SLOT(textCaretMoved(QAccessibleClient::AccessibleObject,int)));
    connect(m_registry, SIGNAL(textSelectionChanged(QAccessibleClient::AccessibleObject)), this, SLOT(textSelectionChanged(QAccessibleClient::AccessibleObject)));

    connect(m_registry, SIGNAL(textInserted(QAccessibleClient::AccessibleObject,QString,int,int)), this, SLOT(textInserted(QAccessibleClient::AccessibleObject,QString,int,int)));
    connect(m_registry, SIGNAL(textRemoved(QAccessibleClient::AccessibleObject,QString,int,int)), this, SLOT(textRemoved(QAccessibleClient::AccessibleObject,QString,int,int)));
    connect(m_registry, SIGNAL(textChanged(QAccessibleClient::AccessibleObject,QString,int,int)), this, SLOT(textChanged(QAccessibleClient::AccessibleObject,QString,int,int)));

    connect(m_registry, SIGNAL(accessibleNameChanged(QAccessibleClient::AccessibleObject)), this, SLOT(accessibleNameChanged(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(accessibleDescriptionChanged(QAccessibleClient::AccessibleObject)), this, SLOT(accessibleDescriptionChanged(QAccessibleClient::AccessibleObject)));

    m_registry->subscribeEventListeners(QAccessibleClient::Registry::AllEventListeners);
}

MainWindow::~MainWindow()
{
    delete m_registry;
}

void MainWindow::MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(QStringLiteral("kde.org"), QStringLiteral("kdea11yapp"));
    settings.setValue(QStringLiteral("cacheStrategy"), int(QAccessibleClient::RegistryPrivateCacheApi(m_registry).cacheType()));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("windowState"), saveState());

    m_eventsWidget->saveSettings(settings);

    settings.sync();

    QMainWindow::closeEvent(event);
}

void MainWindow::MainWindow::initActions()
{
    m_resetTreeAction = new QAction(this);
    m_resetTreeAction->setText(QStringLiteral("Reset Tree"));
    m_resetTreeAction->setShortcut(QKeySequence(QKeySequence::Refresh));
    connect(m_resetTreeAction, SIGNAL(triggered()), m_accessibleObjectTreeModel, SLOT(resetModel()));

    m_followFocusAction = new QAction(this);
    m_followFocusAction->setText(QStringLiteral("Follow Focus"));
    m_followFocusAction->setCheckable(true);
    m_followFocusAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));

    m_showClientCacheAction = new QAction(this);
    m_showClientCacheAction->setText(QStringLiteral("Cache..."));
    connect(m_showClientCacheAction, SIGNAL(triggered()), this, SLOT(showClientCache()));

    m_enableA11yAction = new QAction(this);
    m_enableA11yAction->setText(QStringLiteral("Enable Accessibility"));
    m_enableA11yAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_enableA11yAction->setCheckable(true);
    m_enableA11yAction->setChecked(m_registry->isEnabled());
    connect(m_registry, SIGNAL(enabledChanged(bool)), m_enableA11yAction, SLOT(setChecked(bool)));
    connect(m_enableA11yAction, SIGNAL(toggled(bool)), m_registry, SLOT(setEnabled(bool)));

    m_enableScreenReaderAction = new QAction(this);
    m_enableScreenReaderAction->setText(QStringLiteral("Enable Screen Reader"));
    m_enableScreenReaderAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    m_enableScreenReaderAction->setCheckable(true);
    m_enableScreenReaderAction->setChecked(m_registry->isScreenReaderEnabled());
    connect(m_registry, SIGNAL(screenReaderEnabledChanged(bool)), m_enableScreenReaderAction, SLOT(setChecked(bool)));
    connect(m_enableScreenReaderAction, SIGNAL(toggled(bool)), m_registry, SLOT(setScreenReaderEnabled(bool)));

    m_quitAction = new QAction(tr("&Quit"), this);
    m_quitAction->setShortcuts(QKeySequence::Quit);
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));

    m_copyValueAction = new QAction(tr("&Copy property value"), this);
    m_copyValueAction->setShortcuts(QKeySequence::Copy);
    connect(m_copyValueAction, SIGNAL(triggered()), this, SLOT(copyValue()));
}

void MainWindow::copyValue()
{
    QModelIndex selected = m_propertyView->currentIndex();

    if (!selected.isValid())
        return;

    if (selected.column() == 0) {
        selected = m_propertyView->model()->index(selected.row(), 1, selected.parent());
        if (!selected.isValid())
            return;
    }

    QGuiApplication::clipboard()->setText(selected.data(Qt::DisplayRole).toString());
}

void MainWindow::MainWindow::initMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(QStringLiteral("&Tree"));
    fileMenu->addAction(m_resetTreeAction);
    fileMenu->addAction(m_followFocusAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_quitAction);

    QMenu *editMenu = menuBar()->addMenu(QStringLiteral("&Edit"));
    editMenu->addAction(m_copyValueAction);

    QMenu *settingsMenu = menuBar()->addMenu(QStringLiteral("&Settings"));
    QMenu *dockerMenu = settingsMenu->addMenu(QStringLiteral("Docker"));
    for (const QDockWidget *docker : findChildren<QDockWidget*>()) {
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

    QDockWidget *treeDocker = new QDockWidget(QStringLiteral("Tree"), this);
    treeDocker->setObjectName(QStringLiteral("tree"));
    treeDocker->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    m_accessibleObjectTreeView = new QTreeView(treeDocker);
    m_accessibleObjectTreeView->setAccessibleName(QStringLiteral("Tree of accessibles"));
    m_accessibleObjectTreeView->setAccessibleDescription(QStringLiteral("Displays a hierachical tree of accessible objects"));
    m_accessibleObjectTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_accessibleObjectTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_accessibleObjectTreeView->setAlternatingRowColors(true);
    treeDocker->setWidget(m_accessibleObjectTreeView);

    m_accessibleObjectTreeModel = new AccessibleTree(this);
    m_accessibleObjectTreeModel->setRegistry(m_registry);
    m_accessibleObjectTreeView->setModel(m_accessibleObjectTreeModel);
    m_accessibleObjectTreeView->setColumnWidth(0, 240);
    m_accessibleObjectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_accessibleObjectTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChanged(QModelIndex,QModelIndex)));
    connect(m_accessibleObjectTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeCustomContextMenuRequested(QPoint)));

    QDockWidget *propertyDocker = new QDockWidget(QStringLiteral("&Properties"), this);
    propertyDocker->setObjectName(QStringLiteral("properties"));
    propertyDocker->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    m_propertyView = new QTreeView(propertyDocker);
    propertyDocker->setWidget(m_propertyView);
    m_propertyView->setAccessibleName(QStringLiteral("List of properties"));
    m_propertyView->setAccessibleDescription(QStringLiteral("Displays a the properties of the selected accessible object"));
    m_propertyView->setRootIsDecorated(false);
    m_propertyView->setItemsExpandable(true);
    m_propertyView->setExpandsOnDoubleClick(false);
    m_propertyView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::AnyKeyPressed | QAbstractItemView::EditKeyPressed);
    m_propertyModel = new ObjectProperties(this);
    m_propertyView->setModel(m_propertyModel);
    m_propertyView->setAlternatingRowColors(true);
    connect(m_propertyView, SIGNAL(doubleClicked(QModelIndex)), m_propertyModel, SLOT(doubleClicked(QModelIndex)));

    QDockWidget *uiDocker = new QDockWidget(QStringLiteral("&Boundaries"), this);
    uiDocker->setObjectName(QStringLiteral("boundaries"));
    uiDocker->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    m_uiview = new UiView(uiDocker);
    m_uiview->setAccessibleName(QStringLiteral("Boundaries"));
    m_uiview->setAccessibleDescription(QStringLiteral("Visualize the component boundaries"));
    uiDocker->setWidget(m_uiview);

    QDockWidget *eventsDocker = new QDockWidget(QStringLiteral("E&vents"), this);
    eventsDocker->setObjectName(QStringLiteral("events"));
    eventsDocker->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
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
    AccessibleObject object = m_registry->accessibleFromUrl(url);
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

void MainWindow::setCurrentObject(const QAccessibleClient::AccessibleObject &object)
{
    QModelIndex index = m_accessibleObjectTreeModel->indexForAccessible(object);
    if (index.isValid()) {
        QModelIndex other = m_accessibleObjectTreeModel->index(index.row(), index.column()+1, index.parent());
        Q_ASSERT(other.isValid());
        m_accessibleObjectTreeView->selectionModel()->select(QItemSelection(index, other), QItemSelectionModel::SelectCurrent);
        m_accessibleObjectTreeView->scrollTo(index);

        // Unlike calling setCurrentIndex the select call aboves doe not emit the selectionChanged signal. So, do explicit.
        selectionChanged(index, QModelIndex());
    } else {
        qWarning() << "No such indexForAccessible=" << object;
    }
}

void MainWindow::updateDetails(const AccessibleObject &object, bool force)
{
    if (!force && object != m_propertyModel->currentObject())
        return;

    m_propertyModel->setAccessibleObject(object);
    for(int r = m_propertyModel->rowCount() - 1; r >= 0; --r)
        m_propertyView->setExpanded(m_propertyModel->indexFromItem(m_propertyModel->item(r, 0)), true);
    m_propertyView->resizeColumnToContents(0);
}

void MainWindow::stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active)
{
    if (state == QLatin1String("focused")) {
        m_eventsWidget->addLog(object, EventsWidget::Focus, (active ? QStringLiteral("true") : QStringLiteral("false")));
    } else {
        QString s = state + QStringLiteral(": ") + (active ? QStringLiteral("true") : QStringLiteral("false"));
        m_eventsWidget->addLog(object, EventsWidget::StateChanged, s);
    }
    updateDetails(object);
}

void MainWindow::childAdded(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildAdded ") + QString::number(childIndex));
}

void MainWindow::childRemoved(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildRemoved ") + QString::number(childIndex));
}

void MainWindow::visibleDataChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Object, QStringLiteral("VisibleDataChanged"));
}

void MainWindow::selectionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, QStringLiteral("SelectionChanged"));
}

void MainWindow::modelChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Table, QStringLiteral("ModelChanged"));
}

void MainWindow::selectionChanged(const QModelIndex& current, const QModelIndex&)
{
    QAccessibleClient::AccessibleObject acc;
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
    QAccessibleClient::AccessibleObject acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    QMenu *menu = new QMenu(this);
    connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));
    for (const QSharedPointer<QAction> &a : acc.actions()) {
        menu->addAction(a.data());
    }
    menu->popup(m_accessibleObjectTreeView->mapToGlobal(pos));
}

void MainWindow::added(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, QStringLiteral("Add Object"));
    m_accessibleObjectTreeModel->addAccessible(object);
}

void MainWindow::removed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, QStringLiteral("RemoveObject"));
    m_accessibleObjectTreeModel->removeAccessible(object);
}

void MainWindow::defunct(const QAccessibleClient::AccessibleObject &object)
{
    removed(object);
}

void MainWindow::windowCreated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("Create"));
    m_accessibleObjectTreeModel->addAccessible(object);
}

void MainWindow::windowDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("Destroy"));
    if (!m_accessibleObjectTreeModel->removeAccessible(object)) {
        // assume the app has gone
        m_accessibleObjectTreeModel->updateTopLevelApps();
    }
}

void MainWindow::windowClosed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowClose"));
}

void MainWindow::windowReparented(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowReparent"));
}

void MainWindow::windowMinimized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMinimize"));
}

void MainWindow::windowMaximized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMaximize"));
}

void MainWindow::windowRestored(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowRestore"));
}

void MainWindow::windowActivated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowActivate"));
}

void MainWindow::windowDeactivated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDeactivate"));
}

void MainWindow::windowDesktopCreated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDesktopCreate"));
}

void MainWindow::windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowDesktopDestroy"));
}

void MainWindow::windowRaised(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowRaise"));
}

void MainWindow::windowLowered(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowLower"));
}

void MainWindow::windowMoved(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowMove"));
}

void MainWindow::windowResized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowResize"));
}

void MainWindow::windowShaded(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowShade"));
}

void MainWindow::windowUnshaded(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QStringLiteral("WindowUnshade"));
}

void MainWindow::focusChanged(const QAccessibleClient::AccessibleObject &object)
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

void MainWindow::MainWindow::textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, QStringLiteral("Text caret moved (%1)").arg(pos));
}

void MainWindow::MainWindow::textSelectionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, QStringLiteral("TextSelectionChanged"));
}

QString descriptionForText(const QString& type, const QString& text, int startOffset, int endOffset)
{
    QString shortText = text;
    if (shortText.length() > 50) {
        shortText.truncate(50);
        shortText.append(QLatin1String("..."));
    }
    QString desc = QStringLiteral("Text %1 (%2, %3): \"%4\"").arg(type).arg(QString::number(startOffset)).arg(QString::number(endOffset)).arg(shortText);
    return desc;
}

void MainWindow::textChanged(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("changed"), text, startOffset, endOffset));
}

void MainWindow::textInserted(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("inserted"), text, startOffset, endOffset));
}

void MainWindow::textRemoved(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::Text, descriptionForText(QLatin1String("removed"), text, startOffset, endOffset));
}

void MainWindow::accessibleNameChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::NameChanged);
    m_accessibleObjectTreeModel->updateAccessible(object);
}

void MainWindow::accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object)
{
    updateDetails(object);
    m_eventsWidget->addLog(object, EventsWidget::DescriptionChanged);
    m_accessibleObjectTreeModel->updateAccessible(object);
}

#include "moc_mainwindow.cpp"
