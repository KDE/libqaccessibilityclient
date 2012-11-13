/*
 *  Copyright 2012 Frederik Gladhorn <gladhorn@kde.org>
 *  Copyright 2012 Sebastian Sauer <sebastian.sauer@kdab.com>
 * 
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLIENTCACHEDIALOG_H
#define CLIENTCACHEDIALOG_H

#include <qdialog.h>

#include "qaccessibilityclient/accessibleobject.h"
#include "qaccessibilityclient/registry.h"

class QComboBox;
class QLabel;
class QStandardItemModel;
class QTreeView;

class ClientCacheDialog : public QDialog
{
    Q_OBJECT
public:
    ClientCacheDialog(QAccessibleClient::Registry *registry, QWidget *parent = 0);
private slots:
    void clearCache();
    void cacheStrategyChanged();
    void updateView();
private:
    QAccessibleClient::Registry *m_registry;
    QAccessibleClient::RegistryPrivateCacheApi *m_cache;
    QTreeView *m_view;
    QStandardItemModel *m_model;
    QComboBox *m_cacheCombo;
    QLabel *m_countLabel;
};

#endif
