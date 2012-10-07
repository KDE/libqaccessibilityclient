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

#include "eventview.h"

#include <qtextdocument.h>
#include <qtextcursor.h>
#include <qtextobject.h>
#include <qtimer.h>
#include <qscrollbar.h>
#include <QPair>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMetaObject>
#include <QMetaEnum>
#include <qdebug.h>

class EventsModel : public QStandardItemModel
{
public:
    EventsModel(EventsWidget *view) : QStandardItemModel(view), m_view(view) {
        setColumnCount(2);
        setHorizontalHeaderLabels( QStringList() << QString("Type") << QString("Text") );
    }
    ~EventsModel() {}

    void addLog(QList<QStandardItem*> item)
    {
        QStandardItem *parentItem = invisibleRootItem();
        parentItem->appendRow(item);
    }
private:
    EventsWidget *m_view;
};

class EventsProxyModel : public QSortFilterProxyModel
{
public:
    EventsProxyModel(QWidget *parent = 0) : QSortFilterProxyModel(parent), m_types(EventsWidget::AllEvents) {}
    void setFiler(EventsWidget::EventTypes types) {
        m_types = types;
        invalidateFilter();
    }
protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
        EventsWidget::EventType type = index.data(Qt::UserRole).value<EventsWidget::EventType>();
        return m_types.testFlag(type);
    }
private:
    EventsWidget::EventTypes m_types;
};

using namespace QAccessibleClient;
QAccessible::UpdateHandler EventsWidget::m_originalAccessibilityUpdateHandler = 0;
QObject *EventsWidget::m_textEditForAccessibilityUpdateHandler = 0;

EventsWidget::EventsWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget(parent), m_registry(registry), m_model(new EventsModel(this))/*, m_selectedEvents(AllEvents)*/ {
    m_ui.setupUi(this);

    m_ui.eventListView->setAccessibleName(QLatin1String("Events View"));
    m_ui.eventListView->setAccessibleDescription(QString("Displays all received events"));

    EventsProxyModel *proxyModel = new EventsProxyModel();
    proxyModel->setSourceModel(m_model);
    m_ui.eventListView->setModel(proxyModel);

    QStandardItemModel *filerModel = new QStandardItemModel();
    filerModel->setColumnCount(2);
    filerModel->appendRow(new QStandardItem(QString("Event Filter")));

    QVector< QPair<EventType, QString> > filterList;
    filterList << QPair<EventType, QString>(StateChanged, "State");
    filterList << QPair<EventType, QString>(NameChanged, "Name");
    filterList << QPair<EventType, QString>(DescriptionChanged, "Description");
    filterList << QPair<EventType, QString>(Window, "Window");
    filterList << QPair<EventType, QString>(Focus, "Focus");
    filterList << QPair<EventType, QString>(Document, "Document");
    filterList << QPair<EventType, QString>(Object, "Object");
    filterList << QPair<EventType, QString>(Text, "Text");
    filterList << QPair<EventType, QString>(Table, "Table");
    filterList << QPair<EventType, QString>(Others, "Others");
    for(int i = 0; i < filterList.count(); ++i) {
        QPair<EventType, QString> p = filterList[i];
        QStandardItem* item = new QStandardItem(p.second);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(QVariant::fromValue<EventType>(p.first), Qt::UserRole);
        //item->setData(Qt::Unchecked, Qt::CheckStateRole);
        item->setData(Qt::Checked, Qt::CheckStateRole);
        filerModel->appendRow(QList<QStandardItem*>() << item << new QStandardItem());
    }
    m_ui.filterComboBox->setModel(filerModel);

//m_ui.eventListView->setOpenLinks(false);
//connect(m_ui.eventTextBrowser, SIGNAL(anchorClicked(QUrl)), this, SIGNAL(anchorClicked(QUrl)));
    m_ui.clearButton->setFixedHeight(m_ui.filterComboBox->sizeHint().height());
    connect(m_ui.clearButton, SIGNAL(clicked()), this, SLOT(clearLog()));
    connect(m_ui.filterComboBox->model(), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(checkStateChanged()));

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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void EventsWidget::customUpdateHandler(QAccessibleEvent *event)
{
    QObject *object = event->object();
#else
void EventsWidget::customUpdateHandler(QObject *object, int who, QAccessible::Event reason)
{
#endif
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
    }
    return s;
}

void EventsWidget::clearLog()
{
    m_model->clear();
}

void EventsWidget::processPending()
{
    m_pendingTimer.stop();
    QVector< QList<QStandardItem*> > pendingLogs = m_pendingLogs;
    m_pendingLogs.clear();
    for(int i = 0; i < pendingLogs.count(); ++i) {
        m_model->addLog(pendingLogs[i]);
    }
}

void EventsWidget::addLog(const QAccessibleClient::AccessibleObject &object, EventsWidget::EventType eventType, const QString &text)
{
    if (!object.isValid())
        return;

//     if (object.name() == m_ui.eventListView->accessibleName() && object.description() == m_ui.eventListView->accessibleDescription())
//         return;

    QStandardItem *nameItem = new QStandardItem(eventName(eventType));
    nameItem->setData(QVariant::fromValue<EventType>(eventType), Qt::UserRole);
    QStandardItem *textItem = new QStandardItem(text);
    m_pendingLogs.append(QList<QStandardItem*>() << nameItem << textItem);
    if (!m_pendingTimer.isActive()) {
        m_pendingTimer.start();
    }

#if 0
    bool wasMax = m_ui.eventListView->verticalScrollBar()->value() == m_ui.eventListView->verticalScrollBar()->maximum();

    QTextDocument *doc = m_ui.eventTextBrowser->document();
    doc->blockSignals(true); // to prevent infinte TextCaretMoved events
    QTextCursor cursor(doc->lastBlock());
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    cursor.insertText(eventName.trimmed() + QLatin1String(": "));
    if (!text.isEmpty())
        cursor.insertText(text + QLatin1Char(' '));
    QString url = m_registry->url(object).toString();
    QString objectString = object.name() + QString(" [%1]").arg(object.roleName());
    cursor.insertHtml(QString("<a href=\"%1\">%2</a> ").arg(url).arg(objectString));

    AccessibleObject app = object.application();
    if (app.isValid()) {
        cursor.insertText(" (" + app.name() + ")");
    } else {
        qDebug() << "Invalid parent: " << object;
        cursor.insertText(" (invalid application)");
    }

    cursor.insertBlock();

    doc->blockSignals(false);

    if (wasMax) // scroll down if we where before scrolled down too
        m_ui.eventTextBrowser->verticalScrollBar()->setValue(m_ui.eventTextBrowser->verticalScrollBar()->maximum());
#endif
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
            EventType type = model->data(index, Qt::UserRole).value<EventType>();
            types |= type;
            names.append(QString::fromLatin1(e.valueToKey(type)));
        } else {
            allEvents = false;
        }
    }

    EventsProxyModel *proxyModel = dynamic_cast<EventsProxyModel*>(m_ui.eventListView->model());
    proxyModel->setFiler(types);
}
