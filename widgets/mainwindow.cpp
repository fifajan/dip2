/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * mainwindow.cpp is part of 3D Meta-Object-based Modelling System           *
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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QApplication>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlSerializer>
#include <QXmlFormatter>
#include <QBuffer>

#include "mainwindow.h"
#include "postfixexpr.h"
#include "glarea.h"
#include "viewcontroller.h"
#include "metaobjectscontroller.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent)
{
    fUI.setupUi(this);

    configurePanelsMenu();
    initConnections();

    fUI.wMetaObjectsController->initField();

    // documentXMLData();
}

MainWindow::~MainWindow()
{
}

void MainWindow::openDocument()
{
    QString filename = QFileDialog::getOpenFileName(this,
                trUtf8("Открыть документ"), "", tr("Meta Objects XML (*.mox)"));

    if (!filename.isEmpty())
    {
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        QByteArray xmlData(file.readAll());
        file.close();

        QBuffer xmlBuffer(&xmlData);
        initDocumentWithXML(&xmlBuffer);
    }
}

void MainWindow::saveDocument()
{
    QString filename = QFileDialog::getSaveFileName(this,
                trUtf8("Сохранить документ"), "",
                tr("Meta Objects XML (*.mox)"));

    if (!filename.isEmpty())
    {
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(documentXMLData());
        file.close();
    }
}

void MainWindow::exportSurfaceToWavefront()
{
    QString filename = QFileDialog::getSaveFileName(this,
                trUtf8("Экспортировать поверхность в Wavefront Object"), "",
                tr("Wavefront Object (*.obj)"));

    if (!filename.isEmpty())
    {
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(fUI.wGLArea->wavefrontSurfaceRepresentation());
        file.close();
    }
}

void MainWindow::initConnections()
{    
    // ViewController <-> GLArea
    connect(fUI.wViewController, SIGNAL(rotationXChanged(float)),
                fUI.wGLArea, SLOT(setRotationX(float)));
    connect(fUI.wViewController, SIGNAL(rotationYChanged(float)),
                fUI.wGLArea, SLOT(setRotationY(float)));
    connect(fUI.wViewController, SIGNAL(rotationZChanged(float)),
                fUI.wGLArea, SLOT(setRotationZ(float)));
    connect(fUI.wViewController, SIGNAL(scaleChanged(float)),
                fUI.wGLArea, SLOT(setScale(float)));
    connect(fUI.wViewController, SIGNAL(drawAxisesChanged(bool)),
                fUI.wGLArea, SLOT(setDrawAxises(bool)));
    connect(fUI.wViewController, SIGNAL(drawFacesChanged(bool)),
                fUI.wGLArea, SLOT(setDrawFaces(bool)));
    connect(fUI.wViewController, SIGNAL(shineButtonPressed()),
                fUI.wGLArea, SLOT(shineSurface()));

    // ViewController <-> GLArea colors
    connect(fUI.wViewController, SIGNAL(GLAreaBackgroundColorChanged(QColor)),
                fUI.wGLArea, SLOT(setBackgroundColor(QColor)));
    connect(fUI.wViewController, SIGNAL(GLAreaLigth1ColorChanged(QColor)),
                fUI.wGLArea, SLOT(setLight1Color(QColor)));
    connect(fUI.wViewController, SIGNAL(GLAreaLigth2ColorChanged(QColor)),
                fUI.wGLArea, SLOT(setLight2Color(QColor)));
    connect(fUI.wGLArea, SIGNAL(backgroundColorChanged(QColor)),
                fUI.wViewController, SLOT(setGLAreaBackgroundColor(QColor)));
    connect(fUI.wGLArea, SIGNAL(light1ColorChanged(QColor)),
                fUI.wViewController, SLOT(setGLAreaLigth1Color(QColor)));
    connect(fUI.wGLArea, SIGNAL(light2ColorChanged(QColor)),
                fUI.wViewController, SLOT(setGLAreaLigth2Color(QColor)));

    // GLArea cursor rotations & wheel scale <-> ViewController
    connect(fUI.wGLArea, SIGNAL(rotationXChanged(int)),
                fUI.wViewController, SLOT(setRotationX(int)));
    connect(fUI.wGLArea, SIGNAL(rotationYChanged(int)),
                fUI.wViewController, SLOT(setRotationY(int)));
    connect(fUI.wGLArea, SIGNAL(rotationZChanged(int)),
                fUI.wViewController, SLOT(setRotationZ(int)));
    connect(fUI.wGLArea, SIGNAL(scaleChanged(int)),
                fUI.wViewController, SLOT(setScale(int)));

    // MainMenu <-> GLArea
    connect(fUI.aAdd, SIGNAL(triggered(bool)), fUI.wMetaObjectsController,
                SLOT(enterFieldExpression(bool)));
    connect(fUI.aRemove, SIGNAL(triggered(bool)), fUI.wMetaObjectsController,
                SLOT(removeSelectedMetaObject(bool)));

    // ViewController <-> MetaObjectsController
    connect(fUI.wViewController, SIGNAL(isoLevelChanged(float)),
                fUI.wMetaObjectsController, SLOT(setIsoLevel(float)));
    connect(fUI.wViewController, SIGNAL(gridSidesDimentionChanged(int)),
                fUI.wMetaObjectsController, SLOT(setGridSidesDimention(int)));
    connect(fUI.wViewController, SIGNAL(gridXDimentionChanged(int)),
                fUI.wMetaObjectsController, SLOT(setGridXDimention(int)));
    connect(fUI.wViewController, SIGNAL(gridYDimentionChanged(int)),
                fUI.wMetaObjectsController, SLOT(setGridYDimention(int)));
    connect(fUI.wViewController, SIGNAL(gridZDimentionChanged(int)),
                fUI.wMetaObjectsController, SLOT(setGridZDimention(int)));
    connect(fUI.wViewController, SIGNAL(gridZDimentionChanged(int)),
                fUI.wMetaObjectsController, SLOT(setGridZDimention(int)));
    connect(fUI.wViewController, SIGNAL(facesNormalModeChanged(bool)),
                fUI.wMetaObjectsController, SLOT(setFacesNormalMode(bool)));

    // MetaObjectsController <-> GLArea
    connect(fUI.wMetaObjectsController, SIGNAL(trianglesChanged(
                            const QSharedPointer<const QVector<TriangleN> > &)),
                fUI.wGLArea, SLOT(setTrianglesPtr(const QSharedPointer<
                            const QVector<TriangleN> > &)));

    // MainMenu decument operations
    connect(fUI.aExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(fUI.aOpen, SIGNAL(triggered()), this, SLOT(openDocument()));
    connect(fUI.aSave, SIGNAL(triggered()), this, SLOT(saveDocument()));
    connect(fUI.aExportToOpj, SIGNAL(triggered()),
                this, SLOT(exportSurfaceToWavefront()));
}

void MainWindow::configurePanelsMenu()
{
    fUI.mPanels->addAction(fUI.dwMetaObjectsController->toggleViewAction());
    fUI.mPanels->addAction(fUI.dwViewController->toggleViewAction());
}

QByteArray MainWindow::documentXMLData()
{
    QByteArray docXMLData;
    QBuffer docXMLBuffer(&docXMLData);
    docXMLBuffer.open(QIODevice::WriteOnly);
    docXMLBuffer.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    QByteArray viewXMLData(fUI.wViewController->XMLRepresentation());
    QBuffer viewXMLBuffer(&viewXMLData);
    viewXMLBuffer.open(QIODevice::ReadOnly);
    QByteArray fieldXMLData(
                fUI.wMetaObjectsController->fieldXMLRepresentation());
    QBuffer fieldXMLBuffer(&fieldXMLData);
    fieldXMLBuffer.open(QIODevice::ReadOnly);

    QXmlQuery query;
    query.bindVariable("view", &viewXMLBuffer);
    query.bindVariable("field", &fieldXMLBuffer);
    query.setQuery("<document type=\"meta-objects\" name=\"not-used\">"
                "{ fn:doc($view) }{ fn:doc($field) }</document>");

    QXmlFormatter docFormatter(query, &docXMLBuffer);
    query.evaluateTo(&docFormatter);

    docXMLBuffer.close();
    viewXMLBuffer.close();
    fieldXMLBuffer.close();

    //qDebug() << "XML Document representation = "
    //            << QString(docXMLData.data());

    return docXMLData;
}

void MainWindow::initDocumentWithXML(QBuffer *xmlData)
{
    QXmlQuery query;

    QByteArray viewXMLData;
    QBuffer viewXMLBuffer(&viewXMLData);
    viewXMLBuffer.open(QIODevice::WriteOnly);
    QXmlSerializer viewSerializer(query, &viewXMLBuffer);

    QByteArray fieldXMLData;
    QBuffer fieldXMLBuffer(&fieldXMLData);
    fieldXMLBuffer.open(QIODevice::WriteOnly);
    QXmlSerializer fieldSerializer(query, &fieldXMLBuffer);

    xmlData->open(QIODevice::ReadOnly);
    query.bindVariable("document", xmlData);

    query.setQuery("fn:doc($document)/document/view");
    query.evaluateTo(&viewSerializer);

    query.setQuery("fn:doc($document)/document/field");
    query.evaluateTo(&fieldSerializer);
    xmlData->close();

    if (viewXMLData.count() > 0)
    {
        //qDebug() << "Initializing ViewController with XML: "
        //            << QString(viewXMLData);
        fUI.wViewController->initWithXML(&viewXMLBuffer);
    }
    if (fieldXMLData.count() > 0)
    {
        //qDebug() << "Initializing MetaObjectsController with XML: "
        //            << QString(fieldXMLData);
        fUI.wMetaObjectsController->initWithXML(&fieldXMLBuffer);
    }
}
