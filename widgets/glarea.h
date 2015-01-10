/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * glarea.h is part of 3D Meta-Object-based Modelling System                 *
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

#ifndef GLAREA_H
#define GLAREA_H

#include <math.h>

#include <QGLWidget>
#include <QSharedPointer>
#include <QPoint>
#include <QDebug>

#include "space_types.h"
#include "field.h"
#include "postfixexpr.h"

#define setAndRedraw(v) \
{ \
    v = value; \
    updateGL(); \
}

class QByteArray;
class QColor;

// Represents widget with OpenGL area which shows the result poligonized
// surface. Responds to some mouse activities on it.
class GLArea : public QGLWidget
{
    Q_OBJECT

public:
    GLArea(QWidget *parent);
    QByteArray wavefrontSurfaceRepresentation();

signals:
    void rotationXChanged(int);
    void rotationYChanged(int);
    void rotationZChanged(int);
    void scaleChanged(int);

    void backgroundColorChanged(QColor);
    void light1ColorChanged(QColor);
    void light2ColorChanged(QColor);

public slots:
    void setRotationX(float value) { setAndRedraw(fRotateX); }
    void setRotationY(float value) { setAndRedraw(fRotateY); }
    void setRotationZ(float value) { setAndRedraw(fRotateZ); }
    void setScale(float value) { setAndRedraw(fScale); }
    void setDrawAxises(bool value) { setAndRedraw(fDrawAxises); }
    void setDrawFaces(bool value) { setAndRedraw(fDrawFaces); }
    void setTrianglesPtr(const QSharedPointer<const QVector<TriangleN> > &value)
                { setAndRedraw(fTrianglesPtr); }
    void shineSurface();
    void setBackgroundColor(QColor color);
    void setLight1Color(QColor color);
    void setLight2Color(QColor color);

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);
    void recalculateLigthSpecularColor();
    void applyLigth1OZAngle();
    void applyLigth2OYAngle();
    void applyLightColors();
    void applyBackgroundColor();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void chooseColor();

private:
    void drawTriangles();
    void drawAxises();

private: // data
    int fSelectedMetaObjectIndex;

    float fRotateX;
    float fRotateY;
    float fRotateZ;
    float fScale;

    bool fDrawAxises;
    bool fDrawFaces;

    float fLight1OZAngle;
    float fLight2OYAngle;

    float fLight1Color[4];
    float fLight2Color[4];
    float fLightSpecularColor[4];
    float fBackgroungColor[4];

    QPoint fLastCursorPos;
    QSharedPointer<const QVector<TriangleN> > fTrianglesPtr;
};

#endif // GLAREA_H
