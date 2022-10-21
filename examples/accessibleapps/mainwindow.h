/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QTextBrowser>
#include <QAction>
#include <QDebug>

#include "qaccessibilityclient/accessibleobject.h"
#include "qaccessibilityclient/registry.h"

#include "clientcachedialog.h"

class AccessibleTree;
class ObjectProperties;
class EventsWidget;
class UiView;


class MainWindow :public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void selectionChanged(const QModelIndex &current, const QModelIndex &);
    void treeCustomContextMenuRequested(const QPoint &pos);
    void anchorClicked(const QUrl &url);
    void showClientCache();
    void copyValue();

    void added(const QAccessibleClient::AccessibleObject &object);
    void removed(const QAccessibleClient::AccessibleObject &object);
    void defunct(const QAccessibleClient::AccessibleObject &object);

    void windowCreated(const QAccessibleClient::AccessibleObject &object);
    void windowDestroyed(const QAccessibleClient::AccessibleObject &object);
    void windowClosed(const QAccessibleClient::AccessibleObject &object);
    void windowReparented(const QAccessibleClient::AccessibleObject &object);
    void windowMinimized(const QAccessibleClient::AccessibleObject &object);
    void windowMaximized(const QAccessibleClient::AccessibleObject &object);
    void windowRestored(const QAccessibleClient::AccessibleObject &object);
    void windowActivated(const QAccessibleClient::AccessibleObject &object);
    void windowDeactivated(const QAccessibleClient::AccessibleObject &object);
    void windowDesktopCreated(const QAccessibleClient::AccessibleObject &object);
    void windowDesktopDestroyed(const QAccessibleClient::AccessibleObject &object);
    void windowRaised(const QAccessibleClient::AccessibleObject &object);
    void windowLowered(const QAccessibleClient::AccessibleObject &object);
    void windowMoved(const QAccessibleClient::AccessibleObject &object);
    void windowResized(const QAccessibleClient::AccessibleObject &object);
    void windowShaded(const QAccessibleClient::AccessibleObject &object);
    void windowUnshaded(const QAccessibleClient::AccessibleObject &object);

    void stateChanged(const QAccessibleClient::AccessibleObject &object, const QString &state, bool active);
    void childAdded(const QAccessibleClient::AccessibleObject &object, int childIndex);
    void childRemoved(const QAccessibleClient::AccessibleObject &object, int childIndex);
    void visibleDataChanged(const QAccessibleClient::AccessibleObject &object);
    void selectionChanged(const QAccessibleClient::AccessibleObject &object);
    void modelChanged(const QAccessibleClient::AccessibleObject &object);

    void focusChanged(const QAccessibleClient::AccessibleObject &object);
    void textCaretMoved(const QAccessibleClient::AccessibleObject &object, int pos);
    void textSelectionChanged(const QAccessibleClient::AccessibleObject &object);

    void textChanged(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset);
    void textInserted(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset);
    void textRemoved(const QAccessibleClient::AccessibleObject &object, const QString& text, int startOffset, int endOffset);

    void accessibleNameChanged(const QAccessibleClient::AccessibleObject &object);
    void accessibleDescriptionChanged(const QAccessibleClient::AccessibleObject &object);

private:
    void initActions();
    void initMenu();
    void initUi();

    void addLog(const QAccessibleClient::AccessibleObject &object, const QString &eventName, const QString &text = QString());

    void setCurrentObject(const QAccessibleClient::AccessibleObject &object);
    void updateDetails(const QAccessibleClient::AccessibleObject &object, bool force = false);

    QAccessibleClient::Registry *m_registry;

    QTreeView *m_accessibleObjectTreeView;
    AccessibleTree *m_accessibleObjectTreeModel;

    QTreeView *m_propertyView;
    ObjectProperties *m_propertyModel;

    EventsWidget *m_eventsWidget;
    UiView *m_uiview;

    QAction *m_resetTreeAction;
    QAction *m_followFocusAction;
    QAction *m_showClientCacheAction;
    QAction *m_enableA11yAction;
    QAction *m_enableScreenReaderAction;
    QAction *m_quitAction;
    QAction *m_copyValueAction;
};

#endif
