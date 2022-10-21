/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>


    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef CLIENTCACHEDIALOG_H
#define CLIENTCACHEDIALOG_H

#include <QDialog>

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
    ClientCacheDialog(QAccessibleClient::Registry *registry, QWidget *parent = nullptr);
private Q_SLOTS:
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
