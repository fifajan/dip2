/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * grid.h is part of 3D Meta-Object-based Modelling System                   *
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

#ifndef GRID_H
#define GRID_H

extern const float kDim;

class FieldObject;

// Represents a centered cubic grid with support of different dimentions on
// x, y, z sides. A potential value is defined in each grid point. So grid
// holds some field-object's field potential values. Grid supports subtaction
// and addition of potentian values of other grid.
class Grid
{
public:
    Grid(unsigned int xDim, unsigned int yDim, unsigned int zDim);
    ~Grid();

    void setSidesDimention(int dim);
    void setXDimention(int dim);
    void setYDimention(int dim);
    void setZDimention(int dim);

    inline float xMin() const { return fXMin; }
    inline float xMax() const { return fXMax; }
    inline float xStep() const { return fXStep; }
    inline unsigned int xDimention() const { return fXDim; } // in poins.

    inline float yMin() const { return fYMin; }
    inline float yMax() const { return fYMax; }
    inline float yStep() const { return fYStep; }
    inline unsigned int yDimention() const { return fYDim; } // in points.

    inline float zMin() const { return fZMin; }
    inline float zMax() const { return fZMax; }
    inline float zStep() const { return fZStep; }
    inline unsigned int zDimention() const { return fZDim; } // in points.

    inline unsigned int pointCount() const { return fXDim * fYDim * fZDim; }
    inline unsigned int cellCount() const { return (fXDim - 1) * (fYDim - 1) *
                (fZDim - 1); }

    // Get point index (used to access pointValues() return value) by
    // giving point position in cartesian space coordinates.
    inline unsigned int pointIndex(unsigned int xPos, unsigned int yPos,
                unsigned int zPos) const
                { return (fXDim * fYDim * zPos) + (fXDim * yPos) + xPos; }

    // Get real cartesian space coordinates of cell's first (0-indexed) vertex.
    inline float xCoord(unsigned int xPos) const
                { return fXMin + (xPos * fXStep); }
    inline float yCoord(unsigned int yPos) const
                { return fYMin + (yPos * fYStep); }
    inline float zCoord(unsigned int zPos) const
                { return fZMin + (zPos * fZStep); }

    void fillWithFieldObject(FieldObject *fieldObject);
    void addGrid(const Grid *grid);
    void subtractGrid(const Grid *grid);

    const float *pointValues() const { return fPointValues; }

protected:
    void calculateSteps(); // TODO: clear cell dimention vs. point dimention
                           // question!!!

    void zeroizePoints();
    void allocatePoints();
    void freePoints();

    inline unsigned int maxSideDimention();
    inline unsigned int minSideDimention();
    inline float sideStep();

private: // data

    float *fPointValues;

    float fXMin; // Grid ranges.
    float fXMax;
    float fXStep;
    unsigned int fXDim;

    float fYMin;
    float fYMax;
    float fYStep;
    unsigned int fYDim;

    float fZMin;
    float fZMax;
    float fZStep;
    unsigned int fZDim;
};

#endif // GRID_H
