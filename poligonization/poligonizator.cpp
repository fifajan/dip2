/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * poligonizator.cpp is part of 3D Meta-Object-based Modelling System        *
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

#include <math.h>

#include <QDebug>

#include "fieldobject.h"
#include "grid.h"
#include "poligonizator.h"
#include "normalization.h"

using namespace Normalization;

Poligonizator::Poligonizator(const FieldObject *fieldObject)
            : fNormalMode(FLAT), fIsoLevel(2.0), fFieldObject(fieldObject),
            fGridCells(fFieldObject->grid()->data()->cellCount())
{
    recalculateGridCells();
    recalculateTriangles(true);
    fFlatNormalizedTrianglesPtr = QSharedPointer<QVector<TriangleN> >(
                new QVector<TriangleN>(0));
    fSmoothNormalizedTrianglesPtr = QSharedPointer<QVector<TriangleN> >(
                new QVector<TriangleN>(0));
}

void Poligonizator::setNormalMode(NormalMode normalMode)
{
    if (fNormalMode != normalMode)
    {
        fNormalMode = normalMode;
        recalculateNormalizedTriangles();
    }
}

QSharedPointer<const QVector<TriangleN> > Poligonizator::trianglesPtr() const
{
    return fNormalMode == FLAT ? fFlatNormalizedTrianglesPtr
                : fSmoothNormalizedTrianglesPtr;
}

void Poligonizator::recalculateGridCells()
{
    unsigned int xPos, yPos, zPos;

    const Grid *grid = fFieldObject->grid()->data();

    unsigned int xCellDim = grid->xDimention() - 1;
    unsigned int yCellDim = grid->yDimention() - 1;
    unsigned int zCellDim = grid->zDimention() - 1;

    fGridCells.resize(grid->cellCount());

    unsigned int i = 0;
    for (zPos = 0; zPos <  zCellDim; zPos++)
    {
        for (yPos = 0; yPos <  yCellDim; yPos++)
        {
            for (xPos = 0; xPos <  xCellDim; xPos++)
            {
                fGridCells[i] = GridCell(grid, xPos, yPos, zPos);
                i++;
            }
        }
    }
}

void Poligonizator::recalculateTrianglesInGridCells(
            bool performPointValuesRecalculation)
{
    int cellCount = fGridCells.count();
    for (int i = 0; i <  cellCount; i++)
    {
        fGridCells[i].setIsoLevel(fIsoLevel);
        fGridCells[i].recalculateTriangles(performPointValuesRecalculation);
    }
}

void Poligonizator::recalculateTriangles(bool gridDimentionsChanged,
            bool performPointValuesRecalculation)
{
    if (gridDimentionsChanged)
    {
        recalculateGridCells();
    }

    recalculateTrianglesInGridCells(performPointValuesRecalculation);
    recalculateNormalizedTriangles();
    qDebug() << "Performed tirangles recalculation";
}

void Poligonizator::recalculateNormalizedTriangles()
{
    if (fNormalMode == FLAT)
    {
        recalculateFlatNormalizedTriangles();
    }
    else
    {
        recalculateSmoothNormalizedTriangles();
    }
}

void Poligonizator::recalculateFlatNormalizedTriangles()
{
    int cellCount = fGridCells.count();

    QVector<TriangleN> *normalizedTriangles = new QVector<TriangleN>(0);
    // Reserving memory for estimated maximal number of trinagles per
    // field object
    normalizedTriangles->reserve(4 * cellCount);

    QVector<TriangleN> cellTriangles;
    int cellTriangleCount = 0;
    int triangleCount = 0;
    int i, j;
    for (i = 0; i < cellCount; i++)
    {
        cellTriangles = fGridCells[i].triangles();
        cellTriangleCount = cellTriangles.count();
        for (j = 0; j < cellTriangleCount; j++)
        {
            normalizedTriangles->append(cellTriangles[j]);
        }
        triangleCount += cellTriangleCount;
    }
    normalizedTriangles->squeeze();

    fFlatNormalizedTrianglesPtr =
                QSharedPointer<QVector<TriangleN> >(normalizedTriangles);

    // qDebug() << "TRIANGLES COUNT = " << triangleCount;
}

void Poligonizator::recalculateSmoothNormalizedTriangles()
{
    GridCell *cell = 0;

    QVector<TriangleN> cellTriangles(0);
    QVector<TriangleN> adjacentTriangles(0);

    cellTriangles.reserve(5);
    int cellTriangleCount = 0;

    TriangleN triangle;
    TriangleN normalizedTriangle;

    int cellCount = fGridCells.count();
    QVector<TriangleN> *normalizedTriangles = new QVector<TriangleN>(0);
    // Reserving memory for estimated maximal number of trinagles per
    // field object
    normalizedTriangles->reserve(4 * cellCount);

    int i, j, k;
    for (i = 0; i < cellCount; i++)
    {
        cell = &(fGridCells[i]);
        cellTriangles = cell->triangles();
        cellTriangleCount = cellTriangles.count();
        for (j = 0; j < cellTriangleCount; j++)
        {
            triangle = cellTriangles[j];
            for (k = 0; k < 3; k++)
            {
                adjacentTriangles =
                            adjacentTrianglesForVertex(triangle.p[k].p, cell);
                normalizedTriangle.p[k] =
                            normalizeVertex(triangle.p[k].p, adjacentTriangles);
            }
            normalizedTriangles->append(normalizedTriangle);
        }
    }
    normalizedTriangles->squeeze();

    fSmoothNormalizedTrianglesPtr =
                QSharedPointer<QVector<TriangleN> >(normalizedTriangles);

    //qDebug() << "TRIANGLES COUNT = " << triangleCount;
}

int Poligonizator::gridCellIndex(int xPos, int yPos, int zPos) const
{
    const Grid *grid = fFieldObject->grid()->data();
    int xCellDim = grid->xDimention() - 1;
    int yCellDim = grid->yDimention() - 1;
    int zCellDim = grid->zDimention() - 1;

    if (xPos < 0 || yPos < 0 || zPos < 0 ||
                xPos >= xCellDim || yPos >= yCellDim || zPos >= zCellDim)
    {
        return -1; // invalid cell position was given
    }
    else
    {
        return (xCellDim * yCellDim * zPos) + (xCellDim * yPos) + xPos;
    }
}

QVector<TriangleN> Poligonizator::adjacentTrianglesForVertex(const Point &point,
            const GridCell *cell)
{
    QVector<TriangleN> resTriangles(0);
    // Reserving memory for estimated maximal number of neighboring trinagles
    resTriangles.reserve(27 * 4);

    unsigned int xOffset = cell->xPos();
    unsigned int yOffset = cell->yPos();
    unsigned int zOffset= cell->zPos();

    int cellTriangleCount = 0;
    QVector<TriangleN> cellTriangles(0);
    cellTriangles.reserve(5);

    int index = 0;
    int xPos, yPos, zPos, i;
    for(zPos = -1; zPos < 2; zPos++)
    {
        for(yPos = -1; yPos < 2; yPos++)
        {
            for(xPos = -1; xPos < 2; xPos++)
            {
                index = gridCellIndex(xOffset + xPos, yOffset + yPos,
                            zOffset + zPos);
                if (index >= 0 && fGridCells[index].hasTriangles())
                {
                    cellTriangles = fGridCells[index].triangles();
                    cellTriangleCount = cellTriangles.count();
                    for (i = 0; i < cellTriangleCount; i++)
                    {
                        if (isVertex(point, cellTriangles[i]))
                        {
                            resTriangles += cellTriangles[i];
                        }
                    }
                }
            }
        }
    }
    resTriangles.squeeze();
    // qDebug() << "adjacent triangles COUNT = " << resTriangles.count();
    return resTriangles;
}
