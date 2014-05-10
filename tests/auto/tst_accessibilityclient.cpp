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

#include <qtest.h>

#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qaccessible.h>
#include <qdebug.h>
#include <qprocess.h>
#include <qfileinfo.h>
#include <qsignalspy.h>

#include "qaccessibilityclient/qaccessibilityclient_export.h"
#include "qaccessibilityclient/registry.h"
#include "qaccessibilityclient/accessibleobject.h"

#include "atspi/dbusconnection.h"

typedef QSharedPointer<QAccessibleInterface> QAIPointer;

using namespace QAccessibleClient;

struct Event {
    Event(AccessibleObject *obj)
        : object(obj)
    {}

    AccessibleObject *object;
};

class EventListener : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void focus(QAccessibleClient::AccessibleObject *object) {
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
    void cleanup();

    void tst_registry();
    void tst_application();
    void tst_navigation();
    void tst_focus();
    void tst_states();

    void tst_extents();

    void tst_characterExtents();

private:
    bool startHelperProcess();
    Registry registry;
    QProcess helperProcess;
};

void AccessibilityClientTest::initTestCase()
{
    if (qgetenv("QT_ACCESSIBILITY") != QByteArray("1"))
        qWarning() << "QT_ACCESSIBILITY=1 not found, this leads to failing tests with Qt 4";
}

AccessibleObject *getAppObject(Registry &registry, const QString &appName)
{
//    qDebug() << "Get app object: " << appName;
    QTest::qWait(1000); // give the app time to register on DBus
    QSignalSpy spy(&registry, SIGNAL(applicationsChanged()));
    registry.updateApplications();
    spy.wait();

    AccessibleObject *accApp = 0;

    QApplication::processEvents();
    QVector<AccessibleObject*> apps = registry.applications();
    foreach (AccessibleObject *app, apps) {
        if (app->name() == appName) {
            accApp = app;
            break;
        }
    }
    return accApp;
}

void AccessibilityClientTest::cleanup()
{
    registry.subscribeEventListeners(Registry::NoEventListeners);
}

void AccessibilityClientTest::tst_registry()
{
    QVERIFY(registry.subscribedEventListeners() == Registry::NoEventListeners);
    registry.subscribeEventListeners(Registry::Window);
    QVERIFY(registry.subscribedEventListeners() == Registry::Window);
    registry.subscribeEventListeners(Registry::Focus);
    QVERIFY(registry.subscribedEventListeners() == Registry::Focus);
    registry.subscribeEventListeners(Registry::Focus | Registry::Window);
    QVERIFY(registry.subscribedEventListeners() == Registry::Focus | Registry::Window);

    registry.subscribeEventListeners(Registry::NoEventListeners);
    QVERIFY(registry.subscribedEventListeners() == Registry::NoEventListeners);
    registry.subscribeEventListeners(Registry::AllEventListeners);
    QVERIFY(registry.subscribedEventListeners() == Registry::AllEventListeners);
    QVERIFY(registry.subscribedEventListeners() & Registry::Window);
}

void AccessibilityClientTest::tst_application()
{
    QString appName = QLatin1String("Lib QAccessibleClient test");
    qApp->setApplicationName(appName);
    QWidget w;
    w.setAccessibleName("Foobar 99");
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));

    AccessibleObject *accApp = 0;
    accApp = getAppObject(registry, appName);
    QVERIFY(accApp);
    QVERIFY(accApp->isValid());
    QCOMPARE(accApp->name(), appName);
    QCOMPARE(accApp->childCount(), 1);
}

void AccessibilityClientTest::tst_navigation()
{
    QString appName = QLatin1String("Lib QAccessibleClient test");
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
    w.activateWindow();
    button->setFocus();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTest::qWaitForWindowExposed(&w);
    QTest::qWaitForWindowActive(&w);
#else
    QTest::qWaitForWindowShown(&w);
#endif

    // App
    AccessibleObject *accApp = getAppObject(registry, appName);
    QVERIFY(accApp->isValid());
    QCOMPARE(accApp->name(), appName);
    QCOMPARE(accApp->childCount(), 1);

    // What should this return?
    QCOMPARE(accApp->indexInParent(), -1);

    // Root widget
    AccessibleObject *accW = accApp->child(0);
    QVERIFY(accW->isValid());
    qDebug() << "NAME: " << accW->name();
    QCOMPARE(accW->name(), w.accessibleName());
    QCOMPARE(accW->description(), w.accessibleDescription());
    QCOMPARE(accW->role(), AccessibleObject::Filler);
    QCOMPARE(accW->roleName(), QLatin1String("filler"));
    QCOMPARE(accW->childCount(), 1);
    QCOMPARE(accW->indexInParent(), 0);
    QVERIFY(accW->isActive());

    // Button
    AccessibleObject *accButton = accW->child(0);
    QVERIFY(accButton->isValid());
    QCOMPARE(accButton->name(), button->text());
    QCOMPARE(accButton->description(), desc);
    QCOMPARE(accButton->role(), AccessibleObject::Button);
    QCOMPARE(accButton->roleName(), QLatin1String("push button"));
    QVERIFY(!accButton->localizedRoleName().isEmpty());
    QCOMPARE(accButton->indexInParent(), 0);

    AccessibleObject *accButton2 = accW->children().first();
    QCOMPARE(accButton, accButton2);
    AccessibleObject *parent = accButton->accessibleParent();
    QCOMPARE(parent, accW);
    AccessibleObject *parentParent = parent->accessibleParent();
    QCOMPARE(parentParent, accApp);

    AccessibleObject *invalidChild = accButton->child(0);
    QVERIFY(!invalidChild);

    AccessibleObject *invalidParent = accApp->accessibleParent();
    QVERIFY(!invalidParent);

    // Add a label and line edit
    QLabel *label = new QLabel;
    label->setText("Name:");
    layout->addWidget(label);
    QLineEdit *line = new QLineEdit;
    layout->addWidget(line);
    label->setBuddy(line);
    QApplication::processEvents();
    QCOMPARE(accW->childCount(), 3);

    AccessibleObject *accLabel = accW->child(1);
    QVERIFY(accLabel->isValid());
    QCOMPARE(accLabel->name(), label->text());
    QCOMPARE(accLabel->role(), AccessibleObject::Label);
    QCOMPARE(accLabel->roleName(), QLatin1String("label"));
    QCOMPARE(accLabel->indexInParent(), 1);
    QVERIFY(accLabel->isVisible());
    QVERIFY(!accLabel->isCheckable());
    QVERIFY(!accLabel->isChecked());
    QVERIFY(!accLabel->isFocusable());
    QVERIFY(!accLabel->isFocused());
#if (QT_VERSION < QT_VERSION_CHECK(5, 2, 1))
    QEXPECT_FAIL("", "Labels in Qt 4 report themselves as editable.", Continue);
#endif
    QVERIFY(!accLabel->isEditable());

    AccessibleObject *accLine = accW->child(2);
    QVERIFY(accLine->isValid());
    QCOMPARE(accLine->name(), label->text());
    QCOMPARE(accLine->role(), AccessibleObject::Text);
    QCOMPARE(accLine->roleName(), QLatin1String("text"));
    QCOMPARE(accLine->indexInParent(), 2);
    QVERIFY(accLine->isEditable());
    AccessibleObject *parent1 = accLine->accessibleParent();
    QCOMPARE(parent1, accW);

    QVERIFY(accLine->isFocusable());
    QVERIFY(accButton->isFocusable());
    QVERIFY(accButton->isFocused());
    QVERIFY(!accLine->isFocused());
    line->setFocus();
    QApplication::processEvents();
    QVERIFY(accLine->isFocused());
    QVERIFY(!accButton->isFocused());

    label->setVisible(false);
    line->setVisible(false);
    QApplication::processEvents();
    QTest::qWait(1000);
    QVERIFY(!accLabel->isVisible());
    QVERIFY(!accLine->isVisible());
}

bool AccessibilityClientTest::startHelperProcess()
{
    if (!QFileInfo("./simplewidgetapp").exists()) {
        qWarning() << "WARNING: Could not find test case helper executable."
            " Please run this test in the path where the executable is located.";
        return false;
    }

    // start peer server
#ifdef Q_OS_WIN
    proc.start("simplewidgetapp");
#else
    helperProcess.start("./simplewidgetapp");
#endif
    if (!helperProcess.waitForStarted()) {
        qWarning() << "WARNING: Could not start helper executable. Test will not run.";
        return false;
    }
    QTest::qWait(1000); // let the thing start and say hello to dbus
    return true;
}

void AccessibilityClientTest::tst_focus()
{
    registry.subscribeEventListeners(Registry::Focus);
    EventListener *listener = new EventListener;
    connect(&registry, SIGNAL(focusChanged(QAccessibleClient::AccessibleObject)), listener, SLOT(focus(QAccessibleClient::AccessibleObject)));

    QVERIFY(startHelperProcess());

    AccessibleObject *remoteApp;
    QString appName = QLatin1String("LibKdeAccessibilityClient Simple Widget App");
    // startup and init takes some time, give up to two seconds and a few dbus calls
    int attempts = 0;
    while (attempts < 20) {
        ++attempts;
        QTest::qWait(100);
        remoteApp = getAppObject(registry, appName);
        if (remoteApp)
            break;
    }

    // waiting for two events, may take some time
    for (int i = 0; i < 20; ++i) {
        QTest::qWait(10);
        if (listener->focusEvents.size() >= 2)
            break;
    }

    QVERIFY(remoteApp);
    QVERIFY(remoteApp->isValid());
    QCOMPARE(remoteApp->name(), appName);

    AccessibleObject *window = remoteApp->child(0);
    AccessibleObject *button1 = window->child(0);
    AccessibleObject *button2 = window->child(1);

    // we can get other focus events, check that we only use the ones from our app
    for (int i = 0; i < listener->focusEvents.count(); ++i) {
        AccessibleObject *ev = listener->focusEvents.at(i).object;
        if (ev->application() != remoteApp)
            listener->focusEvents.removeAt(i);;
    }
    QVERIFY(listener->focusEvents.size() == 2);
    QCOMPARE(listener->focusEvents.at(0).object, button1);
    QCOMPARE(listener->focusEvents.at(1).object, button2);

    // use action interface to select the first button again and check that we get an event

    delete listener;
    helperProcess.terminate();
}

void AccessibilityClientTest::tst_states()
{
    registry.subscribeEventListeners(Registry::StateChanged);

    QString appName = QLatin1String("Lib QAccessibleClient test");
    qApp->setApplicationName(appName);
    QWidget w;
    w.setAccessibleName("Root Widget");
    w.setAccessibleDescription("This is a useless widget");
    QVBoxLayout *layout = new QVBoxLayout;
    w.setLayout(layout);

    QPushButton *button1 = new QPushButton;
    layout->addWidget(button1);
    button1->setText(QLatin1String("Hello a11y"));
    QString desc = "This is a button...";
    button1->setAccessibleDescription(desc);

    QPushButton *button2 = new QPushButton;
    layout->addWidget(button2);
    button2->setText(QLatin1String("Hello a11y"));

    w.show();
    button1->setFocus();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTest::qWaitForWindowExposed(&w);
#else
    QTest::qWaitForWindowShown(&w);
#endif

    AccessibleObject *accApp = getAppObject(registry, appName);
    QVERIFY(accApp);

    // Root widget
    AccessibleObject *accW = accApp->child(0);
    QVERIFY(accW);

    // Buttons
    AccessibleObject *accButton1 = accW->child(0);
    QVERIFY(accButton1);
    QCOMPARE(accButton1->name(), button1->text());

    AccessibleObject *accButton2 = accW->child(1);
    QVERIFY(accButton2);
    QCOMPARE(accButton2->name(), button2->text());

    QVERIFY(accButton1->isVisible());
    button1->setVisible(false);
    QVERIFY(!accButton1->isVisible());
    button1->setVisible(true);
    QVERIFY(accButton1->isVisible());

    QVERIFY(accButton1->isEnabled());
    button1->setEnabled(false);
    QVERIFY(!accButton1->isEnabled());
    button1->setEnabled(true);
    QVERIFY(accButton1->isEnabled());
}

void AccessibilityClientTest::tst_extents()
{
    QVERIFY(startHelperProcess());

    AccessibleObject *remoteApp;
    QString appName = QLatin1String("LibKdeAccessibilityClient Simple Widget App");

    int attempts = 0;
    while(attempts < 20) {
        ++attempts;
        QTest::qWait(100);
        remoteApp = getAppObject(registry,appName);
        if(remoteApp)
            break;
    }

    QVERIFY(remoteApp->isValid());
    QCOMPARE(remoteApp->name(), appName);

    AccessibleObject *window = remoteApp->child(0);
    QVERIFY(window->supportedInterfaces() & QAccessibleClient::AccessibleObject::ComponentInterface);
    QCOMPARE(window->boundingRect(),QRect(3,23,200,100));

    AccessibleObject *button1 = window->child(0);
    QVERIFY(button1->name()=="Button 1");
    QCOMPARE(button1->boundingRect(),QRect(13,33,100,20));
    helperProcess.terminate();
}

void AccessibilityClientTest::tst_characterExtents()
{
    QString appName = QLatin1String("Lib QAccessibleClient test");

    QWidget w;
    w.setAccessibleName("Root Widget");
    QTextEdit *textEdit = new QTextEdit(&w);
    textEdit->setGeometry(10,10,600,400);
    w.show();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QTest::qWaitForWindowExposed(&w);
#else
    QTest::qWaitForWindowShown(&w);
#endif
    AccessibleObject *app = getAppObject(registry, appName);

    //Check if the widget is correct
    QVERIFY(app);
    QVERIFY(app->isValid());
    QCOMPARE(app->name(), appName);
    QCOMPARE(app->childCount(), 1);

    AccessibleObject *textArea = app->child(0)->child(0);
    QVERIFY(textArea->supportedInterfaces() & QAccessibleClient::AccessibleObject::TextInterface);

    textEdit->setText("This is useless text that is being used to test this text area.\n I \n hope \n this will get correct\n\t\t\tCharacterExtents!");
    QPoint pos = w.pos();

    int start;
    int end;
    QString textWord = textArea->textWithBoundary(0, AccessibleObject::WordStartBoundary, &start, &end);
    QCOMPARE(textWord, QStringLiteral("This"));
    QCOMPARE(start, 0);
    QCOMPARE(end, 4);
    textWord = textArea->textWithBoundary(6, AccessibleObject::WordStartBoundary, &start, &end);
    QCOMPARE(textWord , QStringLiteral("is"));
    QCOMPARE(start, 5);
    QCOMPARE(end, 7);
    textWord = textArea->textWithBoundary(3, AccessibleObject::WordEndBoundary);
    QCOMPARE(textWord , QStringLiteral("This"));

    QString textSentence = textArea->textWithBoundary(0, AccessibleObject::SentenceEndBoundary);
    QCOMPARE(textSentence, QStringLiteral("This is useless text that is being used to test this text area."));
    QString textLine = textArea->textWithBoundary(0, AccessibleObject::LineEndBoundary);
    QCOMPARE(textLine, QStringLiteral("This is useless text that is being used to test this text area."));
    textLine = textArea->textWithBoundary(0, AccessibleObject::LineEndBoundary);
    QCOMPARE(textLine, QStringLiteral("This is useless text that is being used to test this text area."));

    QCOMPARE(textArea->characterRect(0), QRect(20,40,7,14).translated(pos));
    QCOMPARE(textArea->characterRect(1), QRect(20,40,7,14));
}


QTEST_MAIN(AccessibilityClientTest)

#include "tst_accessibilityclient.moc"
