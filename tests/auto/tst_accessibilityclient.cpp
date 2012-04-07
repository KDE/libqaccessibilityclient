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

#define QT_GUI_LIB
#include <qtest.h>

#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qboxlayout.h>
#include <qdebug.h>

#include "accessible/registry.h"
#include "accessible/accessibleobject.h"

#include "atspi/dbusconnection.h"

using namespace KAccessibleClient;

struct Event {
    Event(const AccessibleObject &object)
        : m_object(object)
    {}

    AccessibleObject m_object;
};

class EventListener : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void focus(const KAccessibleClient::AccessibleObject &object) {
        events.append(Event(object));
    }

public:
    QList<Event> events;
};

class AccessibilityClientTest :public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void tst_navigation();
    void tst_focus();
};

void AccessibilityClientTest::tst_navigation()
{
    QPushButton button;
    button.setText(QLatin1String("Hello a11y"));
    button.show();

    QTest::qWaitForWindowShown(&button);

    Registry r;

    QString appname = QLatin1String("Lib KAccessibleClient test");
    qApp->setApplicationName(appname);
    AccessibleObject app = r.applications().last();
    QVERIFY(app.isValid());
    QCOMPARE(app.name(), appname);

    QCOMPARE(app.childCount(), 1);
    AccessibleObject child1 = app.child(0);
    QVERIFY(child1.isValid());
    QCOMPARE(child1.name(), button.text());
    AccessibleObject child2 = app.children().first();
    QCOMPARE(child1, child2);
    AccessibleObject parent = child1.parent();
    QCOMPARE(parent, app);

    AccessibleObject invalidChild = child1.child(0);
    QVERIFY(!invalidChild.isValid());
    QVERIFY(invalidChild.name().isEmpty());

    AccessibleObject invalidParent = app.parent();
    QVERIFY(!invalidParent.isValid());
    QVERIFY(invalidParent.name().isEmpty());
}

void AccessibilityClientTest::tst_focus()
{
    QMainWindow window;
    QWidget *w = new QWidget();
    window.setCentralWidget(w);
    QVBoxLayout *l = new QVBoxLayout();
    w->setLayout(l);
    QPushButton *button = new QPushButton();
    QPushButton *button2 = new QPushButton();
    l->addWidget(button);
    l->addWidget(button2);

    button->setText(QLatin1String("Button 1"));
    button2->setText(QLatin1String("Button 2"));
    window.show();
    button->setFocus();

    QTest::qWaitForWindowShown(&window);

    Registry r;
    r.subscribeEventListeners(Registry::Focus);
    EventListener listener;
    connect(&r, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)), &listener, SLOT(focus(KAccessibleClient::AccessibleObject)));

    button2->setFocus();

    QTest::qWait(100);

    QCOMPARE(listener.events.size(), 1);
}


QTEST_MAIN(AccessibilityClientTest)

#include "tst_accessibilityclient.moc"
