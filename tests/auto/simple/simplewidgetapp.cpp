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

#include <qapplication.h>
#include <qpushbutton.h>
#include <qboxlayout.h>
#include <qtimer.h>

class SimpleWidgetApp : public QWidget
{
    Q_OBJECT
public:
    SimpleWidgetApp()
    {
        QPushButton *button = new QPushButton(this);
        button->setText("Button 1");
        QPushButton *button2 = new QPushButton(this);
        button2->setText("Button 2");

        this->setGeometry(0,0,200,100);

        button->setText(QLatin1String("Button 1"));
        button->setGeometry(10,10,100,20);
        button2->setText(QLatin1String("Button 2"));
        button2->setGeometry(10,40,100,20);

        button->setFocus();
        QTimer timer;
        timer.singleShot(100, button2, SLOT(setFocus()));
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("LibKdeAccessibilityClient Simple Widget App");

    SimpleWidgetApp simple;
    simple.show();
    return app.exec();
}

#include "simplewidgetapp.moc"
