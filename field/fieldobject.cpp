/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * fieldobject.cpp is part of 3D Meta-Object-based Modelling System          *
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

#include <QBuffer>
#include <QByteArray>
#include <QDebug>

#include "fieldobject.h"

FieldObject::FieldObject(const FieldObject &copyee)
{
    fCurrentGridId = copyee.fCurrentGridId;
    fGrid[0] = copyee.fGrid[0];
    fGrid[1] = copyee.fGrid[1];
    fUsingExternalGrid = copyee.fUsingExternalGrid;
}

FieldObject::FieldObject(unsigned int xDim, unsigned int yDim,
            unsigned int zDim, QBuffer *xmlData)
            : fCurrentGridId(0), fUsingExternalGrid(0)
{
    initGrids(xDim, yDim, zDim);
}
/*
FieldObject::FieldObject(QBuffer *xmlData) : fCurrentGridId(0),
            fUsingExternalGrid(0)
{
    initGrids(kDim, kDim, kDim);
}
*/
void FieldObject::recalculate()
{
    grid()->data()->fillWithFieldObject(this);
}

void FieldObject::useExternalGrid(const FieldObject *fieldObject)
{
    //swapGrid();
    fGrid[fCurrentGridId] = fieldObject->fGrid[fCurrentGridId];
    fUsingExternalGrid = true;
}

void FieldObject::swapGrid()
{
    /*
    if (fUsingExternalGrid)
    {
        fGrid[fCurrentGridId] = QSharedPointer<Grid>(new Grid());
        fUsingExternalGrid = false;
    }
    */
    fCurrentGridId = (fCurrentGridId == 0) ? 1 : 0;
}

void FieldObject::addFieldObject(const FieldObject *fieldObject)
{
    Grid *thisGrid = grid()->data();
    Grid *grid = fieldObject->grid()->data();

    thisGrid->addGrid(grid);
}

void FieldObject::subtractFieldObject(const FieldObject *fieldObject)
{
    Grid *thisGrid = grid()->data();
    Grid *grid = fieldObject->grid()->data();

    thisGrid->subtractGrid(grid);
}

void FieldObject::setGridSidesDimention(unsigned int gridDim)
{
    grid()->data()->setSidesDimention(gridDim);
    swapGrid();
    grid()->data()->setSidesDimention(gridDim);
    recalculate();

}

void FieldObject::setGridXDimention(unsigned int gridXDim)
{
    grid()->data()->setXDimention(gridXDim);
    swapGrid();
    grid()->data()->setXDimention(gridXDim);
    recalculate();
}

void FieldObject::setGridYDimention(unsigned int gridYDim)
{
    grid()->data()->setYDimention(gridYDim);
    swapGrid();
    grid()->data()->setYDimention(gridYDim);
    recalculate();
}

void FieldObject::setGridZDimention(unsigned int gridZDim)
{
    grid()->data()->setZDimention(gridZDim);
    swapGrid();
    grid()->data()->setZDimention(gridZDim);
    recalculate();
}

void FieldObject::initGrids(unsigned int xDim, unsigned int yDim,
            unsigned int zDim)
{
    fGrid[0] = QSharedPointer<Grid>(new Grid(xDim, yDim, zDim));
    fGrid[1] = QSharedPointer<Grid>(new Grid(xDim, yDim, zDim));
}

const QSharedPointer<Grid>* FieldObject::grid() const
{
    return fGrid + fCurrentGridId;
}
