/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QApplication>

#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName(QLatin1String("Randamizer"));

    MainWindow window;
    window.show();

    return app.exec();
}
