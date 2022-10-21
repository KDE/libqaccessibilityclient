/*
    SPDX-FileCopyrightText: 2012 Frederik Gladhorn <gladhorn@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <QApplication>
#include <QPushButton>
#include <QBoxLayout>
#include <QTimer>

class SimpleWidgetApp : public QWidget
{
    Q_OBJECT
public:
    SimpleWidgetApp()
    {
        QPushButton *button = new QPushButton(this);
        button->setText(QStringLiteral("Button 1"));
        QPushButton *button2 = new QPushButton(this);
        button2->setText(QStringLiteral("Button 2"));

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
    app.setApplicationName(QStringLiteral("LibKdeAccessibilityClient Simple Widget App"));

    SimpleWidgetApp simple;
    simple.show();
    return app.exec();
}

#include "simplewidgetapp.moc"
