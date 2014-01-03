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

#include "clientcachedialog.h"

#include "qaccessibilityclient/registrycache_p.h"

#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialogbuttonbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstandarditemmodel.h>
#include <qtreeview.h>

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
    buttonsLay->setMargin(0);
    buttonsLay->setSpacing(lay->margin());
    QPushButton *updateButton = new QPushButton(QString("Refresh"), this);
    buttonsLay->addWidget(updateButton);
    connect(updateButton, SIGNAL(clicked(bool)), this, SLOT(updateView()));
    QPushButton *clearButton = new QPushButton(QString("Clear"), this);
    buttonsLay->addWidget(clearButton);
    
    QLabel *cacheLabel = new QLabel(QString("Strategy:"), this);
    buttonsLay->addWidget(cacheLabel);
    m_cacheCombo = new QComboBox(this);
    cacheLabel->setBuddy(m_cacheCombo);
    m_cacheCombo->setEditable(false);
    m_cacheCombo->addItem(QString("Disable"), int(QAccessibleClient::RegistryPrivateCacheApi::NoCache));
    m_cacheCombo->addItem(QString("Weak"), int(QAccessibleClient::RegistryPrivateCacheApi::WeakCache));
    m_cacheCombo->addItem(QString("Strong"), int(QAccessibleClient::RegistryPrivateCacheApi::StrongCache));
    for(int i = 0; i < m_cacheCombo->count(); ++i) {
        if (m_cacheCombo->itemData(i).toInt() == m_cache->cacheType()) {
            m_cacheCombo->setCurrentIndex(i);
            break;
        }
    }
    connect(m_cacheCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cacheStrategyChanged()));
    buttonsLay->addWidget(m_cacheCombo);
    buttonsLay->addWidget(new QLabel(QString("Count:"), this));
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
    m_model->setHorizontalHeaderLabels( QStringList() << QString("Name") << QString("Role") << QString("Identifier") );
    QStringList cache = m_cache->clientCacheObjects();
    m_countLabel->setText(QString::number(cache.count()));
    Q_FOREACH(const QString &c, cache) {
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
