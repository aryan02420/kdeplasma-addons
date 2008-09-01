/*
 * Copyright (C) 2007,2008 Petri Damstén <damu@iki.fi>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SIMPLEUNIT_H
#define SIMPLEUNIT_H

#include "unit.h"
#include <QHash>

class SimpleUnit : public UnitCategory
{
public:
    SimpleUnit(QObject* parent = 0);

    virtual QStringList units();
    virtual bool hasUnit(const QString &unit);
    virtual Value convert(const Value& value, const QString& to);

protected:
    QHash<QString, QVariant> m_units;
    QString m_default;

    virtual double toDouble(const QString &unit, QString *unitString);
};

#endif
