/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * metaobject.cpp is part of 3D Meta-Object-based Modelling System           *
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

#include <QXmlQuery>
#include <QXmlItem>
#include <QXmlSerializer>
#include <QBuffer>
#include <QXmlResultItems>
#include <QString>
#include <QDebug>

#include "space_types.h"
#include "metaobject.h"
#include "postfixexpr.h"

const char kTypeExpression[] = "expression";
const char kTypePredefined[] = "predefined";

PostfixExprMetaObject::PostfixExprMetaObject(unsigned int xDim,
            unsigned int yDim, unsigned int zDim,
            const QSharedPointer<PostfixExpr> &postfixExprPtr)
            : MetaObject(xDim, yDim, zDim, 0), fIsValid(false)
{
    fIsValid = setPostfixExpression(postfixExprPtr);
    if (fIsValid)
    {
        recalculate();
    }
}

PostfixExprMetaObject::PostfixExprMetaObject(unsigned int xDim,
            unsigned int yDim, unsigned int zDim, QBuffer *xmlData)
            : MetaObject(xDim, yDim, zDim, xmlData), fIsValid(false)
{
    fIsValid = initWithXML(xmlData);
    if (fIsValid)
    {
        recalculate();
    }
}

QByteArray PostfixExprMetaObject::XMLRepresentation()
{
    QByteArray metaObjectXMLData;
    QBuffer metaObjectXMLBuffer(&metaObjectXMLData);
    metaObjectXMLBuffer.open(QIODevice::WriteOnly);

    QByteArray variablesXMLData(fUserVariablesManager.XMLRepresentation());
    QBuffer variablesXMLBuffer(&variablesXMLData);
    variablesXMLBuffer.open(QIODevice::ReadOnly);

    QXmlQuery query;
    QString expression(fPostfixExprPtr->infixString());
    query.bindVariable("expr", QVariant(expression));
    query.bindVariable("variables", &variablesXMLBuffer);

    query.setQuery("<meta-object type=\"expression\" value=\"{ $expr }\">"
                "{ fn:doc($variables)/variables/var }</meta-object>");
    QXmlSerializer metaObjectSerializer(query, &metaObjectXMLBuffer);
    query.evaluateTo(&metaObjectSerializer);

    variablesXMLBuffer.close();
    metaObjectXMLBuffer.close();

    return metaObjectXMLData;
}

float PostfixExprMetaObject::valueAtPoint(const Point& p)
{
    fTemporaryVariables["x"] = p.x;
    fTemporaryVariables["y"] = p.y;
    fTemporaryVariables["z"] = p.z;

    fVariablesManager.setVariableValues(fTemporaryVariables);
    return fPostfixExprPtr->execute();
}

VariablesManager PostfixExprMetaObject::variablesManager()
{
    return fUserVariablesManager;
}

QString PostfixExprMetaObject::description()
{
    return fPostfixExprPtr->infixString();
}

bool PostfixExprMetaObject::initWithXML(QBuffer *xmlData)
{
    bool result = false;

    xmlData->open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("metaObject", xmlData);

    QXmlResultItems resultItems;
    query.setQuery("fn:doc($metaObject)/meta-object/fn:data(@value)");
    query.evaluateTo(&resultItems);
    xmlData->close();

    QString expr(resultItems.next().toAtomicValue().toString());
    qDebug() << "MetaObject expression = " << expr;

    QSharedPointer<PostfixExpr> exprPtr(new PostfixExpr(expr));
    if (exprPtr->successfullyParsed())
    {
        exprPtr->variablesManager().setVariableValuesWithXML(xmlData);
        setPostfixExpression(exprPtr);

        result = true;
    }

    return result;
}

bool PostfixExprMetaObject::setPostfixExpression(
            const QSharedPointer<PostfixExpr> &postfixExprPtr)
{
    bool result = false;

    if (postfixExprPtr->successfullyParsed())
    {
        fPostfixExprPtr = postfixExprPtr;
        fVariablesManager = fPostfixExprPtr->variablesManager();

        fUserVariablesManager = fVariablesManager;
        fUserVariablesManager.removeVariable("x");
        fUserVariablesManager.removeVariable("y");
        fUserVariablesManager.removeVariable("z");

        result = true;
    }

    return result;
}

float CylinderMetaObject::valueAtPoint(const Point& p)
{
    float r2 = 900;
    float d2 = (p.x * p.x + p.y * p.y + p.z * p.z);
    if (d2 < 0.0001) d2 = 0.0001;
    float result = r2 / d2;

    return result;
}

QMap<QString, QSharedPointer<double> > CylinderMetaObject::variables()
{
    return QMap<QString, QSharedPointer<double> >();
}
