/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * space_types.h is part of 3D Meta-Object-based Modelling System            *
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

#ifndef SPACE_TYPES_H
#define SPACE_TYPES_H

#include <stdio.h>

// Represents point in 3D space.
typedef struct
{
    float x;
    float y;
    float z;
} Point;

// Represents point with normal in 3D space.
typedef struct // Point in 3d space with normals.
{
    Point p;
    Point n;
} PointN;

// Represents triangle in 3D space.
typedef struct
{
    Point p[3];
} Triangle;

// Represents triangle with normals in vertexes in 3D space.
typedef struct
{
    PointN p[3];
} TriangleN;

#endif // SPACE_TYPES_H
