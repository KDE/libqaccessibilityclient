/*
    Copyright 2011 Peter Grasch <grasch@simon-listens.org>
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


#ifndef ATSPIACTION_H
#define ATSPIACTION_H
#include <QString>

class AtspiAction
{
private:
  QString m_name;
  QString m_description;
  QString m_comment;
public:
  AtspiAction (const QString& name, const QString& description, const QString& comment) :
    m_name(name), m_description(description), m_comment(comment)
  {}
  
  QString name() { return m_name; }
  QString description() { return m_description; }
  QString comment() { return m_comment; }
};

#endif // ATSPIACTION_H
