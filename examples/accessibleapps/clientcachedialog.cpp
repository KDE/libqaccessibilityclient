/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>
    SPDX-FileCopyrightText: 2012 Sebastian Sauer <sebastian.sauer@kdab.com>


    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "clientcachedialog.h"

#include "qaccessibilityclient/registrycache_p.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTreeView>

ClientCacheDialog::ClientCacheDialog(QAccessibleClient::Registry *registry, QWidget *parent)
: QDialog(parent)
, m_registry(registry)
, m_cache(new QAccessibleClient::RegistryPrivateCacheApi(m_registry))
{
    setModal(true);
    QVBoxLayout *lay = new QVBoxLayout(this);
    setLayout(lay);
    
    m_view = new QTreeView(this);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setRootIsDecorated(false);
    m_view->setSortingEnabled(true);
    m_view->setItemsExpandable(false);
    //list->setHeaderHidden(true);
    m_model = new QStandardItemModel(m_view);
    m_model->setColumnCount(3);
    m_view->setModel(m_model);
    lay->addWidget(m_view);
    
    QHBoxLayout *buttonsLay = new QHBoxLayout(this);
    buttonsLay->setContentsMargins(0, 0, 0, 0);
    buttonsLay->setSpacing(0);
    QPushButton *updateButton = new QPushButton(QStringLiteral("Refresh"), this);
    buttonsLay->addWidget(updateButton);
    connect(updateButton, SIGNAL(clicked(bool)), this, SLOT(updateView()));
    QPushButton *clearButton = new QPushButton(QStringLiteral("Clear"), this);
    buttonsLay->addWidget(clearButton);
    
    QLabel *cacheLabel = new QLabel(QStringLiteral("Strategy:"), this);
    buttonsLay->addWidget(cacheLabel);
    m_cacheCombo = new QComboBox(this);
    cacheLabel->setBuddy(m_cacheCombo);
    m_cacheCombo->setEditable(false);
    m_cacheCombo->addItem(QStringLiteral("Disable"), int(QAccessibleClient::RegistryPrivateCacheApi::NoCache));
    m_cacheCombo->addItem(QStringLiteral("Weak"), int(QAccessibleClient::RegistryPrivateCacheApi::WeakCache));
    for(int i = 0; i < m_cacheCombo->count(); ++i) {
        if (m_cacheCombo->itemData(i).toInt() == m_cache->cacheType()) {
            m_cacheCombo->setCurrentIndex(i);
            break;
        }
    }
    connect(m_cacheCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cacheStrategyChanged()));
    buttonsLay->addWidget(m_cacheCombo);
    buttonsLay->addWidget(new QLabel(QStringLiteral("Count:"), this));
    m_countLabel = new QLabel(this);
    buttonsLay->addWidget(m_countLabel);
    buttonsLay->addStretch(1);
    
    connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearCache()));
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    buttonsLay->addWidget(buttons);
    QPushButton *closeButton = buttons->button(QDialogButtonBox::Close);
    connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
    lay->addLayout(buttonsLay);
    
    resize(minimumSize().expandedTo(QSize(660,420)));
    
    updateView();
    m_view->sortByColumn(2, Qt::AscendingOrder);
}

void ClientCacheDialog::clearCache()
{
    m_cache->clearClientCache();
    updateView();
}

void ClientCacheDialog::cacheStrategyChanged()
{
    int c = m_cacheCombo->itemData(m_cacheCombo->currentIndex()).toInt();
    m_cache->setCacheType(QAccessibleClient::RegistryPrivateCacheApi::CacheType(c));
    updateView();
}

void ClientCacheDialog::updateView()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels( QStringList() << QStringLiteral("Name") << QStringLiteral("Role") << QStringLiteral("Identifier") );
    const QStringList cache = m_cache->clientCacheObjects();
    m_countLabel->setText(QString::number(cache.count()));
    for (const QString &c : cache) {
        QAccessibleClient::AccessibleObject obj = m_cache->clientCacheObject(c);
        if (obj.isValid())
            m_model->appendRow( QList<QStandardItem*>()
            << new QStandardItem(obj.name())
            << new QStandardItem(obj.roleName())
            << new QStandardItem(obj.id()) );
    }
    m_view->setColumnWidth(0, 180);
    m_view->resizeColumnToContents(1);
    m_view->resizeColumnToContents(2);
}

#include "moc_clientcachedialog.cpp"
