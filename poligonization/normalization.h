/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * normalization.h is part of 3D Meta-Object-based Modelling System          *
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

#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include <QVector>

#include "space_types.h"

// Represents set of functions for normal vectors calculation. Normal vectors
// are needed to provide correct lighting in OpenGL
namespace Normalization
{
    Point vector(const Point &a, const Point &b);
    Point normal(const Point &v1, const Point &v2);
    Point normalizeVector(const Point &v);
    TriangleN normalizeTriangle(const Triangle &triangle);
    PointN normalizeVertex(const Point &v,
                const QVector<Triangle> &adjacentTriangles);
    PointN normalizeVertex(const Point &v,
                const QVector<TriangleN> &adjacentTriangles);
    bool equalFloat(float a, float b);
    bool equalPoints(const Point &p1, const Point &p2);
    bool isVertex(const Point &p, const Triangle &triangle);
    bool isVertex(const Point &p, const TriangleN &triangle);
}

#endif // NORMALIZATION_H
