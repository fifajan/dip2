/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * gridcell.h is part of 3D Meta-Object-based Modelling System               *
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

#ifndef GRIDCELL_H
#define GRIDCELL_H

#include <QVector>

#include "space_types.h"

class Grid;

// Represent cubic grid cell with potential value in each cube vetex.
// Grid cell can build triangular isosurface (currently 0-5 triangles)
// of field it contains inside the cell by specifying an isosurface level.
class GridCell
{
public:
    GridCell();
    GridCell(const Grid *gridPtr, unsigned int xPos, unsigned int yPos,
                unsigned int zPos);
    GridCell(const GridCell &copyee);

    inline unsigned int xPos() const { return fXPos; }
    inline unsigned int yPos() const { return fYPos; }
    inline unsigned int zPos() const { return fZPos; }
    inline bool hasTriangles() const { return fHasTriangles; }
    QVector<TriangleN> triangles() const { return fTriangles; }

    void setIsoLevel(float isoLevel) { fIsoLevel = isoLevel; }
    void recalculateTriangles(bool performPointValuesRecalculation = true);

protected:
    void recalculatePointValues();
    void recalculatePoints();

    QVector<Triangle> trianglesMarchingCubes(float isoLevel);
    QVector<Triangle> trianglesMarchingTetrahedrons(float isoLevel);

    QVector<Triangle> tetrahedronTriangles(float isoLevel, int v0, int v1,
                int v2, int v3);

private:
    const Grid *fGridPtr;

    unsigned int fXPos;
    unsigned int fYPos;
    unsigned int fZPos;

    float fIsoLevel;

    bool fHasTriangles;
    QVector<TriangleN> fTriangles;

    Point fPoints[8];
    float fPointValues[8]; // field strength in corresponding vertex.
};

#endif // GRIDCELL_H
