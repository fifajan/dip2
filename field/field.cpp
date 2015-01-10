/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * field.cpp is part of 3D Meta-Object-based Modelling System                *
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

#include <QList>
#include <QVector>
#include <QIODevice>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlItem>
#include <QXmlResultItems>
#include <QXmlSerializer>
#include <QBuffer>

#include "field.h"
#include "postfixexpr.h"
#include "grid.h"

Field::Field(const Field &copyee) : FieldObject(copyee)
{
    fIsoLevel = copyee.fIsoLevel;
    fMetaObjects = copyee.fMetaObjects;
}

Field::Field(unsigned int xDim, unsigned int yDim, unsigned int zDim,
            QBuffer *xmlData)
            : FieldObject(xDim, yDim, zDim, xmlData), fIsoLevel(0)
{
    if (xmlData)
    {
        initWithXML(xmlData);
    }
}

QByteArray Field::XMLRepresentation()
{
    QByteArray fieldXMLData;

    fieldXMLData.append("<field>");

    int metaObjectCount = fMetaObjects.count();
    for (int i = 0; i < metaObjectCount; i++)
    {
        fieldXMLData.append(fMetaObjects[i]->XMLRepresentation());
    }

    fieldXMLData.append("</field>");

    return fieldXMLData;
}

float Field::valueAtPoint(const Point& p)
{
    float result = 0.0;
    int metaObjectCount = fMetaObjects.count();
    //#pragma omp parallel for
    for(int i = 0; i < metaObjectCount; i++)
    {
        result += fMetaObjects[i]->valueAtPoint(p);
    }

    return result;
}

void Field::updateMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr)
{
    metaObjectPtr->swapGrid();
    metaObjectPtr->recalculate();

    metaObjectPtr->swapGrid();
    subtractFieldObject(metaObjectPtr.data());
    metaObjectPtr->swapGrid();
    addFieldObject(metaObjectPtr.data());
}

void Field::useGridOfMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr)
{
    swapGrid();
    useExternalGrid(metaObjectPtr.data());
}

void Field::setGridSidesDimention(unsigned int gridDim)
{
    FieldObject::setGridSidesDimention(gridDim);

    unsigned int metaObjectCount = fMetaObjects.count();
    for(unsigned int i = 0; i < metaObjectCount; i++)
    {
        fMetaObjects[i]->setGridSidesDimention(gridDim);
    }
}

void Field::setGridXDimention(unsigned int gridXDim)
{
    FieldObject::setGridXDimention(gridXDim);

    unsigned int metaObjectCount = fMetaObjects.count();
    for(unsigned int i = 0; i < metaObjectCount; i++)
    {
        fMetaObjects[i]->setGridXDimention(gridXDim);
    }
}

void Field::setGridYDimention(unsigned int gridYDim)
{
    FieldObject::setGridYDimention(gridYDim);

    unsigned int metaObjectCount = fMetaObjects.count();
    for(unsigned int i = 0; i < metaObjectCount; i++)
    {
        fMetaObjects[i]->setGridYDimention(gridYDim);
    }
}

void Field::setGridZDimention(unsigned int gridZDim)
{
    FieldObject::setGridZDimention(gridZDim);

    unsigned int metaObjectCount = fMetaObjects.count();
    for(unsigned int i = 0; i < metaObjectCount; i++)
    {
        fMetaObjects[i]->setGridZDimention(gridZDim);
    }
}

void Field::addMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr)
{
    addFieldObject(metaObjectPtr.data());
    fMetaObjects.append(metaObjectPtr);
}

void Field::removeMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr)
{
    subtractFieldObject(metaObjectPtr.data());
    fMetaObjects.removeOne(metaObjectPtr);
}

bool Field::initWithXML(QBuffer *xmlData)
{
    bool result = false;

    xmlData->open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("field", xmlData);

    QXmlResultItems resultItems;
    query.setQuery("fn:count(fn:doc($field)/field/"
                "meta-object[@type='expression'])");
    // how many expression defined meta-objects we have
    query.evaluateTo(&resultItems);
    xmlData->close();

    int exprMetaObjectCount = resultItems.next().toAtomicValue().toInt();
    qDebug() << "Expression MetaObject count = " << exprMetaObjectCount;

    char queryFormat[] = "let $exprMetaObjects := "
                "fn:doc($field)/field/meta-object[@type='expression'] "
                "return $exprMetaObjects[%i]";

    int xDim = grid()->data()->xDimention();
    int yDim = grid()->data()->yDimention();
    int zDim = grid()->data()->zDimention();

    for (int i = 1; i <= exprMetaObjectCount; i++)
    {
        QByteArray metaObjectXMLData;
        QBuffer metaObjectBuffer(&metaObjectXMLData);
        metaObjectBuffer.open(QIODevice::WriteOnly);

        query.setQuery(QString().sprintf(queryFormat, i));

        QXmlSerializer metaObjectSerializer(query, &metaObjectBuffer);
        query.evaluateTo(&metaObjectSerializer);
        metaObjectBuffer.close();

        PostfixExprMetaObject *metaObject =
                    new PostfixExprMetaObject(xDim, yDim, zDim,
                                &metaObjectBuffer);
        if (!metaObject->isValid())
        {
            qDebug() << "Got invalid MetaObject, skipping it.";
            delete metaObject;
            continue;
        }
        addFieldObject(metaObject);
        fMetaObjects.append(QSharedPointer<MetaObject>(metaObject));
    }

    result = true;
    return result;
}
