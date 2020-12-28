/*
    SPDX-FileCopyrightText: 2018 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>

#include "dumper.h"

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Accessibility Tree dumper"));

    QCommandLineParser p;
    p.addPositionalArgument(QStringLiteral("appname"), QStringLiteral("Application name"));
    QCommandLineOption states(QStringLiteral("states"));
    p.addOption(states);

    if (!p.parse(app.arguments())) {
        QTextStream out(stdout);
        out << QStringLiteral("Could not parse command line arguments.");
        out << p.helpText();
        exit(1);
    }

    Dumper d;
    if (p.isSet(states)) {
        d.showStates(true);
    }

    if (p.positionalArguments().size() == 1) {
        d.run(p.positionalArguments().at(0));
    } else {
        d.run(QString());
    }
}
