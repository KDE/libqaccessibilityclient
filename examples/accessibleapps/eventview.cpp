/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "eventview.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QTextObject>
#include <QTimer>
#include <QScrollBar>
#include <QSettings>
#include <QPair>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMetaObject>
#include <QMetaEnum>
#include <QDebug>

class EventsModel : public QStandardItemModel
{
public:
    enum Role {
        AccessibleRole = 0,
        RoleRole = 1,
        EventRole = 2,
        ActionRole = 3,
        EventTypeRole = Qt::UserRole,
        UrlRole,
        AppNameRole,
        AppUrlRole
    };
    explicit EventsModel(EventsWidget *view) : QStandardItemModel(view), m_view(view) {
        clearLog();
    }
    ~EventsModel() override {}

    QHash<int,QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[AccessibleRole] = "accessible";
        roles[RoleRole] = "role";
        roles[EventRole] = "event";
        roles[EventTypeRole] = "eventType";
        roles[UrlRole] = "url";
        roles[AppNameRole] = "appName";
        roles[AppUrlRole] = "appUrl";
        return roles;
    }

    QString roleLabel(Role role) const
    {
        switch (role) {
            case AccessibleRole: return QStringLiteral("Accessible");
            case RoleRole: return QStringLiteral("Role");
            case EventRole: return QStringLiteral("Event");
            case ActionRole: return QStringLiteral("Action");
            case EventTypeRole:
            case UrlRole:
            case AppNameRole:
            case AppUrlRole:
                break;
        }
        return QString();
    }
    void clearLog()
    {
        clear();
        m_apps.clear();
        setColumnCount(4);
        QStringList headerLabels;
        for (Role r : QList<Role>() << AccessibleRole << RoleRole << EventRole << ActionRole)
            headerLabels << roleLabel(r);
        setHorizontalHeaderLabels(headerLabels);
    }
    struct LogItem {
        QStandardItem *appItem;
        bool isNewAppItem;
        LogItem(QStandardItem *appItem, bool isNewAppItem) : appItem(appItem), isNewAppItem(isNewAppItem) {}
    };
    LogItem addLog(QList<QStandardItem*> item)
    {
        QString appUrl = item.first()->data(AppUrlRole).toString();
        QStandardItem *appItem = nullptr;
        QMap<QString, QStandardItem*>::ConstIterator it = m_apps.constFind(appUrl);
        bool isNewAppItem = it == m_apps.constEnd();
        if (isNewAppItem) {
            QString appName = item.first()->data(AppNameRole).toString();
            m_apps[appUrl] = appItem = new QStandardItem(appName);
            appItem->setData(appUrl, EventsModel::AppUrlRole);
            invisibleRootItem()->appendRow(appItem);
        } else {
            appItem = it.value();
        }
        appItem->appendRow(item);
        return LogItem(appItem, isNewAppItem);
    }
private:
    EventsWidget *m_view;
    QMap<QString, QStandardItem*> m_apps;
};

class EventsProxyModel : public QSortFilterProxyModel
{
public:
    explicit EventsProxyModel(QWidget *parent = nullptr) : QSortFilterProxyModel(parent), m_types(EventsWidget::AllEvents) {}
    EventsWidget::EventTypes filter() const
    {
        return m_types;
    }
    QString accessibleFilter() const
    {
        return m_accessibleFilter;
    }
    QString roleFilter() const
    {
        return m_roleFilter;
    }
    void setFilter(EventsWidget::EventTypes types)
    {
        m_types = types;
        invalidateFilter();
    }
    void setAccessibleFilter(const QString &filter)
    {
        m_accessibleFilter = filter;
        invalidateFilter();
    }
    void setRoleFilter(const QString &filter)
    {
        m_roleFilter = filter;
        invalidateFilter();
    }
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        if (!source_parent.isValid())
            return true;
        if (!m_types.testFlag(EventsWidget::AllEvents)) {
            QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
            EventsWidget::EventType type = index.data(EventsModel::EventTypeRole).value<EventsWidget::EventType>();
            if (!m_types.testFlag(type))
                return false;
        }
        if (!m_accessibleFilter.isEmpty()) {
            QModelIndex index = sourceModel()->index(source_row, EventsModel::AccessibleRole, source_parent);
            QString accessibleName = index.data(Qt::DisplayRole).toString();
            if (!accessibleName.contains(m_accessibleFilter, Qt::CaseInsensitive))
                return false;
        }
        if (!m_roleFilter.isEmpty()) {
            QModelIndex index = sourceModel()->index(source_row, EventsModel::RoleRole, source_parent);
            QString roleName = index.data(Qt::DisplayRole).toString();
            if (!roleName.contains(m_roleFilter, Qt::CaseInsensitive))
                return false;
        }
        return true;
    }
private:
    EventsWidget::EventTypes m_types;
    QString m_accessibleFilter;
    QString m_roleFilter;
};

using namespace QAccessibleClient;
QAccessible::UpdateHandler EventsWidget::m_originalAccessibilityUpdateHandler = nullptr;
QObject *EventsWidget::m_textEditForAccessibilityUpdateHandler = nullptr;

EventsWidget::EventsWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget(parent), m_registry(registry), m_model(new EventsModel(this)), m_proxyModel(new EventsProxyModel(this)) {
    m_ui.setupUi(this);

    m_ui.eventListView->setAccessibleName(QLatin1String("Events View"));
    m_ui.eventListView->setAccessibleDescription(QStringLiteral("Displays all received events"));

    m_proxyModel->setSourceModel(m_model);
    m_ui.eventListView->setModel(m_proxyModel);

    connect(m_ui.accessibleFilterEdit, SIGNAL(textChanged(QString)), this, SLOT(accessibleFilterChanged()));
    connect(m_ui.roleFilterEdit, SIGNAL(textChanged(QString)), this, SLOT(roleFilterChanged()));

    QStandardItemModel *filerModel = new QStandardItemModel();
    QStandardItem *firstFilterItem = new QStandardItem(QStringLiteral("Event Filter"));
    firstFilterItem->setFlags(Qt::ItemIsEnabled);
    filerModel->appendRow(firstFilterItem);

    QVector< EventType > filterList;
    filterList << StateChanged << NameChanged << DescriptionChanged << Window << Focus << Document << Object << Text << Table << Others;
    for(int i = 0; i < filterList.count(); ++i) {
        EventType t = filterList[i];
        QStandardItem* item = new QStandardItem(eventName(t));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(QVariant::fromValue<EventType>(t), EventsModel::EventTypeRole);
        item->setData(Qt::Checked, Qt::CheckStateRole);
        filerModel->appendRow(QList<QStandardItem*>() << item);
    }
    m_ui.filterComboBox->setModel(filerModel);

    m_ui.clearButton->setFixedWidth(QFontMetrics(m_ui.clearButton->font()).boundingRect(m_ui.clearButton->text()).width() + 4);
    m_ui.clearButton->setFixedHeight(m_ui.filterComboBox->sizeHint().height());
    connect(m_ui.clearButton, SIGNAL(clicked()), this, SLOT(clearLog()));
    connect(m_ui.filterComboBox->model(), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(checkStateChanged()));
    connect(m_ui.eventListView, SIGNAL(activated(QModelIndex)), this, SLOT(eventActivated(QModelIndex)));

    // Collect multiple addLog calls and process them after 500 ms earliest. This
    // makes sure multiple calls to addLog will be compressed to one only one
    // view refresh what improves performance.
    m_pendingTimer.setInterval(500);
    connect(&m_pendingTimer, SIGNAL(timeout()), this, SLOT(processPending()));
    m_textEditForAccessibilityUpdateHandler = m_ui.eventListView;
    checkStateChanged();

    // We need to wait for a11y to be active for this hack.
    QTimer::singleShot(500, this, SLOT(installUpdateHandler()));
}

void EventsWidget::installUpdateHandler()
{
    m_originalAccessibilityUpdateHandler = QAccessible::installUpdateHandler(customUpdateHandler);
    if (!m_originalAccessibilityUpdateHandler)
        QTimer::singleShot(500, this, SLOT(installUpdateHandler()));
}

void EventsWidget::customUpdateHandler(QAccessibleEvent *event)
{
    QObject *object = event->object();
    if (object == m_textEditForAccessibilityUpdateHandler)
        return;
    //if (m_originalAccessibilityUpdateHandler)
    //    m_originalAccessibilityUpdateHandler(object, who, reason);
}

QString EventsWidget::eventName(EventType eventType) const
{
    QString s;
    switch (eventType) {
        case EventsWidget::Focus:              s = QLatin1String("Focus"); break;
        case EventsWidget::StateChanged:       s = QLatin1String("State"); break;
        case EventsWidget::NameChanged:        s = QLatin1String("Name"); break;
        case EventsWidget::DescriptionChanged: s = QLatin1String("Description"); break;
        case EventsWidget::Window:             s = QLatin1String("Window"); break;
        case EventsWidget::Document:           s = QLatin1String("Document"); break;
        case EventsWidget::Object:             s = QLatin1String("Object"); break;
        case EventsWidget::Text:               s = QLatin1String("Text"); break;
        case EventsWidget::Table:              s = QLatin1String("Table"); break;
        case EventsWidget::Others:             s = QLatin1String("Others"); break;
    }
    return s;
}

void EventsWidget::loadSettings(QSettings &settings)
{
    settings.beginGroup(QStringLiteral("events"));

    bool eventsFilterOk;
    EventTypes eventsFilter = EventTypes(settings.value(QStringLiteral("eventsFilter")).toInt(&eventsFilterOk));
    if (!eventsFilterOk)
        eventsFilter = AllEvents;

    QAbstractItemModel *model = m_ui.filterComboBox->model();
    if (eventsFilter != m_proxyModel->filter()) {
        for (int i = 1; i < model->rowCount(); ++i) {
            QModelIndex index = model->index(i, 0);
            EventType type = model->data(index, EventsModel::EventTypeRole).value<EventType>();
            if (eventsFilter.testFlag(type))
                model->setData(index, Qt::Checked, Qt::CheckStateRole);
            else
                model->setData(index, Qt::Unchecked, Qt::CheckStateRole);
        }
        m_proxyModel->setFilter(eventsFilter);
    }

    QByteArray eventListViewState = settings.value(QStringLiteral("listViewHeader")).toByteArray();
    if (!eventListViewState.isEmpty())
        m_ui.eventListView->header()->restoreState(eventListViewState);

    settings.endGroup();
}

void EventsWidget::saveSettings(QSettings &settings)
{
    settings.beginGroup(QStringLiteral("events"));
    settings.setValue(QStringLiteral("eventsFilter"), int(m_proxyModel->filter()));

    QByteArray eventListViewState = m_ui.eventListView->header()->saveState();
    settings.setValue(QStringLiteral("listViewHeader"), eventListViewState);

    settings.endGroup();
}

void EventsWidget::clearLog()
{
    m_model->clearLog();
}

void EventsWidget::processPending()
{
    m_pendingTimer.stop();
    QVector< QList<QStandardItem*> > pendingLogs = m_pendingLogs;
    m_pendingLogs.clear();
    //bool wasMax = true;//m_ui.eventListView->verticalScrollBar()->value() - 10 >= m_ui.eventListView->verticalScrollBar()->maximum();
    QStandardItem *lastItem = nullptr;
    QStandardItem *lastAppItem = nullptr;
    for(int i = 0; i < pendingLogs.count(); ++i) {
        QList<QStandardItem*> item = pendingLogs[i];
        EventsModel::LogItem logItem = m_model->addLog(item);

        // Logic to scroll to the last added logItem of the last appItem that is expanded.
        // For appItem's not expanded the logItem is added but no scrolling will happen.
        if (lastItem && lastAppItem && lastAppItem != logItem.appItem)
            lastItem = nullptr;
        bool selected = lastItem;
        if (lastAppItem != logItem.appItem) {
            lastAppItem = logItem.appItem;
            QModelIndex index = m_proxyModel->mapFromSource(m_model->indexFromItem(logItem.appItem));
            if (logItem.isNewAppItem) {
                m_ui.eventListView->setExpanded(index, true);
                selected = true;
            } else {
                selected = m_ui.eventListView->isExpanded(index);
            }
        }
        if (selected)
            lastItem = item.first();
    }
    if (lastItem) { // scroll down to the lastItem.
        //m_ui.eventListView->verticalScrollBar()->setValue(m_ui.eventListView->verticalScrollBar()->maximum());

        QModelIndex index = m_proxyModel->mapFromSource(m_model->indexFromItem(lastItem));
        m_ui.eventListView->scrollTo(index, QAbstractItemView::PositionAtBottom);
        //m_ui.eventListView->scrollTo(index, QAbstractItemView::EnsureVisible);
    }
}

void EventsWidget::addLog(const QAccessibleClient::AccessibleObject &object, EventsWidget::EventType eventType, const QString &text)
{
    if (!object.isValid())
        return;

    QStandardItem *nameItem = new QStandardItem(object.name());
    nameItem->setData(QVariant::fromValue<EventType>(eventType), EventsModel::EventTypeRole);
    nameItem->setData(object.url().toString(), EventsModel::UrlRole);

    AccessibleObject app = object.application();
    if (app.isValid()) {
        nameItem->setData(app.name(), EventsModel::AppNameRole);
        nameItem->setData(app.url().toString(), EventsModel::AppUrlRole);
    }

    QStandardItem *roleItem = new QStandardItem(object.roleName());
    QStandardItem *typeItem = new QStandardItem(eventName(eventType));
    QStandardItem *textItem = new QStandardItem(text);
    m_pendingLogs.append(QList<QStandardItem*>() << nameItem << roleItem << typeItem << textItem);
    if (!m_pendingTimer.isActive()) {
        m_pendingTimer.start();
    }
}

void EventsWidget::checkStateChanged()
{
    EventTypes types;
    QStringList names;
    bool allEvents = true;
    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("EventType"));
    Q_ASSERT(e.isValid());
    QAbstractItemModel *model = m_ui.filterComboBox->model();
    for (int i = 1; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        bool checked = model->data(index, Qt::CheckStateRole).toBool();
        if (checked) {
            EventType type = model->data(index, EventsModel::EventTypeRole).value<EventType>();
            types |= type;
            names.append(QString::fromLatin1(e.valueToKey(type)));
        } else {
            allEvents = false;
        }
    }
    m_proxyModel->setFilter(types);
}

void EventsWidget::eventActivated(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    QModelIndex parent = index.parent();
    QModelIndex firstIndex = m_proxyModel->index(index.row(), 0, parent);
    QString s = m_proxyModel->data(firstIndex, parent.isValid() ? EventsModel::UrlRole : EventsModel::AppUrlRole).toString();
    QUrl url(s);
    if (!url.isValid()) {
        qWarning() << Q_FUNC_INFO << "Invalid url=" << s;
        return;
    }
    Q_EMIT anchorClicked(url);
}

void EventsWidget::accessibleFilterChanged()
{
    m_proxyModel->setAccessibleFilter(m_ui.accessibleFilterEdit->text());
}

void EventsWidget::roleFilterChanged()
{
    m_proxyModel->setRoleFilter(m_ui.roleFilterEdit->text());
}

#include "moc_eventview.cpp"
