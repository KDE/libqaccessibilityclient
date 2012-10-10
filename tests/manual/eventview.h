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

#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QTimer>
#include <qboxlayout.h>
#include <qaccessible.h>

#include "qaccessibilityclient/registry.h"
#include "qaccessibilityclient/accessibleobject.h"

#include "ui_eventview.h"

class QStandardItem;
class QSettings;

class EventsModel;
class EventsProxyModel;

class EventsWidget :public QWidget
{
    Q_OBJECT
    Q_ENUMS(EventType)
    Q_ENUMS(EventTypes)
public:
    enum EventType {
        NoEvents = 0x00,
        StateChanged = 0x01,
        NameChanged = 0x02,
        DescriptionChanged = 0x04,
        Window = 0x08,
        Focus = 0x10,
        Document = 0x20,
        Object = 0x40,
        Text = 0x80,
        Table = 0x100,



        Others = 0x100000,

        AllEvents = 0xffff
    };
    Q_DECLARE_FLAGS(EventTypes, EventType)

    QString eventName(EventType eventType) const;

    explicit EventsWidget(QAccessibleClient::Registry *registry, QWidget *parent = 0);

    void loadSettings(QSettings &settings);
    void saveSettings(QSettings &settings);

    void addLog(const QAccessibleClient::AccessibleObject &object, EventType eventType, const QString &text = QString());

Q_SIGNALS:
    void anchorClicked(const QUrl &);

public Q_SLOTS:
    void checkStateChanged();
private Q_SLOTS:
    void installUpdateHandler();
    void clearLog();
    void processPending();
    void eventActivated(const QModelIndex &index);
    void accessibleFilterChanged();
    void roleFilterChanged();
private:
    QAccessibleClient::Registry *m_registry;
    Ui::EventViewWidget m_ui;
    EventsModel *m_model;
    EventsProxyModel *m_proxyModel;
    QTimer m_pendingTimer;
    QVector< QList<QStandardItem*> > m_pendingLogs;

    // This is to avoid sending updates for the events view.
    // The reason is that we end up in endless loops with other accessible tools such as accerciser.
    // No normal application should have to do this.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    static void customUpdateHandler(QAccessibleEvent *event);
#else
    static void customUpdateHandler(QObject*object, int who, QAccessible::Event reason);
#endif
    static QAccessible::UpdateHandler m_originalAccessibilityUpdateHandler;
    static QObject *m_textEditForAccessibilityUpdateHandler;
};

Q_DECLARE_METATYPE(EventsWidget::EventType)
Q_DECLARE_METATYPE(EventsWidget::EventTypes)

#endif
