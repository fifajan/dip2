/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * fieldobject.h is part of 3D Meta-Object-based Modelling System            *
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

#ifndef FIELDOBJECT_H
#define FIELDOBJECT_H

#include <QSharedPointer>

#include "grid.h"
#include "space_types.h"

class QBuffer;
class QByteArray;

// Represents the basic "field-object" type, Inherited by field and meta-object.
// Field-object (so inherited classes too) holds 2 separate grids of same
// dimention, it is done to have ability to hold field-object's previous
// value. So when field-object is changed we don't have to recalculate the
// whole field as we can do much faster with "swap grid" approach.
class FieldObject
{
public:
    FieldObject(const FieldObject &copyee);
    FieldObject(unsigned int xDim, unsigned int yDim, unsigned int zDim,
                QBuffer *xmlData = 0);

    virtual QByteArray XMLRepresentation() = 0;

    virtual ~FieldObject() {}
    virtual float valueAtPoint(const Point& p) = 0;
    virtual void useExternalGrid(const FieldObject *fieldObject);
    virtual void swapGrid();

    virtual void addFieldObject(const FieldObject *fieldObject);
    virtual void subtractFieldObject(const FieldObject *fieldObject);

    void setGridSidesDimention(unsigned int gridDim);
    void setGridXDimention(unsigned int gridXDim);
    void setGridYDimention(unsigned int gridYDim);
    void setGridZDimention(unsigned int gridZDim);

    virtual void recalculate();

    virtual const QSharedPointer<Grid>* grid() const;

protected:

    void initGrids(unsigned int xDim, unsigned int yDim, unsigned int zDim);

private: // data
    unsigned int fCurrentGridId;
    QSharedPointer<Grid> fGrid[2];
    bool fUsingExternalGrid;
};

#endif // FIELDOBJECT_H
