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

#include <qtextbrowser.h>

#include <qboxlayout.h>

#include "qaccessibilityclient/registry.h"
#include "qaccessibilityclient/accessibleobject.h"

class EventsWidget :public QWidget
{
    Q_OBJECT
public:
    explicit EventsWidget(QAccessibleClient::Registry *registry, QWidget *parent = 0)
        : QWidget(parent), m_registry(registry) {
        setLayout(new QVBoxLayout());
        layout()->setMargin(0);

        m_eventsEdit = new QTextBrowser(this);
        m_eventsEdit->setAccessibleName(QLatin1String("Events View"));
        m_eventsEdit->setAccessibleDescription(QString("Displays all received events"));
        m_eventsEdit->setOpenLinks(false);
        layout()->addWidget(m_eventsEdit);
        connect(m_eventsEdit, SIGNAL(anchorClicked(QUrl)), this, SIGNAL(anchorClicked(QUrl)));
    }
    void addLog(const QAccessibleClient::AccessibleObject &object, const QString &eventName, const QString &text = QString());

Q_SIGNALS:
    void anchorClicked(const QUrl &);
private:
    QAccessibleClient::Registry *m_registry;
    QTextBrowser *m_eventsEdit;
};
