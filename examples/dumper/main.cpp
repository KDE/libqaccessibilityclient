/*
    Copyright 2018 Frederik Gladhorn <gladhorn@kde.org>

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
