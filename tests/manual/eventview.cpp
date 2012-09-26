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
#include <QTextBlock>
#include <qscrollbar.h>

using namespace QAccessibleClient;

EventsWidget::EventsWidget(QAccessibleClient::Registry *registry, QWidget *parent)
    : QWidget(parent), m_registry(registry), m_selectedEvents(AllEvents) {
    m_ui.setupUi(this);

    m_ui.eventTextBrowser->setAccessibleName(QLatin1String("Events View"));
    m_ui.eventTextBrowser->setAccessibleDescription(QString("Displays all received events"));
    m_ui.eventTextBrowser->setOpenLinks(false);
    connect(m_ui.eventTextBrowser, SIGNAL(anchorClicked(QUrl)), this, SIGNAL(anchorClicked(QUrl)));
    connect(m_ui.clearButton, SIGNAL(clicked()), m_ui.eventTextBrowser, SLOT(clear()));
    connect(m_ui.eventSelectionTree->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(checkStateChanged()));
}

void EventsWidget::addLog(const QAccessibleClient::AccessibleObject &object, const QString &eventName, const QString &text)
{
    if (!object.isValid())
        return;
    if (object.name() == m_ui.eventTextBrowser->accessibleName() && object.description() == m_ui.eventTextBrowser->accessibleDescription())
        return;

    bool found = false;

    if (eventName == QLatin1String("StateChanged")) {
        found = true;
        if (text.contains("focus")) {
            found = true;
            if (!(m_selectedEvents & Focus))
                return;
        } else {
            if (!(m_selectedEvents & StateChanged))
                return;
        }
    }
    if (eventName == QLatin1String("AccessibleNameChanged")) {
        found = true;
        if (!(m_selectedEvents & NameChanged))
            return;
    }
    if (eventName == QLatin1String("AccessibleDescriptionChanged")) {
        found = true;
        if (!(m_selectedEvents & DescriptionChanged))
            return;
    }
    if (eventName.startsWith("Window")) {
        found = true;
        if (!(m_selectedEvents & Window))
            return;
    }
    if (eventName.startsWith("Document")) {
        found = true;
        if (!(m_selectedEvents & Document))
            return;
    }
    if (eventName.startsWith("Object")) {
        found = true;
        if (!(m_selectedEvents & Object))
            return;
    }
    if (eventName.startsWith("Text")) {
        found = true;
        if (!(m_selectedEvents & Text))
            return;
    }
    if (!(m_selectedEvents & Others) && !found)
        return;

    bool wasMax = m_ui.eventTextBrowser->verticalScrollBar()->value() == m_ui.eventTextBrowser->verticalScrollBar()->maximum();

    QTextDocument *doc = m_ui.eventTextBrowser->document();
    doc->blockSignals(true); // to prevent infinte TextCaretMoved events
    QTextCursor cursor(doc->lastBlock());
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    QString s = QString("%1: %2").arg(eventName).arg(object.name());
    QUrl url = m_registry->url(object);
    cursor.insertText(s.trimmed() + QLatin1Char(' '));
    cursor.insertHtml(QString("(<a href=\"%1\">%2</a>) ").arg(url.toString()).arg(object.roleName()));
    if (!text.isEmpty())
        cursor.insertText(QLatin1Char(' ') + text);

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
