/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef QACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_P_H
#define QACCESSIBILITYCLIENT_ACCESSIBLEOBJECT_P_H

#include <QString>
#include <QSharedPointer>
#include <QAction>

namespace QAccessibleClient {

class RegistryPrivate;

class AccessibleObjectPrivate
{
public:
    AccessibleObjectPrivate(RegistryPrivate *reg, const QString &service_, const QString &path_);
    ~AccessibleObjectPrivate();

    RegistryPrivate *registryPrivate;
    QString service;
    QString path;

    bool defunct;
    mutable QVector< QSharedPointer<QAction> > actions;
    mutable bool actionsFetched;

    bool operator==(const AccessibleObjectPrivate &other) const;

    void setDefunct();

private:
    Q_DISABLE_COPY(AccessibleObjectPrivate)
};

}

#endif
