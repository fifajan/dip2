/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * grid.cpp is part of 3D Meta-Object-based Modelling System                 *
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

#include <stdio.h>
#include <math.h>

#include <QDebug>

#include "grid.h"
#include "fieldobject.h"
#include "space_types.h"

const float kMin = -50.0;
const float kMax = 50.0;
const float kDim = 18;

namespace Util
{
    inline unsigned int maxDimention(unsigned int dim1, unsigned int dim2,
                unsigned int dim3)
    {
        return qMax(qMax(dim1, dim2), dim3);
    }

    inline float axisMin(unsigned int dim, float step)
    {
        return -floorf(0.5 * dim * step);
    }

    inline float axisMax(unsigned int dim, float step)
    {
        return ceilf(0.5 * dim * step);
    }
}

using namespace Util;

Grid::Grid(unsigned int xDim, unsigned int yDim, unsigned int zDim)
            : fPointValues(0),
            fXMin(kMin), fXMax(kMax), fXDim(xDim),
            fYMin(kMin), fYMax(kMax), fYDim(yDim),
            fZMin(kMin), fZMax(kMax), fZDim(zDim)
{
    calculateSteps();
    allocatePoints();
}

Grid::~Grid()
{
    freePoints();
}

void Grid::calculateSteps()
{
    fXStep = sideStep();
    fXMin = axisMin(fXDim, fXStep);
    fXMax = axisMax(fXDim, fXStep);

    fYStep = sideStep();
    fYMin = axisMin(fYDim, fYStep);
    fYMax = axisMax(fYDim, fYStep);

    fZStep = sideStep();
    fZMin = axisMin(fZDim, fZStep);
    fZMax = axisMax(fZDim, fZStep);
}

void Grid::setSidesDimention(int dim)
{
    fXDim = dim;
    fYDim = dim;
    fZDim = dim;

    calculateSteps();

    freePoints();
    allocatePoints();
}

void Grid::setXDimention(int xDim)
{
    fXDim = xDim;
    fXStep = sideStep();
    fXMin = axisMin(fXDim, fXStep);
    fXMax = axisMax(fXDim, fXStep);

    freePoints();
    allocatePoints();
}

void Grid::setYDimention(int yDim)
{
    fYDim = yDim;
    fYStep = sideStep();
    fYMin = axisMin(fYDim, fYStep);
    fYMax = axisMax(fYDim, fYStep);

    freePoints();
    allocatePoints();
}

void Grid::setZDimention(int zDim)
{
    fZDim = zDim;
    fZStep = sideStep();
    fZMin = axisMin(fZDim, fZStep);
    fZMax = axisMax(fZDim, fZStep);

    freePoints();
    allocatePoints();
}

void Grid::fillWithFieldObject(FieldObject *fieldObject)
{
    Point p = { 0.0, 0.0, 0.0 };

    unsigned int zPos = 0;
    unsigned int yPos = 0;
    unsigned int xPos = 0;

    unsigned int index = 0;

    //#pragma omp parallel for
    for(zPos = 0; zPos < fZDim; zPos++)
    //#pragma omp sections
    {
        //#pragma omp section
        {
            p.z = zCoord(zPos);

            for(yPos = 0; yPos < fYDim; yPos++)
            {
                p.y = yCoord(yPos);

                for(xPos = 0; xPos < fXDim; xPos++)
                {
                    p.x = xCoord(xPos);

                    fPointValues[index] = fieldObject->valueAtPoint(p);
                    index++;
                }
            }
        }
    }
}

void Grid::addGrid(const Grid *grid)
{
    unsigned int thisPointCount = pointCount();
    if ( fPointValues && grid->fPointValues &&
                (thisPointCount == grid->pointCount()) )
    {
        for(unsigned int i = 0; i < thisPointCount; i++)
        {
            fPointValues[i] += grid->fPointValues[i];
        }
    }
    else
    {
        printf("Different point counts or no points! Doing noting.\n");
        printf("Self point count = %i.\n", thisPointCount);
        printf("Operand point count = %i.\n", grid->pointCount());
    }
}

void Grid::subtractGrid(const Grid *grid)
{
    unsigned int thisPointCount = pointCount();
    if ( fPointValues && grid->fPointValues &&
                (thisPointCount == grid->pointCount()) )
    {
        for(unsigned int i = 0; i < thisPointCount; i++)
        {
            fPointValues[i] -= grid->fPointValues[i];
        }
    }
    else
    {
        printf("Different point counts or no points! Doing noting.\n");
        printf("Self point count = %i.\n", thisPointCount);
        printf("Operand point count = %i.\n", grid->pointCount());
    }
}

void Grid::zeroizePoints()
{
    if (fPointValues)
    {
        unsigned int thisPointCount = pointCount();
        for(unsigned int i = 0; i < thisPointCount; i++)
        {
            fPointValues[i] = 0.0;
        }
    }
}

void Grid::allocatePoints()
{
    unsigned int thisPointCount = pointCount();
    fPointValues = new float[thisPointCount];

    zeroizePoints();
}

void Grid::freePoints()
{
    if (fPointValues)
    {
        delete[] fPointValues;
        fPointValues = 0;
    }
}

unsigned int Grid::maxSideDimention()
{
    return maxDimention(fXDim, fYDim, fZDim);
}


float Grid::sideStep()
{
    return (kMax - kMin) / maxSideDimention();
}
