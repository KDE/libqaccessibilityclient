/*
    SPDX-FileCopyrightText: 2018 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "qaccessibilityclient/accessibleobject.h"
#include "qaccessibilityclient/registry.h"

#ifndef DUMPER_H
#define DUMPER_H

class Dumper : public QObject
{
    Q_OBJECT
public:
    explicit Dumper(QObject *parent = nullptr);
    void run(const QString &appname) const;
    void printChild(const QAccessibleClient::AccessibleObject &object, int indent = 0) const;
    void showStates(bool show) { m_showStates = show; }

private:
    bool m_showStates = false;
};

#endif
