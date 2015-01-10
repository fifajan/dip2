/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * viewcontroller.cpp is part of 3D Meta-Object-based Modelling Syste        *
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

#include <QXmlQuery>
#include <QXmlItem>
#include <QXmlSerializer>
#include <QBuffer>
#include <QXmlResultItems>
#include <QDebug>

#include "viewcontroller.h"

ViewController::ViewController(QWidget *parent) : QWidget(parent)
{
    fGLAreaBackgroundColor = QColor("#333366");
    fGLAreaLight1Color = QColor("#bfffff");
    fGLAreaLight2Color = QColor("#bfffff");
    fUI.setupUi(this);
    groupRadioButtons();
    initConnections();
}

void ViewController::initWithXML(QBuffer *xmlData)
{
    xmlData->open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("view", xmlData);
    QXmlResultItems resultItems;
    QXmlItem item;

    query.setQuery("fn:doc($view)/view/camera/fn:data(@rotX)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slRotX->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/camera/fn:data(@rotY)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slRotY->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/camera/fn:data(@rotZ)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slRotZ->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/camera/fn:data(@scaleMult)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.sbScaleMult->setValue(item.toAtomicValue().toDouble());
    }

    query.setQuery("fn:doc($view)/view/camera/fn:data(@scale)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slScale->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/grid/fn:data(@isoMult)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.sbIsoLevelMult->setValue(item.toAtomicValue().toDouble());
    }

    query.setQuery("fn:doc($view)/view/grid/fn:data(@iso)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slIsoLevel->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/grid/fn:data(@dimX)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slGridXDim->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/grid/fn:data(@dimY)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slGridYDim->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/grid/fn:data(@dimZ)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.slGridZDim->setValue(item.toAtomicValue().toInt());
    }

    query.setQuery("fn:doc($view)/view/display/fn:data(@axises)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fUI.chAxisesShow->setCheckState((item.toAtomicValue().toBool())
                    ? Qt::Checked : Qt::Unchecked);
    }

    query.setQuery("fn:doc($view)/view/display/fn:data(@polygonMode)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        if ((item.toAtomicValue().toString()) == "edges")
        {
            fUI.rbEdges->setChecked(true);
        }
    }

    query.setQuery("fn:doc($view)/view/display/fn:data(@normalMode)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        if ((item.toAtomicValue().toString()) == "smooth")
        {
            fUI.rbNormalVertexes->setChecked(true);
        }
    }

    query.setQuery("fn:doc($view)/view/colors/fn:data(@background)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fGLAreaBackgroundColor = QColor(item.toAtomicValue().toString());
        emit GLAreaBackgroundColorChanged(fGLAreaBackgroundColor);
    }

    query.setQuery("fn:doc($view)/view/colors/fn:data(@light1)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fGLAreaLight1Color = QColor(item.toAtomicValue().toString());
        emit GLAreaLigth1ColorChanged(fGLAreaLight1Color);
    }

    query.setQuery("fn:doc($view)/view/colors/fn:data(@light2)");
    query.evaluateTo(&resultItems);
    item = resultItems.next();
    if (!item.isNull())
    {
        fGLAreaLight2Color = QColor(item.toAtomicValue().toString());
        emit GLAreaLigth2ColorChanged(fGLAreaLight2Color);
    }

    xmlData->close();
}

QByteArray ViewController::XMLRepresentation()
{
    char gridFormat[] = "<grid dimX=\"%i\" dimY=\"%i\" dimZ=\"%i\" "
                "isoMult=\"%4.4f\" iso=\"%i\"/>";
    char cameraFormat[] = "<camera rotX=\"%i\" rotY=\"%i\" rotZ=\"%i\" "
                "scaleMult=\"%4.4f\" scale=\"%i\"/>";
    char displayFormat[] = "<display axises=\"%s\" "
                "polygonMode=\"%s\" normalMode=\"%s\"/>";
    char colorsFormat[] = "<colors background=\"%s\" "
                "light1=\"%s\" light2=\"%s\"/>";

    QString gridXMLData(QString().sprintf(gridFormat, fUI.slGridXDim->value(),
                fUI.slGridYDim->value(), fUI.slGridZDim->value(),
                fUI.sbIsoLevelMult->value(), fUI.slIsoLevel->value()));
    QString cameraXMLData(QString().sprintf(cameraFormat, fUI.slRotX->value(),
                fUI.slRotY->value(), fUI.slRotZ->value(),
                fUI.sbScaleMult->value(), fUI.slScale->value()));
    QString displayXMLData(QString().sprintf(displayFormat,
                (fUI.chAxisesShow->isChecked()) ? "true" : "false",
                (fUI.rbFaces->isChecked()) ? "faces" : "edges",
                (fUI.rbNormalFaces->isChecked()) ? "flat" : "smooth"));
    QString colorsXMLData(QString().sprintf(colorsFormat,
                fGLAreaBackgroundColor.name().toAscii().data(),
                fGLAreaLight1Color.name().toAscii().data(),
                fGLAreaLight2Color.name().toAscii().data()));

    QByteArray viewXMLData("<view>");
    viewXMLData += gridXMLData;
    viewXMLData += cameraXMLData;
    viewXMLData += displayXMLData;
    viewXMLData += colorsXMLData;
    viewXMLData += "</view>";

    return viewXMLData;
}

void ViewController::groupRadioButtons()
{
    fPolygonModeGroup.addButton(fUI.rbEdges);
    fPolygonModeGroup.addButton(fUI.rbFaces);

    fNormalModeGroup.addButton(fUI.rbNormalFaces);
    fNormalModeGroup.addButton(fUI.rbNormalVertexes);
}

void ViewController::initConnections()
{
    // Rotation aroud oX
    connect(fUI.slRotX, SIGNAL(valueChanged(int)),
                this, SLOT(changeRotationX(int)));
    connect(fUI.slRotX, SIGNAL(valueChanged(int)),
                fUI.dRotX, SLOT(setValue(int)));
    connect(fUI.dRotX, SIGNAL(valueChanged(int)),
                fUI.sbRotX, SLOT(setValue(int)));
    connect(fUI.sbRotX, SIGNAL(valueChanged(int)),
                fUI.slRotX, SLOT(setValue(int)));

    // Rotation aroud oY
    connect(fUI.slRotY, SIGNAL(valueChanged(int)),
                this, SLOT(changeRotationY(int)));
    connect(fUI.slRotY, SIGNAL(valueChanged(int)),
                fUI.dRotY, SLOT(setValue(int)));
    connect(fUI.dRotY, SIGNAL(valueChanged(int)),
                fUI.sbRotY, SLOT(setValue(int)));
    connect(fUI.sbRotY, SIGNAL(valueChanged(int)),
                fUI.slRotY, SLOT(setValue(int)));

    // Rotation aroud oZ
    connect(fUI.slRotZ, SIGNAL(valueChanged(int)),
                this, SLOT(changeRotationZ(int)));
    connect(fUI.slRotZ, SIGNAL(valueChanged(int)),
                fUI.dRotZ, SLOT(setValue(int)));
    connect(fUI.dRotZ, SIGNAL(valueChanged(int)),
                fUI.sbRotZ, SLOT(setValue(int)));
    connect(fUI.sbRotZ, SIGNAL(valueChanged(int)),
                fUI.slRotZ, SLOT(setValue(int)));

    // Scale
    connect(fUI.sbScaleMult, SIGNAL(valueChanged(double)),
                this, SLOT(changeScaleMultiplier(double)));
    connect(fUI.slScale, SIGNAL(valueChanged(int)),
                this, SLOT(changeScale(int)));
    connect(fUI.slScale, SIGNAL(valueChanged(int)),
                fUI.sbScale, SLOT(setValue(int)));
    connect(fUI.sbScale, SIGNAL(valueChanged(int)),
                fUI.slScale, SLOT(setValue(int)));

    // Isosurface
    connect(fUI.sbIsoLevelMult, SIGNAL(valueChanged(double)),
                this, SLOT(changeIsoLeveleMultiplier(double)));
    connect(fUI.slIsoLevel, SIGNAL(valueChanged(int)),
                this, SLOT(changeIsoLevel(int)));
    connect(fUI.slIsoLevel, SIGNAL(valueChanged(int)),
                fUI.sbIsoLevel, SLOT(setValue(int)));
    connect(fUI.sbIsoLevel, SIGNAL(valueChanged(int)),
                fUI.slIsoLevel, SLOT(setValue(int)));

    // Gdid dimentions
    initStandaloneGridDimentionConnections();
    connect(fUI.chSyncGridDim, SIGNAL(stateChanged(int)),
                this, SLOT(changeGridDimentionsSynchronized(int)));

    // Axises & Edges/Faces
    connect(fUI.chAxisesShow, SIGNAL(stateChanged(int)),
                this, SLOT(changeDrawAxises(int)));
    connect(fUI.rbFaces, SIGNAL(toggled(bool)),
                this, SLOT(changeDrawFaces(bool)));

    // Normal mode
    connect(fUI.rbNormalFaces, SIGNAL(toggled(bool)),
                this, SLOT(changeFacesNormalMode(bool)));

    // Shine button
    connect(fUI.bShine, SIGNAL(clicked()), this, SIGNAL(shineButtonPressed()));
}

void ViewController::brakeGridDimentionConnections()
{
    disconnect(fUI.sbGridXDim, SIGNAL(valueChanged(int)), 0, 0);
    disconnect(fUI.slGridXDim, SIGNAL(valueChanged(int)), 0, 0);

    disconnect(fUI.sbGridYDim, SIGNAL(valueChanged(int)), 0, 0);
    disconnect(fUI.slGridYDim, SIGNAL(valueChanged(int)), 0, 0);

    disconnect(fUI.sbGridZDim, SIGNAL(valueChanged(int)), 0, 0);
    disconnect(fUI.slGridZDim, SIGNAL(valueChanged(int)), 0, 0);
}

void ViewController::initSynchronizedGridDimentionConnections()
{
    // Grid X dimention
    connect(fUI.sbGridXDim, SIGNAL(valueChanged(int)),
                fUI.sbGridYDim, SLOT(setValue(int)));
    connect(fUI.slGridXDim, SIGNAL(valueChanged(int)),
                fUI.sbGridXDim, SLOT(setValue(int)));
    connect(fUI.sbGridXDim, SIGNAL(valueChanged(int)),
                fUI.slGridXDim, SLOT(setValue(int)));

    // Grid Y dimention
    connect(fUI.sbGridYDim, SIGNAL(valueChanged(int)),
                fUI.sbGridZDim, SLOT(setValue(int)));
    connect(fUI.slGridYDim, SIGNAL(valueChanged(int)),
                fUI.sbGridYDim, SLOT(setValue(int)));
    connect(fUI.sbGridYDim, SIGNAL(valueChanged(int)),
                fUI.slGridYDim, SLOT(setValue(int)));

    // Grid Z dimention
    connect(fUI.sbGridZDim, SIGNAL(valueChanged(int)),
                fUI.sbGridXDim, SLOT(setValue(int)));
    connect(fUI.sbGridZDim, SIGNAL(valueChanged(int)),
                this, SLOT(changeGridSidesDimention(int)));
    connect(fUI.slGridZDim, SIGNAL(valueChanged(int)),
                fUI.sbGridZDim, SLOT(setValue(int)));
    connect(fUI.sbGridZDim, SIGNAL(valueChanged(int)),
                fUI.slGridZDim, SLOT(setValue(int)));
}

void ViewController::initStandaloneGridDimentionConnections()
{
    // Grid X dimention
    connect(fUI.sbGridXDim, SIGNAL(valueChanged(int)),
                this, SLOT(changeGridXDimention(int)));
    connect(fUI.slGridXDim, SIGNAL(valueChanged(int)),
                fUI.sbGridXDim, SLOT(setValue(int)));
    connect(fUI.sbGridXDim, SIGNAL(valueChanged(int)),
                fUI.slGridXDim, SLOT(setValue(int)));

    // Grid Y dimention
    connect(fUI.sbGridYDim, SIGNAL(valueChanged(int)),
                this, SLOT(changeGridYDimention(int)));
    connect(fUI.slGridYDim, SIGNAL(valueChanged(int)),
                fUI.sbGridYDim, SLOT(setValue(int)));
    connect(fUI.sbGridYDim, SIGNAL(valueChanged(int)),
                fUI.slGridYDim, SLOT(setValue(int)));

    // Grid Z dimention
    connect(fUI.sbGridZDim, SIGNAL(valueChanged(int)),
                this, SLOT(changeGridZDimention(int)));
    connect(fUI.slGridZDim, SIGNAL(valueChanged(int)),
                fUI.sbGridZDim, SLOT(setValue(int)));
    connect(fUI.sbGridZDim, SIGNAL(valueChanged(int)),
                fUI.slGridZDim, SLOT(setValue(int)));
}

void ViewController::changeRotationX(int value)
{
    qDebug() << QString().sprintf("New RotX value = %4.4f", (float)value);
    emit rotationXChanged((float)value);
}

void ViewController::changeRotationY(int value)
{
    qDebug() << QString().sprintf("New RotY value = %4.4f", (float)value);
    emit rotationYChanged((float)value);
}

void ViewController::changeRotationZ(int value)
{
    qDebug() << QString().sprintf("New RotZ value = %4.4f", (float)value);
    emit rotationZChanged((float)value);
}

void ViewController::changeScale(int value)
{
    qDebug() << QString().sprintf("New Scale value = %4.4f",
                fUI.sbScaleMult->value()*value);
    emit scaleChanged(fUI.sbScaleMult->value()*value);
}

void ViewController::changeScaleMultiplier(double value)
{
    qDebug() << QString().sprintf("New Scale value = %4.4f",
                fUI.slScale->value()*value);
    emit scaleChanged(fUI.slScale->value()*value);
}

void ViewController::changeIsoLevel(int value)
{
    qDebug() << QString().sprintf("New IsoLevel value = %4.4f",
                fUI.sbIsoLevelMult->value()*value);
    emit isoLevelChanged(fUI.sbIsoLevelMult->value()*value);
}

void ViewController::changeIsoLeveleMultiplier(double value)
{
    qDebug() << QString().sprintf("New IsoLevel value = %4.4f",
                fUI.slIsoLevel->value()*value);
    emit isoLevelChanged(fUI.slIsoLevel->value()*value);
}

void ViewController::changeGridSidesDimention(int value)
{
    qDebug() << QString().sprintf("New GridDim value = %i", value);
    emit gridSidesDimentionChanged((float)value);
}

void ViewController::changeGridXDimention(int value)
{
    qDebug() << QString().sprintf("New GridXDim value = %i", value);
    emit gridXDimentionChanged((float)value);
}

void ViewController::changeGridYDimention(int value)
{
    qDebug() << QString().sprintf("New GridYDim value = %i", value);
    emit gridYDimentionChanged((float)value);
}

void ViewController::changeGridZDimention(int value)
{
    qDebug() << QString().sprintf("New GridZDim value = %i", value);
    emit gridZDimentionChanged((float)value);
}

void ViewController::changeDrawAxises(int value)
{
    qDebug() << QString().sprintf("New DrawAxises value = %i", value);
    emit drawAxisesChanged((bool)value);
}

void ViewController::changeDrawFaces(bool value)
{
    qDebug() << QString().sprintf("New DrawFaces value = %i", value);
    emit drawFacesChanged(value);
}

void ViewController::changeGridDimentionsSynchronized(int value)
{
    qDebug() << QString().sprintf("New GridDimentionsSynchronized value = %i",
                value);
    brakeGridDimentionConnections();
    if (Qt::Checked == value)
    {
        initSynchronizedGridDimentionConnections();
    }
    else
    {
        initStandaloneGridDimentionConnections();
    }
}

void ViewController::changeFacesNormalMode(bool value)
{
    qDebug() << QString().sprintf("New NormalFacesMode value = %i", value);
    emit facesNormalModeChanged(value);
}
