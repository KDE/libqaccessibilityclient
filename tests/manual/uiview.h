/*
    Copyright 2012 Sebastian Sauer <sebastian.sauer@kdab.com>

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

#ifndef UIVIEW_H
#define UIVIEW_H

#include "qaccessibilityclient/accessibleobject.h"
#include "qaccessibilityclient/registry.h"

#include <QImage>
#include <QPixmap>
#include <QRect>
#include <QScrollArea>

namespace QAccessibleClient {
    class AccessibleObject;
}

class UiView;

class UiWidget : public QWidget
{
public:
    UiWidget(UiView *view);
    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);
    virtual QSize sizeHint() const;
protected:
    QImage *m_image;
    QPixmap m_screen;
    QRect m_bounds;
    QAccessibleClient::AccessibleObject m_object;
    QMap<QAccessibleClient::AccessibleObject::Role, const char*> m_roleColors;

    virtual void paintEvent(QPaintEvent *event);
private:
    QPixmap grabScreen() const;
    QRect bounds(const QAccessibleClient::AccessibleObject &acc) const;
    void drawObject(QPainter *painter, const QAccessibleClient::AccessibleObject &acc, int depth = 0);
};

class UiView :public QScrollArea
{
    Q_OBJECT
public:
    UiView(QWidget *parent = 0);
    ~UiView();

    void setAccessibleObject(const QAccessibleClient::AccessibleObject &acc);

private:
    UiWidget *m_uiWidget;

    QWidget* createWidget(const QAccessibleClient::AccessibleObject &acc, QRect &bounds);
};

#endif
