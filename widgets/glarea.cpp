/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * glarea.cpp is part of 3D Meta-Object-based Modelling System               *
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
#include <stdio.h>
#include <unistd.h>

#include <GL/glu.h>

#include <QtOpenGL>
#include <QDebug>
#include <QByteArray>
#include <QInputDialog>
#include <QStringList>
#include <QColorDialog>

#include "glarea.h"
#include "field.h"

const char kBackgroungItem[] = "Background";
const char kLight1Item[] = "Light 1";
const char kLight2Item[] = "Light 2";

GLArea::GLArea(QWidget *parent) : QGLWidget(parent),
            fRotateX(0.0), fRotateY(0.0), fRotateZ(0.0), fScale(1.0),
            fDrawAxises(true), fDrawFaces(true), fLight1OZAngle(1.1),
            fLight2OYAngle(2.0)
{
    fLight1Color[0] = 0.75;
    fLight1Color[1] = 1.0;
    fLight1Color[2] = 1.0;
    fLight1Color[3] = 1.0;

    fLight2Color[0] = 0.75;
    fLight2Color[1] = 1.0;
    fLight2Color[2] = 1.0;
    fLight2Color[3] = 1.0;

    recalculateLigthSpecularColor();
    fLightSpecularColor[3] = 1.0;

    fBackgroungColor[0] = 0.2;
    fBackgroungColor[1] = 0.2;
    fBackgroungColor[2] = 0.4;
    fBackgroungColor[3] = 1.0;


    fTrianglesPtr = QSharedPointer<const QVector<TriangleN> >(
                QSharedPointer<QVector<TriangleN> >(new QVector<TriangleN>()));

    updateGL();
}

QByteArray GLArea::wavefrontSurfaceRepresentation()
{
    QByteArray result;

    // f.e.:
    //
    // v  0.00  0.00  0.00
    // v  1.00  2.00  3.00
    // v -1.00 -2.00 -3.00
    // f 1 2 3
    //
    // describes one face defined by three vertexes

    const char vertexEntryFormat[] = "v %6.6f %6.6f %6.6f\n";
    const char faceEntryFormat[] = "f %i %i %i\n";

    TriangleN triangle;
    Point p;
    int vertexIndex = 0;

    int triangleCount = fTrianglesPtr->count();
    int i, j;
    for (i = 0; i < triangleCount; i++)
    {
        triangle = fTrianglesPtr->at(i);
        for (j = 0; j < 3; j++)
        {
            p = triangle.p[j].p;
            result += QString().sprintf(vertexEntryFormat, p.x, p.y, p.z);
        }
        vertexIndex = 3 * i;
        result += QString().sprintf(faceEntryFormat,
                    vertexIndex + 1, vertexIndex + 2, vertexIndex + 3);
    }

    return result;
}

void GLArea::shineSurface()
{
    int steps = 100;
    int delay = 30000 / steps; // in microseconds
    float angle = 2.0 * M_PI / steps;
    for (int i = 0; i <= steps; i++)
    {
        fLight1OZAngle += angle;
        fLight2OYAngle += 2.0 * angle;
        applyLigth1OZAngle();
        applyLigth2OYAngle();
        updateGL();
        usleep(delay);
    }
}

void GLArea::setBackgroundColor(QColor color)
{
    fBackgroungColor[0] = color.redF();
    fBackgroungColor[1] = color.greenF();
    fBackgroungColor[2] = color.blueF();
    applyBackgroundColor();
    emit backgroundColorChanged(color);
    updateGL();
}

void GLArea::setLight1Color(QColor color)
{
    fLight1Color[0] = color.redF();
    fLight1Color[1] = color.greenF();
    fLight1Color[2] = color.blueF();
    applyLightColors();
    emit light1ColorChanged(color);
    updateGL();
}

void GLArea::setLight2Color(QColor color)
{
    fLight2Color[0] = color.redF();
    fLight2Color[1] = color.greenF();
    fLight2Color[2] = color.blueF();
    applyLightColors();
    emit light2ColorChanged(color);
    updateGL();
}

void GLArea::drawAxises()
{
    glLineWidth(2);
    {
        glEnable(GL_LINE_STIPPLE); // dashed negative axises
        {
            glBegin(GL_LINES);
            {
                glColor3f(1.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(-50.0, 0.0, 0.0);

                glColor3f(0.0, 1.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, -50.0, 0.0);

                glColor3f(0.0, 0.0, 1.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, -50.0);
            }
            glEnd();
        }

        glDisable(GL_LINE_STIPPLE); // solid positive axises
        {
            glBegin(GL_LINES);
            {
                glColor3f(1.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(50.0, 0.0, 0.0);

                glColor3f(0.0, 1.0, 0.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, 50.0, 0.0);

                glColor3f(0.0, 0.0, 1.0);
                glVertex3f(0.0, 0.0, 0.0);
                glVertex3f(0.0, 0.0, 50.0);
            }
            glEnd();
        }
    }
    glLineWidth(1);
}

void GLArea::drawTriangles()
{	
    int triangleCount =  fTrianglesPtr->count();
    // qDebug() << "Triangles count = " << triangleCount;

    glBegin(GL_TRIANGLES);
    {
        for(int i = 0; i < triangleCount; i++)
        {
            // TODO: optimize flat normals case
            TriangleN tri = fTrianglesPtr->at(i);
            float triP1Coords[] = { tri.p[0].p.x, tri.p[0].p.y, tri.p[0].p.z };
            float triP2Coords[] = { tri.p[1].p.x, tri.p[1].p.y, tri.p[1].p.z };
            float triP3Coords[] = { tri.p[2].p.x, tri.p[2].p.y, tri.p[2].p.z };

            float triN1Coords[] = { tri.p[0].n.x, tri.p[0].n.y, tri.p[0].n.z };
            float triN2Coords[] = { tri.p[1].n.x, tri.p[1].n.y, tri.p[1].n.z };
            float triN3Coords[] = { tri.p[2].n.x, tri.p[2].n.y, tri.p[2].n.z };
            //float triNCoords[] = { tri.p[0].n.x, tri.p[0].n.y, tri.p[0].n.z };

            glNormal3fv(triN1Coords);
            glVertex3fv(triP1Coords);

            glNormal3fv(triN2Coords);
            glVertex3fv(triP2Coords);

            glNormal3fv(triN3Coords);
            glVertex3fv(triP3Coords);
        }
    }
    glEnd();
}


void GLArea::initializeGL()
{	
    glMatrixMode(GL_PROJECTION); //select projection matrix
    glLoadIdentity(); //reset

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);
    applyBackgroundColor();

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_NORMALIZE);

    float ambient[]={0.05f, 0.05f, 0.05f, 1.0f};

    float shininess=64.0f;

    //set up lighting
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);

    applyLigth1OZAngle();
    applyLigth2OYAngle();
    applyLightColors();

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);

    //set full reflectance
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    glEnable(GL_LIGHTING);

    // glEnable (GL_LINE_SMOOTH); // Enable Antialiased lines
    glPushAttrib (GL_LINE_BIT);
    glLineStipple (2, 0x0505); // dash pattern
}

void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode( GL_PROJECTION ); // Выбор матрицы проекций

    glPopMatrix();
    glPushMatrix();

    glRotatef(fRotateX, 1.0, 0.0, 0.0);
    glRotatef(fRotateY, 0.0, 1.0, 0.0);
    glRotatef(fRotateZ, 0.0, 0.0, 1.0);

    glMatrixMode( GL_MODELVIEW ); // Выбор матрицы вида модели
    glLoadIdentity();
    glScalef(fScale, fScale, fScale);

    glDisable(GL_LIGHTING);
    {
        if(fDrawAxises)
        {
            drawAxises();
        }
    }
    glEnable(GL_LIGHTING);

    if (fDrawFaces)
    {

        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT, GL_FILL);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT, GL_LINE);
    }

    drawTriangles();
}

void GLArea::resizeGL(int width, int height)
{
    glViewport(0.0, 0.0, width, height);

    glMatrixMode( GL_PROJECTION ); // Выбор матрицы проекций

    glLoadIdentity(); // Сброс матрицы проекции

    // Вычисление соотношения геометрических размеров для окна
    gluPerspective( 65.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f );
    gluLookAt(30.0, 30.0, 30.0, 0.0, 0.0, 0.0, 30.0, 30.0, 31.0);
    glPushMatrix();

    glMatrixMode( GL_MODELVIEW ); // Выбор матрицы вида модели

    glLoadIdentity(); // Сброс матрицы вида модели
}

void GLArea::recalculateLigthSpecularColor()
{
    for (int i = 0; i < 3; i++)
    {
        fLightSpecularColor[i] = 0.4f * (fLight1Color[i] + fLight2Color[i]);
    }
}

void GLArea::applyLigth1OZAngle()
{
    float position[4];
    position[0] = 100.0 * cosf(fLight1OZAngle);
    position[1] = 0.0;
    position[2] = 100.0 * sinf(fLight1OZAngle);
    position[3] = 1.0;

    glLightfv(GL_LIGHT1, GL_POSITION, position);
}

void GLArea::applyLigth2OYAngle()
{
    float position[4];
    position[0] = 100.0 * cosf(fLight2OYAngle);
    position[1] = 100.0 * sinf(fLight2OYAngle);
    position[2] = 0.0;
    position[3] = 1.0;

    glLightfv(GL_LIGHT2, GL_POSITION, position);
}

void GLArea::applyLightColors()
{
    glLightfv(GL_LIGHT1, GL_DIFFUSE, fLight1Color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, fLightSpecularColor);

    glLightfv(GL_LIGHT2, GL_DIFFUSE, fLight2Color);
    glLightfv(GL_LIGHT2, GL_SPECULAR, fLightSpecularColor);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fLightSpecularColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
                fLightSpecularColor);
}

void GLArea::applyBackgroundColor()
{
    glClearColor(fBackgroungColor[0], fBackgroungColor[1], fBackgroungColor[2],
                fBackgroungColor[3]);
}

void GLArea::mousePressEvent(QMouseEvent *event)
{
    fLastCursorPos = event->pos();
}

void GLArea::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - fLastCursorPos.x();
    int dy = event->y() - fLastCursorPos.y();

    if (event->buttons() & Qt::LeftButton)
    {
        fRotateX += 3 * dy;
        fRotateY += 3 * dx;
        emit rotationXChanged((int)fRotateX);
        emit rotationYChanged((int)fRotateY);
    }
    else if (event->buttons() & Qt::RightButton)
    {
        fRotateX += 3 * dy;
        fRotateZ += 3 * dx;
        emit rotationXChanged((int)fRotateX);
        emit rotationZChanged((int)fRotateZ);
    }

    fLastCursorPos = event->pos();
    updateGL();
}

void GLArea::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    emit scaleChanged(numSteps);
}

void GLArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    chooseColor();
}

void GLArea::chooseColor()
{
    QStringList items;
    items << kBackgroungItem << kLight1Item << kLight2Item;
    bool ok = false;
    QString selectedItem(QInputDialog::getItem(this, trUtf8("Изменить цвет"),
                trUtf8("для"), items, 0, false, &ok));
    if (ok && !selectedItem.isEmpty())
    {
        float *colotPtr = selectedItem == kBackgroungItem
                    ? fBackgroungColor : selectedItem == kLight1Item
                                ? fLight1Color : fLight2Color;
        QColor initialColor;
        initialColor.setRedF(colotPtr[0]);
        initialColor.setGreenF(colotPtr[1]);
        initialColor.setBlueF(colotPtr[2]);
        QColor selectedColor(QColorDialog::getColor(initialColor, this,
                    trUtf8("Выберите цвет для ") + selectedItem));
        if (selectedColor.isValid())
        {
            if (selectedItem == kBackgroungItem)
            {
                setBackgroundColor(selectedColor);
            }
            else if (selectedItem == kLight1Item)
            {
                setLight1Color(selectedColor);
            }
            else if (selectedItem == kLight2Item)
            {
                setLight2Color(selectedColor);
            }
        }
    }
}
