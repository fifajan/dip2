/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * normalization.cpp is part of 3D Meta-Object-based Modelling System        *
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

#include "normalization.h"

const float kPrecision = 0.0001;

Point Normalization::vector(const Point &a, const Point &b)
{
    Point v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    v.z = a.z - b.z;

    return v;
}

Point Normalization::normal(const Point &v1, const Point &v2)
{
    Point n;
    n.x = v1.y * v2.z - v1.z * v2.y;
    n.y = v1.z * v2.x - v1.x * v2.z;
    n.z = v1.x * v2.y - v1.y * v2.x;

    return n;
}

Point Normalization::normalizeVector(const Point &v)
{
    Point nv;
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    nv.x = v.x / length;
    nv.y = v.y / length;
    nv.z = v.z / length;
    return nv;
}

TriangleN Normalization::normalizeTriangle(const Triangle &triangle)
{
    TriangleN resTriangle;
    resTriangle.p[0].p = triangle.p[0];
    resTriangle.p[1].p = triangle.p[1];
    resTriangle.p[2].p = triangle.p[2];

    resTriangle.p[0].n = normal(vector(triangle.p[0], triangle.p[1]),
                vector(triangle.p[0], triangle.p[2]));

    resTriangle.p[1].n = resTriangle.p[0].n;
    resTriangle.p[2].n = resTriangle.p[0].n;

    return resTriangle;
}

PointN Normalization::normalizeVertex(const Point &v,
            const QVector<Triangle> &adjacentTriangles)
{
    PointN resVertex;
    Point n = { 0.0, 0.0, 0.0 };
    resVertex.p = v;
    resVertex.n = n;

    TriangleN adjacentTriangle;
    int adjacentTriangleCount = adjacentTriangles.count();
    for (int i = 0; i < adjacentTriangleCount; i++)
    {
        adjacentTriangle = normalizeTriangle(adjacentTriangles[i]);
        resVertex.n.x += adjacentTriangle.p[0].n.x;
        resVertex.n.y += adjacentTriangle.p[0].n.y;
        resVertex.n.z += adjacentTriangle.p[0].n.z;
    }

    resVertex.n.x /= adjacentTriangleCount;
    resVertex.n.y /= adjacentTriangleCount;
    resVertex.n.z /= adjacentTriangleCount;

    return resVertex;
}

PointN Normalization::normalizeVertex(const Point &v,
            const QVector<TriangleN> &adjacentTriangles)
{
    PointN resVertex;
    Point n = { 0.0, 0.0, 0.0 };
    resVertex.p = v;
    resVertex.n = n;

    TriangleN adjacentTriangle;
    int adjacentTriangleCount = adjacentTriangles.count();
    for (int i = 0; i < adjacentTriangleCount; i++)
    {
        adjacentTriangle = adjacentTriangles[i];
        resVertex.n.x += adjacentTriangle.p[0].n.x;
        resVertex.n.y += adjacentTriangle.p[0].n.y;
        resVertex.n.z += adjacentTriangle.p[0].n.z;
    }

    resVertex.n.x /= adjacentTriangleCount;
    resVertex.n.y /= adjacentTriangleCount;
    resVertex.n.z /= adjacentTriangleCount;

    return resVertex;
}

bool Normalization::equalFloat(float a, float b)
{
    return fabs(a - b) < kPrecision;
}

bool Normalization::equalPoints(const Point &p1, const Point &p2)
{
    return equalFloat(p1.x, p2.x) && equalFloat(p1.y, p2.y) &&
                equalFloat(p1.z, p2.z);
}

bool Normalization::isVertex(const Point &p, const Triangle &triangle)
{
    return equalPoints(p, triangle.p[0]) || equalPoints(p, triangle.p[1]) ||
                equalPoints(p, triangle.p[2]);
}

bool Normalization::isVertex(const Point &p, const TriangleN &triangle)
{
    return equalPoints(p, triangle.p[0].p) || equalPoints(p, triangle.p[1].p) ||
                equalPoints(p, triangle.p[2].p);
}
