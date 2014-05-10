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

#include "ui_mainwindow.h"

#include <QAbstractListModel>

class AppList : public QAbstractListModel
{
    Q_OBJECT
public:
    AppList();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void appsChanged();

private:
    QAccessibleClient::Registry *m_registry;
    QVector<QAccessibleClient::AccessibleObject*> m_apps;
};

class MainWindow :public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private:
    AppList m_appList;
    Ui::MainWindowUi m_ui;
};

#endif
