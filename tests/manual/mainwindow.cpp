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
#include "accessibleproperties.h"
#include "eventview.h"
#include "uiview.h"
#include "qaccessibilityclient/registry.h"
#include "qaccessibilityclient/accessibleobject.h"
#include "accessibletree.h"

#include <qitemselectionmodel.h>
#include <qstring.h>
#include <qtreeview.h>
#include <qheaderview.h>
#include <qdockwidget.h>
#include <qmenubar.h>
#include <qsettings.h>
#include <qurl.h>
#include <qpointer.h>
#include <qdialog.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListView>
#include <QStandardItemModel>
#include <QComboBox>

using namespace QAccessibleClient;

ClientCacheDialog::ClientCacheDialog(QAccessibleClient::Registry *registry, QWidget *parent)
    : QDialog(parent)
    , m_registry(registry)
{
    setModal(true);
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);

    m_view = new QTreeView(this);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setRootIsDecorated(false);
    m_view->setSortingEnabled(true);
    m_view->setItemsExpandable(false);
    //list->setHeaderHidden(true);
    m_model = new QStandardItemModel(m_view);
    m_model->setColumnCount(3);
    m_view->setModel(m_model);
    lay->addWidget(m_view);

    QHBoxLayout *buttonsLay = new QHBoxLayout(this);
    buttonsLay->setMargin(0);
    buttonsLay->setSpacing(lay->margin());
    QPushButton *updateButton = new QPushButton(QString("Refresh"), this);
    buttonsLay->addWidget(updateButton);
    connect(updateButton, SIGNAL(clicked(bool)), this, SLOT(updateView()));
    QPushButton *clearButton = new QPushButton(QString("Clear"), this);
    buttonsLay->addWidget(clearButton);

    QLabel *cacheLabel = new QLabel(QString("Strategy:"), this);
    buttonsLay->addWidget(cacheLabel);
    m_cacheCombo = new QComboBox(this);
    cacheLabel->setBuddy(m_cacheCombo);
    m_cacheCombo->setEditable(false);
    m_cacheCombo->addItem(QString("Disable"), int(QAccessibleClient::Registry::NoCache));
    m_cacheCombo->addItem(QString("Weak"), int(QAccessibleClient::Registry::WeakCache));
    m_cacheCombo->addItem(QString("Strong"), int(QAccessibleClient::Registry::StrongCache));
    for(int i = 0; i < m_cacheCombo->count(); ++i) {
        if (m_cacheCombo->itemData(i).toInt() == m_registry->cacheType()) {
            m_cacheCombo->setCurrentIndex(i);
            break;
        }
    }
    connect(m_cacheCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cacheStrategyChanged()));
    buttonsLay->addWidget(m_cacheCombo);
    buttonsLay->addWidget(new QLabel(QString("Count:"), this));
    m_countLabel = new QLabel(this);
    buttonsLay->addWidget(m_countLabel);
    buttonsLay->addStretch(1);

    connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearCache()));
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    buttonsLay->addWidget(buttons);
    QPushButton *closeButton = buttons->button(QDialogButtonBox::Close);
    connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    lay->addLayout(buttonsLay);

    resize(minimumSize().expandedTo(QSize(660,420)));

    updateView();
    m_view->sortByColumn(2, Qt::AscendingOrder);
}

void ClientCacheDialog::clearCache()
{
    m_registry->clearClientCache();
    updateView();
}

void ClientCacheDialog::cacheStrategyChanged()
{
    int c = m_cacheCombo->itemData(m_cacheCombo->currentIndex()).toInt();
    m_registry->setCacheType(Registry::CacheType(c));
    updateView();
}

void ClientCacheDialog::updateView()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels( QStringList() << QString("Name") << QString("Role") << QString("Identifier") );
    QStringList cache = m_registry->clientCacheObjects();
    m_countLabel->setText(QString::number(cache.count()));
    Q_FOREACH(const QString &c, cache) {
        AccessibleObject obj = m_registry->clientCacheObject(c);
        if (obj.isValid())
            m_model->appendRow( QList<QStandardItem*>()
                << new QStandardItem(obj.name())
                << new QStandardItem(obj.roleName())
                << new QStandardItem(obj.id()) );
    }
    m_view->setColumnWidth(0, 180);
    m_view->resizeColumnToContents(1);
    m_view->resizeColumnToContents(2);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QLatin1String("Randamizer"));
    m_registry = new QAccessibleClient::Registry(this);

    initUi();
    initActions();
    initMenu();

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
    connect(m_registry, SIGNAL(textChanged(QAccessibleClient::AccessibleObject)), this, SLOT(textChanged(QAccessibleClient::AccessibleObject)));

    connect(m_registry, SIGNAL(accessibleNameChanged(QAccessibleClient::AccessibleObject)), this, SLOT(accessibleNameChanged(QAccessibleClient::AccessibleObject)));
    connect(m_registry, SIGNAL(accessibleDescriptionChanged(QAccessibleClient::AccessibleObject)), this, SLOT(accessibleDescriptionChanged(QAccessibleClient::AccessibleObject)));

    QSettings settings("kde.org", "kdea11yapp");
    m_registry->setCacheType(Registry::CacheType(settings.value("cacheStrategy", m_registry->cacheType()).toInt()));
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    m_registry->subscribeEventListeners(QAccessibleClient::Registry::AllEventListeners);
}

MainWindow::~MainWindow()
{
    delete m_registry;
}

void MainWindow::MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("kde.org", "kdea11yapp");
    settings.setValue("cacheStrategy", int(m_registry->cacheType()));
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
}

void MainWindow::MainWindow::initUi()
{
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
    //setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    QDockWidget *treeDocker = new QDockWidget(QString("Tree"), this);
    treeDocker->setObjectName("tree");
    treeDocker->setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_treeView = new QTreeView(treeDocker);
    m_treeView->setAccessibleName(QString("Tree of accessibles"));
    m_treeView->setAccessibleDescription(QString("Displays a hierachical tree of accessible objects"));
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeDocker->setWidget(m_treeView);

    m_treeModel = new AccessibleTree(this);
    m_treeModel->setRegistry(m_registry);
    m_treeView->setModel(m_treeModel);
    m_treeView->setColumnWidth(0, 240);
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

    QDockWidget *uiDocker = new QDockWidget(QString("Boundaries"), this);
    uiDocker->setObjectName("Boundaries");
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
    QAccessibleClient::AccessibleObject object = m_registry->fromUrl(url);
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
    QModelIndex index = m_treeModel->indexForAccessible(object);
    if (index.isValid()) {
        QModelIndex other = m_treeModel->index(index.row(), index.column()+1, index.parent());
        Q_ASSERT(other.isValid());
        m_treeView->selectionModel()->select(QItemSelection(index, other), QItemSelectionModel::SelectCurrent);
        m_treeView->scrollTo(index);

        // Unlike calling setCurrentIndex the select call aboves doe not emit the selectionChanged signal. So, do explicit.
        selectionChanged(index, QModelIndex());
    } else {
        qWarning() << "No such indexForAccessible=" << object;
    }
}

void MainWindow::stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active)
{
    if (state == QLatin1String("focus")) {
        m_eventsWidget->addLog(object, EventsWidget::Focus, (active ? QString("true") : QString("false")));
    } else {
        QString s = state + QString(": ") + (active ? QString("true") : QString("false"));
        m_eventsWidget->addLog(object, EventsWidget::StateChanged, s);
    }
}

void MainWindow::childAdded(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildAdded ") + QString::number(childIndex));
}

void MainWindow::childRemoved(const QAccessibleClient::AccessibleObject &object, int childIndex)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, QLatin1String("ChildRemoved ") + QString::number(childIndex));
}

void MainWindow::visibleDataChanged(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, "VisibleDataChanged");
}

void MainWindow::selectionChanged(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Table, "SelectionChanged");
}

void MainWindow::modelChanged(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Table, "ModelChanged");
}

void MainWindow::MainWindow::selectionChanged(const QModelIndex& current, const QModelIndex&)
{
    QAccessibleClient::AccessibleObject acc;
    if (current.isValid() && current.internalPointer()) {
        acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    }
    m_propertyModel->setAccessibleObject(acc);
    m_uiview->setAccessibleObject(acc);
    m_propertyView->expandAll();
    m_propertyView->resizeColumnToContents(0);
}

void MainWindow::MainWindow::treeCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex current = m_treeView->currentIndex();
    if (!current.isValid())
        return;
    QAccessibleClient::AccessibleObject acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
    QMenu *menu = new QMenu(this);
    connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));
    Q_FOREACH(const QSharedPointer<QAction> &a, acc.actions()) {
        menu->addAction(a.data());
    }
    menu->popup(m_treeView->mapToGlobal(pos));
}

void MainWindow::added(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, QString("Add Object"));
    m_treeModel->addAccessible(object);
}

void MainWindow::removed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Object, "RemoveObject");
    m_treeModel->removeAccessible(object);
}

void MainWindow::defunct(const QAccessibleClient::AccessibleObject &object)
{
    removed(object);
}

void MainWindow::windowCreated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, "Create");
    m_treeModel->addAccessible(object);
}

void MainWindow::windowDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("Destroy"));
    m_treeModel->removeAccessible(object);
}

void MainWindow::windowClosed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowClose"));
}

void MainWindow::windowReparented(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowReparent"));
}

void MainWindow::windowMinimized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowMinimize"));
}

void MainWindow::windowMaximized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowMaximize"));
}

void MainWindow::windowRestored(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowRestore"));
}

void MainWindow::windowActivated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowActivate"));
}

void MainWindow::windowDeactivated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowDeactivate"));
}

void MainWindow::windowDesktopCreated(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowDesktopCreate"));
}

void MainWindow::windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowDesktopDestroy"));
}

void MainWindow::windowRaised(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowRaise"));
}

void MainWindow::windowLowered(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowLower"));
}

void MainWindow::windowMoved(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowMove"));
}

void MainWindow::windowResized(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowResize"));
}

void MainWindow::windowShaded(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowShade"));
}

void MainWindow::windowUnshaded(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Window, QString("WindowUnshade"));
}

void MainWindow::focusChanged(const QAccessibleClient::AccessibleObject &object)
{
    if (m_followFocusAction->isChecked()) {
        // We need to block the focus for the treeView while setting the current item
        // to prevent that setting that item would change focus to the treeView.
        Qt::FocusPolicy prevFocusPolicy = m_treeView->focusPolicy();
        m_treeView->setFocusPolicy(Qt::NoFocus);

        setCurrentObject(object);

        m_treeView->setFocusPolicy(prevFocusPolicy);
    }
}

void MainWindow::MainWindow::textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos)
{
    m_eventsWidget->addLog(object, EventsWidget::Text, QString("TextCaretMoved"));
}

void MainWindow::MainWindow::textSelectionChanged(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Text, QString("TextSelectionChanged"));
}

void MainWindow::textChanged(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::Text, QString("TextChanged"));
}

void MainWindow::accessibleNameChanged(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::NameChanged);
    m_treeModel->updateAccessible(object);
}

void MainWindow::accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object)
{
    m_eventsWidget->addLog(object, EventsWidget::DescriptionChanged);
    m_treeModel->updateAccessible(object);
}

