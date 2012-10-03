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
#include <qdebug.h>

using namespace QAccessibleClient;
QAccessible::UpdateHandler EventsWidget::m_originalAccessibilityUpdateHandler = 0;
QObject *EventsWidget::m_textEditForAccessibilityUpdateHandler = 0;

EventsWidget::EventsWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget(parent), m_registry(registry), m_selectedEvents(AllEvents) {
    m_ui.setupUi(this);

    m_ui.eventTextBrowser->setAccessibleName(QLatin1String("Events View"));
    m_ui.eventTextBrowser->setAccessibleDescription(QString("Displays all received events"));
    m_ui.eventTextBrowser->setOpenLinks(false);
    connect(m_ui.eventTextBrowser, SIGNAL(anchorClicked(QUrl)), this, SIGNAL(anchorClicked(QUrl)));
    connect(m_ui.clearButton, SIGNAL(clicked()), m_ui.eventTextBrowser, SLOT(clear()));
    connect(m_ui.eventSelectionTree->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(checkStateChanged()));

    m_textEditForAccessibilityUpdateHandler = m_ui.eventTextBrowser;
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

void EventsWidget::addLog(const QAccessibleClient::AccessibleObject &object, EventsWidget::EventTypes eventType, const QString &text)
{
    if (!object.isValid())
        return;
    if (object.name() == m_ui.eventTextBrowser->accessibleName() && object.description() == m_ui.eventTextBrowser->accessibleDescription())
        return;

    if (!(m_selectedEvents & eventType))
        return;

    QString eventName;
    switch (eventType) {
    case Focus:              eventName = QLatin1String("Focus event"); break;
    case StateChanged:       eventName = QLatin1String("State changed"); break;
    case NameChanged:        eventName = QLatin1String("Name changed"); break;
    case DescriptionChanged: eventName = QLatin1String("Description changed"); break;
    case Window:             eventName = QLatin1String("Window event"); break;
    case Document:           eventName = QLatin1String("Document event"); break;
    case Object:             eventName = QLatin1String("Object event"); break;
    case Text:               eventName = QLatin1String("Text event"); break;
    case Table:              eventName = QLatin1String("Table event"); break;
    }

    bool wasMax = m_ui.eventTextBrowser->verticalScrollBar()->value() == m_ui.eventTextBrowser->verticalScrollBar()->maximum();

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
}

void EventsWidget::checkStateChanged()
{
    m_selectedEvents = NoEvents;
    QAbstractItemModel *model = m_ui.eventSelectionTree->model();
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        bool checked = model->data(index, Qt::CheckStateRole).toBool();
        if (checked) {
            if (model->data(index) == "Window")
                m_selectedEvents = m_selectedEvents | Window;
            if (model->data(index) == "Name Changed")
                m_selectedEvents = m_selectedEvents | NameChanged;
            if (model->data(index) == "Description Changed")
                m_selectedEvents = m_selectedEvents | DescriptionChanged;
            if (model->data(index) == "State Changed")
                m_selectedEvents = m_selectedEvents | StateChanged;
            if (model->data(index) == "Focus")
                m_selectedEvents = m_selectedEvents | Focus;
            if (model->data(index) == "Text")
                m_selectedEvents = m_selectedEvents | Text;
            if (model->data(index) == "Document")
                m_selectedEvents = m_selectedEvents | Document;
            if (model->data(index) == "Object")
                m_selectedEvents = m_selectedEvents | Object;
            if (model->data(index) == "Others")
                m_selectedEvents = m_selectedEvents | Others;
        }
    }
}
