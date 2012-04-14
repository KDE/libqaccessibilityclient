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

#include "mainwindow.h"

#include <qitemselectionmodel.h>
#include <qplaintextedit.h>
#include <qstring.h>
#include <qtreeview.h>

#include "kdeaccessibilityclient/registry.h"
#include "kdeaccessibilityclient/accessibleobject.h"

#include "accessibletree.h"

#include "tests_auto_modeltest_modeltest.h"

using namespace KAccessibleClient;

MainWindow::MainWindow(QWidget *parent)
    :KMainWindow(parent)
{
    ui.setupUi(this);

    KAccessibleClient::Registry *reg = new KAccessibleClient::Registry(this);
    reg->subscribeEventListeners(KAccessibleClient::Registry::Focus);
    connect(reg, SIGNAL(focusChanged(KAccessibleClient::AccessibleObject)), this, SLOT(focusChanged(KAccessibleClient::AccessibleObject)));

    AccessibleTree *model = new AccessibleTree(this);

    ui.treeView->setModel(model);
    model->setRegistry(reg);

    // The ultimate model verificaton helper :p
    new ModelTest(model, this);

    connect(ui.treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChanged(QModelIndex,QModelIndex)));
    connect(ui.action_Reset_tree, SIGNAL(triggered()), model, SLOT(resetModel()));
    ui.action_Reset_tree->setShortcut(QKeySequence(QKeySequence::Refresh));
}

void MainWindow::focusChanged(const KAccessibleClient::AccessibleObject &object)
{
    ui.label->setText(i18n("Focus changed to: %1 - %2 (%3)", object.name(), object.roleName(), object.role()));
}

void MainWindow::selectionChanged(const QModelIndex& current, const QModelIndex& old)
{
    QString text;

    if (current.isValid() && current.internalPointer()) {
        AccessibleObject acc = static_cast<AccessibleWrapper*>(current.internalPointer())->acc;
        text += acc.name() + " (" + acc.description() + ")\n";
        text += " role: " + acc.roleName();
        text += "\n childCount: " + QString::number(acc.childCount());
        text += "\n Checked: " + (acc.isChecked() ? QLatin1String("yes") : QLatin1String("no")) + '\n';

        foreach (const AccessibleObject &child, acc.children()) {
            text += "\nChild: " + child.name() + " (" + child.roleName() + ")";
        }
    }
    ui.plainTextEdit->setPlainText(text);
}


#include <mainwindow.moc>

