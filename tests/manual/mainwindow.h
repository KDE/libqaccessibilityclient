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
#include <qdialog.h>
#include <qdebug.h>

#include "kdeaccessibilityclient/accessibleobject.h"
#include "kdeaccessibilityclient/registry.h"

class AccessibleTree;
class ObjectProperties;
class UiView;

class QTreeView;
class QStandardItemModel;
class QComboBox;
class QLabel;

class ClientCacheDialog : public QDialog
{
    Q_OBJECT
public:
    ClientCacheDialog(KAccessibleClient::Registry *registry, QWidget *parent = 0);
private slots:
    void clearCache();
    void cacheStrategyChanged();
    void updateView();
private:
    KAccessibleClient::Registry *m_registry;
    QTreeView *m_view;
    QStandardItemModel *m_model;
    QComboBox *m_cacheCombo;
    QLabel *m_countLabel;
};

class MainWindow :public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void listAccessibles();

protected:
    void closeEvent(QCloseEvent *event);

private Q_SLOTS:
    void selectionChanged(const QModelIndex &current, const QModelIndex &);
    void treeCustomContextMenuRequested(const QPoint &pos);
    void anchorClicked(const QUrl &url);
    void showClientCache();

    void added(const KAccessibleClient::AccessibleObject &object);
    void removed(const KAccessibleClient::AccessibleObject &object);
    void defunct(const KAccessibleClient::AccessibleObject &object);

    void windowCreated(const KAccessibleClient::AccessibleObject &object);
    void windowDestroyed(const KAccessibleClient::AccessibleObject &object);
    void windowClosed(const KAccessibleClient::AccessibleObject &object);
    void windowReparented(const KAccessibleClient::AccessibleObject &object);
    void windowMinimized(const KAccessibleClient::AccessibleObject &object);
    void windowMaximized(const KAccessibleClient::AccessibleObject &object);
    void windowRestored(const KAccessibleClient::AccessibleObject &object);
    void windowActivated(const KAccessibleClient::AccessibleObject &object);
    void windowDeactivated(const KAccessibleClient::AccessibleObject &object);
    void windowDesktopCreated(const KAccessibleClient::AccessibleObject &object);
    void windowDesktopDestroyed(const KAccessibleClient::AccessibleObject &object);
    void windowRaised(const KAccessibleClient::AccessibleObject &object);
    void windowLowered(const KAccessibleClient::AccessibleObject &object);
    void windowMoved(const KAccessibleClient::AccessibleObject &object);
    void windowResized(const KAccessibleClient::AccessibleObject &object);
    void windowShaded(const KAccessibleClient::AccessibleObject &object);
    void windowUnshaded(const KAccessibleClient::AccessibleObject &object);

    void stateChanged(const KAccessibleClient::AccessibleObject &object, const QString &state, int detail1, int detail2);
    void childrenChanged(const KAccessibleClient::AccessibleObject &object, const QString &state, int detail1, int detail2);
    void visibleDataChanged(const KAccessibleClient::AccessibleObject &object);
    void selectionChanged(const KAccessibleClient::AccessibleObject &object);
    void modelChanged(const KAccessibleClient::AccessibleObject &object);

    void focusChanged(const KAccessibleClient::AccessibleObject &object);
    void textCaretMoved(const KAccessibleClient::AccessibleObject &object, int pos);
    void textSelectionChanged(const KAccessibleClient::AccessibleObject &object);
    void textChanged(const KAccessibleClient::AccessibleObject &object);

private:
    KAccessibleClient::Registry *m_registry;

    QTreeView *m_treeView;
    AccessibleTree *m_treeModel;

    QTreeView *m_propertyView;
    ObjectProperties *m_propertyModel;

    QTextBrowser *m_eventsEdit;
    UiView *m_uiview;

    QAction *m_resetTreeAction;
    QAction *m_followFocusAction;
    QAction *m_showClientCacheAction;
    QAction *m_enableA11yAction;
    QAction *m_quitAction;

    void initActions();
    void initMenu();
    void initUi();

    void addLog(const KAccessibleClient::AccessibleObject &object, const QString &eventName, const QString &text = QString());
    void setCurrentObject(const KAccessibleClient::AccessibleObject &object);
};

#endif
