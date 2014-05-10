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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qtreeview.h>
#include <qtextbrowser.h>
#include <qaction.h>
#include <qdebug.h>

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
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private Q_SLOTS:
    void treeSelectionChanged(const QModelIndex &current, const QModelIndex &);
    void treeCustomContextMenuRequested(const QPoint &pos);
    void anchorClicked(const QUrl &url);
    void showClientCache();

    void added(QAccessibleClient::AccessibleObject *object);
    void removed(QAccessibleClient::AccessibleObject *object);
    void defunct(QAccessibleClient::AccessibleObject *object);

    void windowCreated(QAccessibleClient::AccessibleObject *object);
    void windowDestroyed(QAccessibleClient::AccessibleObject *object);
    void windowClosed(QAccessibleClient::AccessibleObject *object);
    void windowReparented(QAccessibleClient::AccessibleObject *object);
    void windowMinimized(QAccessibleClient::AccessibleObject *object);
    void windowMaximized(QAccessibleClient::AccessibleObject *object);
    void windowRestored(QAccessibleClient::AccessibleObject *object);
    void windowActivated(QAccessibleClient::AccessibleObject *object);
    void windowDeactivated(QAccessibleClient::AccessibleObject *object);
    void windowDesktopCreated(QAccessibleClient::AccessibleObject *object);
    void windowDesktopDestroyed(QAccessibleClient::AccessibleObject *object);
    void windowRaised(QAccessibleClient::AccessibleObject *object);
    void windowLowered(QAccessibleClient::AccessibleObject *object);
    void windowMoved(QAccessibleClient::AccessibleObject *object);
    void windowResized(QAccessibleClient::AccessibleObject *object);
    void windowShaded(QAccessibleClient::AccessibleObject *object);
    void windowUnshaded(QAccessibleClient::AccessibleObject *object);

    void stateChanged(QAccessibleClient::AccessibleObject *object, const QString &state, bool active);
    void childAdded(QAccessibleClient::AccessibleObject *object, int childIndex);
    void childRemoved(QAccessibleClient::AccessibleObject *object, int childIndex);
    void visibleDataChanged(QAccessibleClient::AccessibleObject *object);
    void selectionChanged(QAccessibleClient::AccessibleObject *object);
    void modelChanged(QAccessibleClient::AccessibleObject *object);

    void focusChanged(QAccessibleClient::AccessibleObject *object);
    void textCaretMoved(QAccessibleClient::AccessibleObject *object, int pos);
    void textSelectionChanged(QAccessibleClient::AccessibleObject *object);

    void textChanged(QAccessibleClient::AccessibleObject *object, const QString& text, int startOffset, int endOffset);
    void textInserted(QAccessibleClient::AccessibleObject *object, const QString& text, int startOffset, int endOffset);
    void textRemoved(QAccessibleClient::AccessibleObject *object, const QString& text, int startOffset, int endOffset);

    void accessibleNameChanged(QAccessibleClient::AccessibleObject *object);
    void accessibleDescriptionChanged(QAccessibleClient::AccessibleObject *object);

private:
    void initActions();
    void initMenu();
    void initUi();

    void addLog(QAccessibleClient::AccessibleObject *object, const QString &eventName, const QString &text = QString());

    void setCurrentObject(QAccessibleClient::AccessibleObject *object);
    void updateDetails(QAccessibleClient::AccessibleObject *object, bool force = false);

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
};

#endif
