/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * metaobjectscontroller.cpp is part of 3D Meta-Object-based Modelling       *
 * System                                                                    *
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

#include <QMessageBox>
#include <QInputDialog>
#include <QListWidget>
#include <QVector>
#include <QIcon>
#include <QPixmap>
#include <QMap>
#include <QList>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlSerializer>
#include <QByteArray>
#include <QBuffer>
#include <QIODevice>
#include <QVariant>

#include "metaobjectscontroller.h"
#include "glarea.h"
#include "field.h"
#include "grid.h"
#include "postfixexpr.h"
#include "metaobject.h"

// pictures
#include "fast.xpm"
#include "expression.xpm"

MetaObjectItem::MetaObjectItem(
            const QSharedPointer<MetaObject> &metaObjectPtr) : QListWidgetItem()
{
    fMetaObjectPtr = metaObjectPtr;
}

MetaObjectItem::~MetaObjectItem()
{
}

QVariant MetaObjectItem::data(int role)
{
    QVariant result;
    switch(role)
    {
        case Qt::DisplayRole:
        {
            result = QVariant(fMetaObjectPtr->description());
            break;
        }
        case Qt::DecorationRole:
        {
            switch(fMetaObjectPtr->type())
            {
                case PREDEFINED:
                {
                    result = QVariant(QIcon(QPixmap(fast_xpm)));
                    break;
                }
                case EXPRESSION:
                {
                    result = QVariant(QIcon(QPixmap(expression_xpm)));
                    break;
                }
                default:
                {
                    // should never get here.
                    break;
                }
            }
            break;
        }
        default:
        {
            result = QListWidgetItem::data(role);
            break;
        }
    }
    return result;
}

MetaObjectsController::MetaObjectsController(QWidget *parent) : QWidget(parent),
            fField(kDim, kDim, kDim), fPoligonizator(&fField)
{
    fUI.setupUi(this);
    fUI.saVariables->setHidden(true);

    initConnections();

    // TEMPORARY -->
    fUI.bNext->setHidden(true);
    fUI.bPrevious->setHidden(true);
    fUI.lCount->setHidden(true);
    fUI.chOnlySelected->setHidden(true);
}

void MetaObjectsController::initWithXML(QBuffer *xmlData)
{
    const Grid *grid = fField.grid()->data();
    unsigned int xDim = grid->xDimention();
    unsigned int yDim = grid->yDimention();
    unsigned int zDim = grid->zDimention();
    fField = Field(xDim, yDim, zDim, xmlData);

    float isoLevel = fPoligonizator.isoLevel();
    NormalMode normalMode = fPoligonizator.normalMode();
    fPoligonizator = Poligonizator(&fField);
    fPoligonizator.setIsoLevel(isoLevel);
    fPoligonizator.setNormalMode(normalMode);

    fPoligonizator.recalculateTriangles(true);
    emit trianglesChanged(fPoligonizator.trianglesPtr());

    QList<QSharedPointer<MetaObject> > metaObjects(fField.metaObjects());
    int metaObjectCount = metaObjects.count();
    for (int i = 0; i < metaObjectCount; i++)
    {
        addMetaObjectItem(metaObjects[i]);
    }
}

void MetaObjectsController::initField()
{
    /*
    QByteArray fieldXMLData;
    QBuffer fieldBuffer(&fieldXMLData); // This is a QIODevice.
    fieldBuffer.open(QIODevice::WriteOnly);

    QXmlQuery query;
    query.setQuery("doc('xml/doc.xml')/document/field");

    QXmlSerializer fieldSerializer(query, &fieldBuffer);
    query.evaluateTo(&fieldSerializer);
    fieldBuffer.close();
    fField = Field(&fieldBuffer);

    fField.XMLRepresentation();
    */

    /*
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "40/sqrt((x+22)^2+y^2+z^2)")), 18, 18, 18)));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+(y+22)^2+z^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+y^2+(z+22)^2)")))));

    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/((x+22)^2+y^2+z^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+(y+22)^2+z^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+y^2+(z+22)^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/((x+22)^2+y^2+z^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+(y+22)^2+z^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+y^2+(z+22)^2)")))));

    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/((x+22)^2+y^2+z^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+(y+22)^2+z^2)")))));
    addMetaObject(QSharedPointer<MetaObject>(new PostfixExprMetaObject(
                QSharedPointer<PostfixExpr>(new PostfixExpr(
                            "200/(x^2+y^2+(z+22)^2)")))));
    */
}

void MetaObjectsController::addMetaObjectItem(const QSharedPointer<MetaObject>
            &metaObjectPtr)
{
    MetaObjectItem *metaObjectItem = new MetaObjectItem(metaObjectPtr);
    metaObjectItem->setText(
                metaObjectItem->data(Qt::DisplayRole).value<QString>());
    metaObjectItem->setIcon(
                metaObjectItem->data(Qt::DecorationRole).value<QIcon>());
    fUI.lwMetaObjectsList->addItem(metaObjectItem);
    fUI.lwMetaObjectsList->setCurrentRow(fUI.lwMetaObjectsList->count() - 1);
}

void MetaObjectsController::initConnections()
{
    connect(fUI.bAdd, SIGNAL(clicked(bool)), this,
                SLOT(enterFieldExpression(bool)));
    connect(fUI.bRemove, SIGNAL(clicked(bool)), this,
                SLOT(removeSelectedMetaObject(bool)));
    connect(fUI.lwMetaObjectsList, SIGNAL(itemSelectionChanged()), this,
                SLOT(updateVariableControls()));
}

void MetaObjectsController::clearVariableControls()
{
    QLayoutItem *item;
    while ((item = fUI.flVariables->takeAt(0)) != 0)
    {
        delete item->widget();
        delete item;
    }
}

void MetaObjectsController::variablesChanged(double)
{
    qDebug() << "IN MetaObjectsController::variablesChanged()";
    MetaObjectItem *currentItem = static_cast<MetaObjectItem *>(
                fUI.lwMetaObjectsList->currentItem());
    QSharedPointer<MetaObject> currentMetaObjectPtr =
                currentItem->metaObjectPtr();

    fField.updateMetaObject(currentMetaObjectPtr);
    fPoligonizator.recalculateTriangles();
    emit trianglesChanged(fPoligonizator.trianglesPtr());
}

void MetaObjectsController::updateVariableControls()
{
    clearVariableControls();

    qDebug() << "New selected meta-object!";
    int variableCount = 0;

    MetaObjectItem *currentItem= static_cast<MetaObjectItem *>(
                fUI.lwMetaObjectsList->currentItem());
    if (currentItem)
    {
        VariablesManager variablesManager(
                    currentItem->metaObjectPtr()->variablesManager());
        QStringList variableNames(variablesManager.variableNames());

        variableCount = variableNames.count();
        for (int i = 0; i < variableCount; i++)
        {
            QString varName(variableNames[i]);

            VariableValueWrapper *varValueWrapper =
                        variablesManager.variableValueWrapper(varName).data();

            QLabel *varLabel = new QLabel(varName + " = ");
            QDoubleSpinBox *varSpinBox = new QDoubleSpinBox();
            varSpinBox->setMaximumHeight(25);
            varSpinBox->setMaximum(1000.0);
            varSpinBox->setMinimum(-1000.0);
            varSpinBox->setSingleStep(0.5);
            varSpinBox->setDecimals(4);
            varSpinBox->setValue(varValueWrapper->value());

            fUI.flVariables->addRow(varLabel, varSpinBox);

            connect(varSpinBox, SIGNAL(valueChanged(double)),
                        varValueWrapper, SLOT(setValue(double)));

            connect(varValueWrapper, SIGNAL(valueChanged(double)),
                        this, SLOT(variablesChanged(double)));
        }
    }

    fUI.saVariables->setHidden(currentItem && variableCount ? false : true);
}

void MetaObjectsController::enterFieldExpression(bool)
{
    QString exprString(QInputDialog::getText(this,
                trUtf8("Введите формулу мета-объекта:"),
                QString("F(x, y, z) =\t\t\t\t\t")));
    if (!exprString.isEmpty())
    {
        QSharedPointer<PostfixExpr>
                    exprPtr(new PostfixExpr(exprString.toAscii().data()));

        if (exprPtr->successfullyParsed())
        {
            const Grid *grid = fField.grid()->data();
            unsigned int xDim = grid->xDimention();
            unsigned int yDim = grid->yDimention();
            unsigned int zDim = grid->zDimention();
            QSharedPointer<MetaObject> metaObjectPtr(
                        new PostfixExprMetaObject(xDim, yDim, zDim, exprPtr));
            addMetaObject(metaObjectPtr);
        }
        else
        {
            QMessageBox::information(this, trUtf8("Ошибка в формуле!"),
                        trUtf8("Синтаксическая ошибка."));
        }
    }
}

void MetaObjectsController::removeSelectedMetaObject(bool)
{
    MetaObjectItem *selectedItem = static_cast<MetaObjectItem *>(
                fUI.lwMetaObjectsList->currentItem());

    if (selectedItem)
    {
        removeMetaObject(selectedItem->metaObjectPtr());
        delete selectedItem;
    }

    fUI.lwMetaObjectsList->setCurrentRow(fUI.lwMetaObjectsList->count() - 1);
}

void MetaObjectsController::setIsoLevel(float value)
{
    fPoligonizator.setIsoLevel(value);
    fPoligonizator.recalculateTriangles(false, false);
    emit trianglesChanged(fPoligonizator.trianglesPtr());
}

void MetaObjectsController::setGridSidesDimention(int value)
{

    fField.setGridSidesDimention(value);
    fPoligonizator.recalculateTriangles(true);
    emit trianglesChanged(fPoligonizator.trianglesPtr());
}

void MetaObjectsController::setGridXDimention(int value)
{
    fField.setGridXDimention(value);
    fPoligonizator.recalculateTriangles(true);
    emit trianglesChanged(fPoligonizator.trianglesPtr());
}

void MetaObjectsController::setGridYDimention(int value)
{
    fField.setGridYDimention(value);
    fPoligonizator.recalculateTriangles(true);
    emit trianglesChanged(fPoligonizator.trianglesPtr());
}

void MetaObjectsController::setGridZDimention(int value)
{
    fField.setGridZDimention(value);
    fPoligonizator.recalculateTriangles(true);
    emit trianglesChanged(fPoligonizator.trianglesPtr());
}

void MetaObjectsController::setFacesNormalMode(bool value)
{
    NormalMode mode = value ? FLAT : SMOOTH;
    if (fPoligonizator.normalMode() != mode)
    {
        fPoligonizator.setNormalMode(mode);
        emit trianglesChanged(fPoligonizator.trianglesPtr());
    }
}

void MetaObjectsController::addMetaObject(const QSharedPointer<MetaObject>
            &metaObjectPtr)
{
    addMetaObjectItem(metaObjectPtr);

    fField.addMetaObject(metaObjectPtr);
    fPoligonizator.recalculateTriangles();
    emit trianglesChanged(fPoligonizator.trianglesPtr());
}
void MetaObjectsController::removeMetaObject(const QSharedPointer<MetaObject>
            &metaObjectPtr)
{
    fField.removeMetaObject(metaObjectPtr);
    fPoligonizator.recalculateTriangles();
    emit trianglesChanged(fPoligonizator.trianglesPtr());
    qDebug() << "MetaObjectsList count = " << fUI.lwMetaObjectsList->count();
}
