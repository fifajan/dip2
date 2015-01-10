/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * field.h is part of 3D Meta-Object-based Modelling System                  *
 *                                                                           *
 * Copyright (c) 2010 Alexey Ivchenko aka fifajan <fifajan@ukr.net>          *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.              *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef FIELD_H
#define FIELD_H

#include <QSharedPointer>

#include "metaobject.h"
#include "fieldobject.h"

class QBuffer;
class QByteArray;

// Represents scalar field defined as a set of meta-objects. Field value is:
//                     n
// F_field(x, y, z) = SUM(F_mo(x, y, z)).
//                     1
class Field : public FieldObject
{
public:
    Field(const Field &copyee);
    Field(unsigned int xDim, unsigned int yDim, unsigned int zDim,
                QBuffer *xmlData = 0);

    virtual QByteArray XMLRepresentation();

    virtual float valueAtPoint(const Point& p);

    void updateMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr);
    void useGridOfMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr);

    void setGridSidesDimention(unsigned int gridDim);
    void setGridXDimention(unsigned int gridXDim);
    void setGridYDimention(unsigned int gridYDim);
    void setGridZDimention(unsigned int gridZDim);

    void addMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr);
    void removeMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr);

    QList<QSharedPointer<MetaObject> > metaObjects() { return fMetaObjects; }

protected:
    bool initWithXML(QBuffer *xmlData);

private:
    float fIsoLevel;
    QList<QSharedPointer<MetaObject> > fMetaObjects;
};

#endif // FIELD_H
