/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * viewcontroller.h is part of 3D Meta-Object-based Modelling Syste          *
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

#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QtGui/QWidget>
#include <QButtonGroup>
#include <QColor>

#include "ui_viewcontroller.h"

class QBuffer;
class QByteArray;
class QColor;

// Represents view controller widget. It controls OpenGL camera properties,
// display options, isosurface level and grid dimentions.
class ViewController : public QWidget
{
    Q_OBJECT

public:
    ViewController(QWidget *parent = 0);
    void initWithXML(QBuffer *xmlData);
    QByteArray XMLRepresentation();

public slots:
    void setRotationX(int value) { fUI.slRotX->setValue(value); }
    void setRotationY(int value) { fUI.slRotY->setValue(value); }
    void setRotationZ(int value) { fUI.slRotZ->setValue(value); }
    void setScale(int value) { fUI.slScale->setValue(fUI.slScale->value() +
                value); }
    void setGLAreaBackgroundColor(QColor color)
                { fGLAreaBackgroundColor = color; }
    void setGLAreaLigth1Color(QColor color) { fGLAreaLight1Color = color; }
    void setGLAreaLigth2Color(QColor color) { fGLAreaLight2Color = color; }

signals:
    void rotationXChanged(float);
    void rotationYChanged(float);
    void rotationZChanged(float);
    void scaleChanged(float);
    void isoLevelChanged(float);
    void drawAxisesChanged(bool);
    void drawFacesChanged(bool);
    void gridSidesDimentionChanged(int);
    void gridXDimentionChanged(int);
    void gridYDimentionChanged(int);
    void gridZDimentionChanged(int);
    void facesNormalModeChanged(bool);
    void shineButtonPressed();
    void GLAreaBackgroundColorChanged(QColor);
    void GLAreaLigth1ColorChanged(QColor);
    void GLAreaLigth2ColorChanged(QColor);

protected:
    void groupRadioButtons();

    void initConnections();

    void brakeGridDimentionConnections();
    void initSynchronizedGridDimentionConnections();
    void initStandaloneGridDimentionConnections();

private slots:
    void changeRotationX(int value);
    void changeRotationY(int value);
    void changeRotationZ(int value);
    void changeScale(int value);
    void changeScaleMultiplier(double value);
    void changeIsoLevel(int value);
    void changeIsoLeveleMultiplier(double value);
    void changeGridSidesDimention(int value);
    void changeGridXDimention(int value);
    void changeGridYDimention(int value);
    void changeGridZDimention(int value);
    void changeDrawAxises(int value);
    void changeDrawFaces(bool value);
    void changeGridDimentionsSynchronized(int value);
    void changeFacesNormalMode(bool value);

private:
    Ui::ViewController fUI;

    QButtonGroup fPolygonModeGroup;
    QButtonGroup fNormalModeGroup;

    QColor fGLAreaBackgroundColor;
    QColor fGLAreaLight1Color;
    QColor fGLAreaLight2Color;
};

#endif // VIEWCONTROLLER_H
