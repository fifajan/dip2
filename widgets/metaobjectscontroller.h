/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * metaobjectscontroller.h is part of 3D Meta-Object-based Modelling System  *
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

#ifndef METAOBJECTSCONTROLLER_H
#define METAOBJECTSCONTROLLER_H

#include <QtGui/QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSharedPointer>

#include "ui_metaobjectscontroller.h"
#include "field.h"
#include "poligonizator.h"

class QByteArray;
class MetaObject;

class MetaObjectItem : public QListWidgetItem
{
public:
    MetaObjectItem(const QSharedPointer<MetaObject> &metaObjectPtr);
    virtual ~MetaObjectItem();

    QSharedPointer<MetaObject> metaObjectPtr() { return fMetaObjectPtr; }
    virtual QVariant data(int role);

private:
    QString fDescription;

    QSharedPointer<MetaObject> fMetaObjectPtr;
};

// Represent meta object controller widget. It controls the set of currently
// present meta object. It can select, add and remove meta-objects. Also
// it provides dynamic control widgets for variables present in selected
// meta-object.
class MetaObjectsController : public QWidget
{
    Q_OBJECT

public:
    MetaObjectsController(QWidget *parent = 0);
    void initWithXML(QBuffer *xmlData);

    QByteArray fieldXMLRepresentation() { return fField.XMLRepresentation(); }

    void initField();

signals:
    void trianglesChanged(const QSharedPointer<const QVector<TriangleN> > &);

public slots:
    void setIsoLevel(float value);

    void setGridSidesDimention(int value);
    void setGridXDimention(int value);
    void setGridYDimention(int value);
    void setGridZDimention(int value);

    void setFacesNormalMode(bool value);

    void enterFieldExpression(bool);
    void removeSelectedMetaObject(bool);

private slots:
    void variablesChanged(double);
    void updateVariableControls();
    void addMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr);
    void removeMetaObject(const QSharedPointer<MetaObject> &metaObjectPtr);

protected:
    void addMetaObjectItem(const QSharedPointer<MetaObject> &metaObjectPtr);

    void initConnections();
    void clearVariableControls();

private:
    Ui::MetaObjectsController fUI;

    Field fField;
    Poligonizator fPoligonizator;
};

#endif // METAOBJECTSCONTROLLER_H
