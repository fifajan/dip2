/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * metaobject.h is part of 3D Meta-Object-based Modelling System             *
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

#ifndef METAOBJECT_H
#define METAOBJECT_H

#include <QSharedPointer>
#include <QMap>
#include <QString>

#include "fieldobject.h"
#include "variablesmanager.h"

class QBuffer;
class QByteArray;

typedef enum
{
    PREDEFINED = 1,
    EXPRESSION
} MetaObjectType;

class PostfixExpr;

// Represent meta-object. Class needs additional work.
class MetaObject : public FieldObject
{
public:
    MetaObject(unsigned int xDim, unsigned int yDim, unsigned int zDim,
                QBuffer *xmlData = 0)
                : FieldObject(xDim, yDim, zDim, xmlData) {}

    virtual VariablesManager variablesManager() = 0;
    virtual QString description() = 0;
    virtual MetaObjectType type() = 0;

protected:
    virtual bool initWithXML(QBuffer *xmlData) = 0;
};

// Represent meta-object field potential values of which are defined by a
// postfix expression.
class PostfixExprMetaObject : public MetaObject
{
public:
    PostfixExprMetaObject(unsigned int xDim, unsigned int yDim,
                unsigned int zDim, const QSharedPointer<PostfixExpr>
                &postfixExprPtr);
    PostfixExprMetaObject(unsigned int xDim, unsigned int yDim,
                unsigned int zDim, QBuffer *xmlData);

    virtual QByteArray XMLRepresentation();

    virtual float valueAtPoint(const Point& p);
    // Returns variables mamager without x, y, z
    virtual VariablesManager variablesManager();
    virtual QString description();
    virtual MetaObjectType type() { return EXPRESSION; }

    bool isValid() { return fIsValid; }

protected:
    virtual bool initWithXML(QBuffer *xmlData);
    bool setPostfixExpression(const QSharedPointer<PostfixExpr>
                &postfixExprPtr);

private:
    bool fIsValid;
    VariablesManager fVariablesManager;
    VariablesManager fUserVariablesManager;
    QSharedPointer<PostfixExpr> fPostfixExprPtr;
    // temporary values storage, exist just for better performance
    QMap<QString, double> fTemporaryVariables;

};

// Represent meta-object field potential values of which are defined by a
// hardcoded otimized expression. This approach gives maximal computation
// performance. Descendants could implement universal, usefull and oftenly
// used meta-objects.
class PredefinedMetaObject : public MetaObject
{
public:
    //PredefinedMetaObject() : MetaObject() {}
    virtual MetaObjectType type() { return PREDEFINED; }
private:
};

class CylinderMetaObject : public PredefinedMetaObject
{
public:
    //CylinderMetaObject() : PredefinedMetaObject() {}
    virtual float valueAtPoint(const Point& p);
    virtual QMap<QString, QSharedPointer<double> > variables();
private:
};

#endif // METAOBJECT_H
