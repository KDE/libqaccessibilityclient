/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QTimer>
#include <QBoxLayout>
#include <QAccessible>

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
    Q_ENUM(EventType)
    Q_ENUM(EventTypes)

    QString eventName(EventType eventType) const;

    explicit EventsWidget(QAccessibleClient::Registry *registry, QWidget *parent = nullptr);

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
    static void customUpdateHandler(QAccessibleEvent *event);
    static QAccessible::UpdateHandler m_originalAccessibilityUpdateHandler;
    static QObject *m_textEditForAccessibilityUpdateHandler;
};

Q_DECLARE_METATYPE(EventsWidget::EventType)
Q_DECLARE_METATYPE(EventsWidget::EventTypes)

#endif
