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
#include <qlabel.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qaccessible.h>
#include <qdebug.h>
#include <qprocess.h>

#include "kdeaccessibilityclient/registry.h"
#include "kdeaccessibilityclient/accessibleobject.h"

#include "atspi/dbusconnection.h"
#include "atspi/atspi-constants.h"

typedef QSharedPointer<QAccessibleInterface> QAIPointer;

using namespace KAccessibleClient;

struct Event {
    Event(const AccessibleObject &obj)
        : object(obj)
    {}

    AccessibleObject object;
};

class EventListener : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void focus(const KAccessibleClient::AccessibleObject &object) {
        focusEvents.append(Event(object));
    }

public:
    QList<Event> focusEvents;
};

class AccessibilityClientTest :public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void tst_application();
    void tst_navigation();
    void tst_focus();

private:
    Registry registry;
};

void AccessibilityClientTest::initTestCase()
{
    qDebug() << "Starting test.";
    if (qgetenv("QT_ACCESSIBILITY") != QByteArray("1"))
        qWarning() << "QT_ACCESSIBILITY=1 not found, this leads to failing tests with Qt 4";
}

AccessibleObject getAppObject(const Registry &r, const QString &appName)
{
    AccessibleObject accApp;

    QApplication::processEvents();
    QList<AccessibleObject> apps = r.applications();
    foreach (const AccessibleObject &app, apps) {
        if (app.name() == appName) {
            accApp = app;
            break;
        }
    }
    return accApp;
}

void AccessibilityClientTest::tst_application()
{
    QString appName = QLatin1String("Lib KAccessibleClient test");
    qApp->setApplicationName(appName);
    QWidget w;
    w.setAccessibleName("Foobar 99");
    w.show();

    AccessibleObject accApp;
    QVERIFY(!accApp.isValid());
    accApp = getAppObject(registry, appName);
    QVERIFY(accApp.isValid());
    QCOMPARE(accApp.name(), appName);
    QCOMPARE(accApp.childCount(), 1);
}

void AccessibilityClientTest::tst_navigation()
{
    QString appName = QLatin1String("Lib KAccessibleClient test");
    qApp->setApplicationName(appName);
    QWidget w;
    w.setAccessibleName("Root Widget");
    w.setAccessibleDescription("This is a useless widget");
    QVBoxLayout *layout = new QVBoxLayout;
    w.setLayout(layout);

    QPushButton *button = new QPushButton;
    layout->addWidget(button);
    button->setText(QLatin1String("Hello a11y"));
    QString desc = "This is a button...";
    button->setAccessibleDescription(desc);
    w.show();
    button->setFocus();

    QTest::qWaitForWindowShown(&w);

    // App
    AccessibleObject accApp = getAppObject(registry, appName);
    QVERIFY(accApp.isValid());
    QCOMPARE(accApp.name(), appName);
    QCOMPARE(accApp.childCount(), 1);

    // What should this return?
    QCOMPARE(accApp.indexInParent(), -1);

    // Root widget
    AccessibleObject accW = accApp.child(0);
    QVERIFY(accW.isValid());
    qDebug() << "NAME: " << accW.name();
    QCOMPARE(accW.name(), w.accessibleName());
    QCOMPARE(accW.description(), w.accessibleDescription());
    QCOMPARE(accW.role(), ATSPI_ROLE_FILLER);
    QCOMPARE(accW.roleName(), QLatin1String("filler"));
    QCOMPARE(accW.childCount(), 1);
    QCOMPARE(accW.indexInParent(), 0);
    QVERIFY(accW.isActive());

    // Button
    AccessibleObject accButton = accW.child(0);
    QVERIFY(accButton.isValid());
    QCOMPARE(accButton.name(), button->text());
    QCOMPARE(accButton.description(), desc);
    QCOMPARE(accButton.role(), ATSPI_ROLE_PUSH_BUTTON);
    QCOMPARE(accButton.roleName(), QLatin1String("push button"));
    QVERIFY(!accButton.localizedRoleName().isEmpty());
    QCOMPARE(accButton.indexInParent(), 0);

    AccessibleObject accButton2 = accW.children().first();
    QCOMPARE(accButton, accButton2);
    AccessibleObject parent = accButton.parent();
    QCOMPARE(parent, accW);
    AccessibleObject parentParent = parent.parent();
    QCOMPARE(parentParent, accApp);

    AccessibleObject invalidChild = accButton.child(0);
    QVERIFY(!invalidChild.isValid());
    QVERIFY(invalidChild.name().isEmpty());

    AccessibleObject invalidParent = accApp.parent();
    QVERIFY(!invalidParent.isValid());
    QVERIFY(invalidParent.name().isEmpty());

    // Add a label and line edit
    QLabel *label = new QLabel;
    label->setText("Name:");
    layout->addWidget(label);
    QLineEdit *line = new QLineEdit;
    layout->addWidget(line);
    label->setBuddy(line);
    QApplication::processEvents();
    QCOMPARE(accW.childCount(), 3);

    AccessibleObject accLabel = accW.child(1);
    QVERIFY(accLabel.isValid());
    QCOMPARE(accLabel.name(), label->text());
    QCOMPARE(accLabel.role(), ATSPI_ROLE_LABEL);
    QCOMPARE(accLabel.roleName(), QLatin1String("label"));
    QCOMPARE(accLabel.indexInParent(), 1);
    QVERIFY(accLabel.isVisible());
    QVERIFY(!accLabel.isCheckable());
    QVERIFY(!accLabel.isChecked());
    QVERIFY(!accLabel.isFocusable());
    QVERIFY(!accLabel.isFocused());
    QEXPECT_FAIL("", "Labels in Qt 4 report themselves as editable.", Continue);
    QVERIFY(!accLabel.isEditable());

    AccessibleObject accLine = accW.child(2);
    QVERIFY(accLine.isValid());
    QCOMPARE(accLine.name(), label->text());
    QCOMPARE(accLine.role(), ATSPI_ROLE_TEXT);
    QCOMPARE(accLine.roleName(), QLatin1String("text"));
    QCOMPARE(accLine.indexInParent(), 2);
    QVERIFY(accLine.isEditable());
    AccessibleObject parent1 = accLine.parent();
    QCOMPARE(parent1, accW);

    QVERIFY(accLine.isFocusable());
    QVERIFY(accButton.isFocusable());
    QVERIFY(accButton.isFocused());
    QVERIFY(!accLine.isFocused());
    line->setFocus();
    QApplication::processEvents();
    QVERIFY(accLine.isFocused());
    QVERIFY(!accButton.isFocused());

    label->setVisible(false);
    line->setVisible(false);
    QApplication::processEvents();
    QTest::qWait(1000);
    QVERIFY(!accLabel.isVisible());
    QVERIFY(!accLine.isVisible());
}

void AccessibilityClientTest::tst_focus()
{
    registry.subscribeEventListeners(Registry::Focus);
    EventListener *listener = new EventListener;
    connect(&registry, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)), listener, SLOT(focus(KAccessibleClient::AccessibleObject)));

    QProcess proc;
    // start peer server
    #ifdef Q_OS_WIN
    proc.start("simplewidgetapp");
    #else
    proc.start("./simplewidgetapp");
    #endif
    QVERIFY(proc.waitForStarted());

    AccessibleObject remoteApp;
    QString appName = QLatin1String("LibKdeAccessibilityClient Simple Widget App");
    // startup and init takes some time, give up to two seconds and a few dbus calls
    int attempts = 0;
    while (attempts < 20) {
        ++attempts;
        QTest::qWait(100);
        remoteApp = getAppObject(registry, appName);
        if (remoteApp.isValid())
            break;
    }

    // waiting for two events, may take some time
    for (int i = 0; i < 20; ++i) {
        QTest::qWait(10);
        if (listener->focusEvents.size() >= 2)
            break;
    }

    QVERIFY(remoteApp.isValid());
    QCOMPARE(remoteApp.name(), appName);

    AccessibleObject window = remoteApp.child(0);
    AccessibleObject button1 = window.child(0);
    AccessibleObject button2 = window.child(1);
    QCOMPARE(listener->focusEvents.size(), 2);
    QCOMPARE(listener->focusEvents.at(0).object, button2);
    QCOMPARE(listener->focusEvents.at(1).object, button1);

    // use action interface to select the first button again and check that we get an event

    delete listener;
    proc.terminate();
}


QTEST_MAIN(AccessibilityClientTest)

#include "tst_accessibilityclient.moc"
