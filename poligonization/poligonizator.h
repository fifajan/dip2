/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * poligonizator.h is part of 3D Meta-Object-based Modelling System          *
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

#ifndef POLIGONIZATOR_H
#define POLIGONIZATOR_H

#include <QVector>
#include <QSharedPointer>

#include "gridcell.h"
#include "space_types.h"

typedef enum
{
    FLAT = 1,
    SMOOTH
} NormalMode;

class FieldObject;

// Represents tool for poligonization (giving triangular isosurface
// representation) of any field object. Poligonozator's output are normalized
// triangles. Flat and smooth triangles normalization are supported.
class Poligonizator
{
public:
    Poligonizator(const FieldObject *fieldObject);

    inline float isoLevel() const { return fIsoLevel; }
    void setIsoLevel(float isoLevel) { fIsoLevel = isoLevel; }

    NormalMode normalMode() const { return fNormalMode; }
    void setNormalMode(NormalMode normalMode);

    QSharedPointer<const QVector<TriangleN> > trianglesPtr() const;
    void recalculateTriangles(bool gridDimentionsChanged = false,
                bool performPointValuesRecalculation = true);

protected:
    // Performs complete grid repartitioning into cells and computes field
    // values in each cell's vertex
    void recalculateGridCells();

    void recalculateTrianglesInGridCells(
                bool performPointValuesRecalculation = true);

    void recalculateNormalizedTriangles();
    void recalculateFlatNormalizedTriangles();
    void recalculateSmoothNormalizedTriangles();

    // Returns -1 if invalid cell position is given
    int gridCellIndex(int xPos, int yPos, int zPos) const;
    QVector<TriangleN> adjacentTrianglesForVertex(const Point &point,
                const GridCell *cell);

private:
    NormalMode fNormalMode;
    float fIsoLevel;
    const FieldObject *fFieldObject;
    QVector<GridCell> fGridCells;
    QSharedPointer<QVector<TriangleN> > fFlatNormalizedTrianglesPtr;
    QSharedPointer<QVector<TriangleN> > fSmoothNormalizedTrianglesPtr;
};

#endif // POLIGONIZATOR_H
