/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * gridcell.cpp is part of 3D Meta-Object-based Modelling System             *
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

#include "grid.h"
#include "gridcell.h"
#include "marchingcubes_tables.h"
#include "normalization.h"

using namespace Normalization;

namespace MarchingCubes
{
    Point interpolateCrossPoint(float isoLevel, const Point &p1,
                const Point &p2, float p1Val, float p2Val)
    {
        Point p;

        if (equalFloat(isoLevel, p1Val))
        {
            p = p1;
        }
        else if (equalFloat(isoLevel, p2Val))
        {
            p = p2;
        }
        else if (equalFloat(p1Val, p2Val))
        {
            p = p1;
        }
        else
        {
            double mu = (isoLevel - p1Val) / (p2Val - p1Val);

            p.x = p1.x + mu * (p2.x - p1.x);
            p.y = p1.y + mu * (p2.y - p1.y);
            p.z = p1.z + mu * (p2.z - p1.z);
        }

        // NaN checks
        if (p.x != p.x) p.x = 0.0;
        if (p.y != p.y) p.y = 0.0;
        if (p.z != p.z) p.z = 0.0;

        return(p);
    }
}

using namespace MarchingCubes;

GridCell::GridCell() : fGridPtr(0), fXPos(0), fYPos(0), fZPos(0),
            fIsoLevel(2.0), fHasTriangles(false),
            fTriangles(QVector<TriangleN>(0))
{
    for (int i = 0; i < 8; i++)
    {
        Point p = { 0.0, 0.0, 0.0 };
        fPoints[i] = p;
        fPointValues[i] = 0.0;
    }
}

GridCell::GridCell(const Grid *gridPtr,
            unsigned int xPos, unsigned int yPos, unsigned int zPos)
            : fGridPtr(gridPtr), fXPos(xPos), fYPos(yPos), fZPos(zPos),
            fIsoLevel(2.0), fHasTriangles(true),
            fTriangles(QVector<TriangleN>(0))
{
    recalculatePoints();
    recalculatePointValues();
}

GridCell::GridCell(const GridCell &copyee)
{
    fGridPtr = copyee.fGridPtr;

    fXPos = copyee.fXPos;
    fYPos = copyee.fYPos;
    fZPos = copyee.fZPos;

    fIsoLevel = copyee.fIsoLevel;

    fHasTriangles = copyee.fHasTriangles;
    fTriangles = copyee.fTriangles;

    for (int i = 0; i < 8; i++)
    {
        fPoints[i] = copyee.fPoints[i];
        fPointValues[i] = copyee.fPointValues[i];
    }
}

void GridCell::recalculateTriangles(bool performPointValuesRecalculation)
{
    if (performPointValuesRecalculation)
    {
        recalculatePointValues();
    }

    QVector<Triangle> unnormalizedTriangles(trianglesMarchingCubes(fIsoLevel));
    int triangleCount = unnormalizedTriangles.count();

    fTriangles.clear();
    if (triangleCount < 1)
    {
        fHasTriangles = false;
    }
    else
    {
        fHasTriangles = true;
        fTriangles.resize(triangleCount);
        // calculate flat normal for each triangle
        for (int i = 0; i < triangleCount; i++)
        {
            fTriangles[i] = normalizeTriangle(unnormalizedTriangles[i]);
        }
    }
}

/*
    Given a grid cell and an isoLevel, calculate the triangular facets
    required to represent the isosurface through the cell. Return the
    number of triangular facets, the array "resTriangles" will be loaded up
    with the vertices at most 5 triangular facets. 0 will be returned if the
    grid cell is either totally above of totally below the isoLevel.
*/
QVector<Triangle> GridCell::trianglesMarchingCubes(float isoLevel)
{
    QVector<Triangle> resTriangles(6);

    int i = 0;
    int resTrianglesCount = 0;
    int cubeIndex = 0;
    Point vertexList[12];

    /*
        Determine the index into the edge table which
        tells us which vertices are inside of the surface
    */
    if (fPointValues[0] < isoLevel) cubeIndex |= 1;
    if (fPointValues[1] < isoLevel) cubeIndex |= 2;
    if (fPointValues[2] < isoLevel) cubeIndex |= 4;
    if (fPointValues[3] < isoLevel) cubeIndex |= 8;
    if (fPointValues[4] < isoLevel) cubeIndex |= 16;
    if (fPointValues[5] < isoLevel) cubeIndex |= 32;
    if (fPointValues[6] < isoLevel) cubeIndex |= 64;
    if (fPointValues[7] < isoLevel) cubeIndex |= 128;

    /* Cube is entirely in/out of the surface */
    if (kEdgeTable[cubeIndex] == 0)
    {
        return QVector<Triangle>();
    }

    /* Find the vertices where the surface intersects the cube */
    if (kEdgeTable[cubeIndex] & 1)
    {
        vertexList[0] = interpolateCrossPoint(isoLevel, fPoints[0],
                    fPoints[1], fPointValues[0], fPointValues[1]);
    }
    if (kEdgeTable[cubeIndex] & 2)
    {
        vertexList[1] = interpolateCrossPoint(isoLevel, fPoints[1],
                    fPoints[2], fPointValues[1], fPointValues[2]);
    }
    if (kEdgeTable[cubeIndex] & 4)
    {
        vertexList[2] = interpolateCrossPoint(isoLevel, fPoints[2],
                    fPoints[3], fPointValues[2], fPointValues[3]);
    }
    if (kEdgeTable[cubeIndex] & 8)
    {
        vertexList[3] = interpolateCrossPoint(isoLevel, fPoints[3],
                    fPoints[0], fPointValues[3], fPointValues[0]);
    }
    if (kEdgeTable[cubeIndex] & 16)
    {
        vertexList[4] = interpolateCrossPoint(isoLevel, fPoints[4],
                    fPoints[5], fPointValues[4], fPointValues[5]);
    }
    if (kEdgeTable[cubeIndex] & 32)
    {
        vertexList[5] = interpolateCrossPoint(isoLevel, fPoints[5],
                    fPoints[6], fPointValues[5], fPointValues[6]);
    }
    if (kEdgeTable[cubeIndex] & 64)
    {
        vertexList[6] = interpolateCrossPoint(isoLevel, fPoints[6],
                    fPoints[7], fPointValues[6], fPointValues[7]);
    }
    if (kEdgeTable[cubeIndex] & 128)
    {
        vertexList[7] = interpolateCrossPoint(isoLevel, fPoints[7],
                    fPoints[4], fPointValues[7], fPointValues[4]);
    }
    if (kEdgeTable[cubeIndex] & 256)
    {
        vertexList[8] = interpolateCrossPoint(isoLevel, fPoints[0],
                    fPoints[4], fPointValues[0], fPointValues[4]);
    }
    if (kEdgeTable[cubeIndex] & 512)
    {
        vertexList[9] = interpolateCrossPoint(isoLevel, fPoints[1],
                    fPoints[5], fPointValues[1], fPointValues[5]);
    }
    if (kEdgeTable[cubeIndex] & 1024)
    {
        vertexList[10] = interpolateCrossPoint(isoLevel, fPoints[2],
                    fPoints[6], fPointValues[2], fPointValues[6]);
    }
    if (kEdgeTable[cubeIndex] & 2048)
    {
        vertexList[11] = interpolateCrossPoint(isoLevel, fPoints[3],
                    fPoints[7], fPointValues[3], fPointValues[7]);
    }

    /* Create the triangle */
    resTrianglesCount = 0;
    for (i = 0; kTriTable[cubeIndex][i] != -1; i += 3)
    {
        resTriangles[resTrianglesCount].p[0] =
                    vertexList[kTriTable[cubeIndex][i]];
        resTriangles[resTrianglesCount].p[1] =
                    vertexList[kTriTable[cubeIndex][i + 1]];
        resTriangles[resTrianglesCount].p[2] =
                    vertexList[kTriTable[cubeIndex][i + 2]];
        resTrianglesCount++;
    }

    resTriangles.resize(resTrianglesCount);

    return(resTriangles);
}

QVector<Triangle> GridCell::trianglesMarchingTetrahedrons(float isoLevel)
{
    QVector<Triangle> resTriangles;
    resTriangles.reserve(20);

    resTriangles += tetrahedronTriangles(isoLevel, 0,2,3,7);
    resTriangles += tetrahedronTriangles(isoLevel, 0,2,6,7);
    resTriangles += tetrahedronTriangles(isoLevel, 0,4,6,7);
    resTriangles += tetrahedronTriangles(isoLevel, 0,6,1,2);
    resTriangles += tetrahedronTriangles(isoLevel, 0,6,1,4);
    resTriangles += tetrahedronTriangles(isoLevel, 5,6,1,4);

    return resTriangles;
}

void GridCell::recalculatePointValues()
{
    unsigned int pIndex[8];
    pIndex[0] = fGridPtr->pointIndex(fXPos, fYPos, fZPos);
    pIndex[1] = fGridPtr->pointIndex(fXPos + 1, fYPos, fZPos);
    pIndex[2] = fGridPtr->pointIndex(fXPos + 1, fYPos + 1, fZPos);
    pIndex[3] = fGridPtr->pointIndex(fXPos, fYPos + 1, fZPos);
    pIndex[4] = fGridPtr->pointIndex(fXPos, fYPos, fZPos + 1);
    pIndex[5] = fGridPtr->pointIndex(fXPos + 1, fYPos, fZPos + 1);
    pIndex[6] = fGridPtr->pointIndex(fXPos + 1, fYPos + 1, fZPos + 1);
    pIndex[7] = fGridPtr->pointIndex(fXPos, fYPos + 1, fZPos + 1);

    const float *gridPointValues = fGridPtr->pointValues();

    for (unsigned int i = 0; i < 8; i++)
    {
        fPointValues[i] = gridPointValues[pIndex[i]];
    }

    fTriangles.clear();
    fHasTriangles = true;
}

void GridCell::recalculatePoints()
{
    fPoints[0].x = fGridPtr->xCoord(fXPos);
    fPoints[0].y = fGridPtr->yCoord(fYPos);
    fPoints[0].z = fGridPtr->zCoord(fZPos);

    fPoints[1].x = fGridPtr->xCoord(fXPos + 1);
    fPoints[1].y = fGridPtr->yCoord(fYPos);
    fPoints[1].z = fGridPtr->zCoord(fZPos);

    fPoints[2].x = fGridPtr->xCoord(fXPos + 1);
    fPoints[2].y = fGridPtr->yCoord(fYPos + 1);
    fPoints[2].z = fGridPtr->zCoord(fZPos);

    fPoints[3].x = fGridPtr->xCoord(fXPos);
    fPoints[3].y = fGridPtr->yCoord(fYPos + 1);
    fPoints[3].z = fGridPtr->zCoord(fZPos);

    fPoints[4].x = fGridPtr->xCoord(fXPos);
    fPoints[4].y = fGridPtr->yCoord(fYPos);
    fPoints[4].z = fGridPtr->zCoord(fZPos + 1);

    fPoints[5].x = fGridPtr->xCoord(fXPos + 1);
    fPoints[5].y = fGridPtr->yCoord(fYPos);
    fPoints[5].z = fGridPtr->zCoord(fZPos + 1);

    fPoints[6].x = fGridPtr->xCoord(fXPos + 1);
    fPoints[6].y = fGridPtr->yCoord(fYPos + 1);
    fPoints[6].z = fGridPtr->zCoord(fZPos + 1);

    fPoints[7].x = fGridPtr->xCoord(fXPos);
    fPoints[7].y = fGridPtr->yCoord(fYPos + 1);
    fPoints[7].z = fGridPtr->zCoord(fZPos + 1);
}

/*
    Polygonise a tetrahedron given its vertices within a cube. This is an
    alternative algorithm to polygonisegrid. It results in a smoother surface
    but more triangular facets. It's main purpose is still to polygonise
    a gridded dataset and would normally be called 6 times, one for each
    tetrahedron making up the grid cell.
*/
QVector<Triangle> GridCell::tetrahedronTriangles(float isoLevel,
            int v0, int v1, int v2, int v3)
{
    QVector<Triangle> resTriangles(5);

    int resTrianglesCount = 0;
    int triangleIndex = 0;

    /*
        Determine which of the 16 cases we have given which vertices
        are above or below the isosurface
    */
    triangleIndex = 0;
    if (fPointValues[v0] < isoLevel) triangleIndex |= 1;
    if (fPointValues[v1] < isoLevel) triangleIndex |= 2;
    if (fPointValues[v2] < isoLevel) triangleIndex |= 4;
    if (fPointValues[v3] < isoLevel) triangleIndex |= 8;

    /* Form the vertices of the triangles for each case */
    switch (triangleIndex)
    {
        case 0x00:
        case 0x0F:
        {
            break;
        }
        case 0x0E: ///
        case 0x01:
        {
            resTriangles[0].p[0] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v1],
                        fPointValues[v0], fPointValues[v1]);
            resTriangles[0].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v2],
                        fPointValues[v0], fPointValues[v2]);
            resTriangles[0].p[2] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v3],
                        fPointValues[v0], fPointValues[v3]);
            resTrianglesCount++;
            break;
        }
        case 0x0D: ///
        case 0x02:
        {
            resTriangles[0].p[0] = interpolateCrossPoint(isoLevel,
                        fPoints[v1], fPoints[v0],
                        fPointValues[v1], fPointValues[v0]);
            resTriangles[0].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v1], fPoints[v3],
                        fPointValues[v1], fPointValues[v3]);
            resTriangles[0].p[2] = interpolateCrossPoint(isoLevel,
                        fPoints[v1], fPoints[v2],
                        fPointValues[v1], fPointValues[v2]);
            resTrianglesCount++;
            break;
        }
        case 0x0C: ///
        case 0x03:
        {
            resTriangles[0].p[0] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v3],
                        fPointValues[v0], fPointValues[v3]);
            resTriangles[0].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v2],
                        fPointValues[v0], fPointValues[v2]);
            resTriangles[0].p[2] = interpolateCrossPoint(isoLevel,
                        fPoints[v1], fPoints[v3],
                        fPointValues[v1], fPointValues[v3]);
            resTrianglesCount++;

            resTriangles[1].p[0] = resTriangles[0].p[2];
            resTriangles[1].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v1], fPoints[v2],
                        fPointValues[v1], fPointValues[v2]);
            resTriangles[1].p[2] = resTriangles[0].p[1];
            resTrianglesCount++;
            break;
        }
        case 0x0B: ///
        case 0x04:
        {
            resTriangles[0].p[0] = interpolateCrossPoint(isoLevel,
                        fPoints[v2], fPoints[v0],
                        fPointValues[v2], fPointValues[v0]);
            resTriangles[0].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v2], fPoints[v1],
                        fPointValues[v2], fPointValues[v1]);
            resTriangles[0].p[2] = interpolateCrossPoint(isoLevel,
                        fPoints[v2], fPoints[v3],
                        fPointValues[v2], fPointValues[v3]);
            resTrianglesCount++;
            break;
        }
        case 0x0A: ///
        case 0x05:
        {
            resTriangles[0].p[0] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v1],
                        fPointValues[v0], fPointValues[v1]);
            resTriangles[0].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v2], fPoints[v3],
                        fPointValues[v2], fPointValues[v3]);
            resTriangles[0].p[2] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v3],
                        fPointValues[v0], fPointValues[v3]);
            resTrianglesCount++;

            resTriangles[1].p[0] = resTriangles[0].p[0];
            resTriangles[1].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v1], fPoints[v2],
                        fPointValues[v1], fPointValues[v2]);
            resTriangles[1].p[2] = resTriangles[0].p[1];
            resTrianglesCount++;
            break;
        }
        case 0x09: ///
        case 0x06:
        {
            resTriangles[0].p[0] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v1],
                        fPointValues[v0], fPointValues[v1]);
            resTriangles[0].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v1], fPoints[v3],
                        fPointValues[v1], fPointValues[v3]);
            resTriangles[0].p[2] = interpolateCrossPoint(isoLevel,
                        fPoints[v2], fPoints[v3],
                        fPointValues[v2], fPointValues[v3]);
            resTrianglesCount++;

            resTriangles[1].p[0] = resTriangles[0].p[0];
            resTriangles[1].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v0], fPoints[v2],
                        fPointValues[v0], fPointValues[v2]);
            resTriangles[1].p[2] = resTriangles[0].p[2];
            resTrianglesCount++;
            break;
        }
        case 0x07: ///
        case 0x08:
        {
            resTriangles[0].p[0] = interpolateCrossPoint(isoLevel,
                        fPoints[v3], fPoints[v0],
                        fPointValues[v3], fPointValues[v0]);
            resTriangles[0].p[1] = interpolateCrossPoint(isoLevel,
                        fPoints[v3], fPoints[v2],
                        fPointValues[v3], fPointValues[v2]);
            resTriangles[0].p[2] = interpolateCrossPoint(isoLevel,
                        fPoints[v3], fPoints[v1],
                        fPointValues[v3], fPointValues[v1]);
            resTrianglesCount++;
            break;
        }
    }

    resTriangles.resize(resTrianglesCount);

    return(resTriangles);
}
