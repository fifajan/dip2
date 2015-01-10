/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * main.cpp is part of 3D Meta-Object-based Modelling System                 *
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

#include <iostream>
#include <sys/time.h>

#include <QMap>
#include <QXmlQuery>
#include <QXmlItem>
#include <QXmlResultItems>
#include <QXmlNodeModelIndex>
#include <QXmlSerializer>
#include <QAbstractXmlNodeModel>
#include <QString>
#include <QByteArray>
#include <QBuffer>
#include <QFile>
#include <QIODevice>
#include <QDebug>

#include "mainwindow.h"
#include "postfixexpr.h"
#include "metaobject.h"
#include "metaobjectscontroller.h"

double getSecondsPassed()
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    return 0.000000001 * ts.tv_nsec + ts.tv_sec;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    double seconds = getSecondsPassed();
    MainWindow *window = new MainWindow();
    window->show();
    seconds = getSecondsPassed() - seconds;
    std::cout << "Init duration = " << seconds << std::endl;
    return app.exec();

    ////////// Performance tests: ///////////
    /*
    if (argc == 2)
    {
        double seconds = getSecondsPassed();
        QApplication app(argc, argv);

        QFile xmlData(argv[1]);

        QByteArray fieldXMLData;
        QBuffer fieldXMLBuffer(&fieldXMLData);
        fieldXMLBuffer.open(QIODevice::WriteOnly);

        QXmlQuery query;
        QXmlSerializer fieldSerializer(query, &fieldXMLBuffer);

        xmlData.open(QIODevice::ReadOnly);
        query.bindVariable("document", &xmlData);

        query.setQuery("fn:doc($document)/document/field");
        query.evaluateTo(&fieldSerializer);
        xmlData.close();

        MetaObjectsController controller;
        controller.initWithXML(&fieldXMLBuffer);

        seconds = getSecondsPassed() - seconds;
        std::cout << "Init duration = " << seconds << std::endl;
    }
    else
    {
        std::cout << "Usage: ./dip2 benchmark.mox" << std::endl;
    }
    return 0;
    */
    ////////// XML tests: ///////////

    //QApplication app(argc, argv);

    /*
    QByteArray fieldXMLData;
    QBuffer fieldBuffer(&fieldXMLData); // This is a QIODevice.
    fieldBuffer.open(QIODevice::WriteOnly);

    QXmlQuery query;
    query.setQuery("doc('xml/doc.xml')/document/field");

    QXmlSerializer fieldSerializer(query, &fieldBuffer);
    query.evaluateTo(&fieldSerializer);
    fieldBuffer.close();

    Field field(&fieldBuffer);
    */

    /*
    QByteArray metaObjectXMLData;
    QBuffer metaObjectBuffer(&metaObjectXMLData); // This is a QIODevice.
    metaObjectBuffer.open(QIODevice::WriteOnly);

    QXmlQuery query;
    query.setQuery("doc('xml/doc.xml')/document/field/meta-object[3]");

    QXmlSerializer metaObjectSerializer(query, &metaObjectBuffer);
    query.evaluateTo(&metaObjectSerializer);
    metaObjectBuffer.close();

    PostfixExprMetaObject metaObject(&metaObjectBuffer);
    */

    //return 0;//app.exec();

    ////////// RPN tests: ///////////

    /*
    PostfixExpr expr("x+y+z-sin(x*y)+(x-y)^3+cos(x*y*z)");
    printf("Expr was%s parsed successfully!\n",
                expr.successfullyParsed() ? "" : " NOT");

    //printf("Filtered exp = '%s'; res = %4.4f\n",
    //            expr.infixString().toAscii().data(), expr.execute());

    QMap<QString, double> vars;
    vars.insert("x", 1.0);
    vars.insert("y", 1.5);
    vars.insert("z", -3.0);

    expr.variablesManager().setVariableValues(vars);

    printf("Filtered exp = '%s'; res = %4.4f\n",
        expr.infixString().toAscii().data(), expr.execute());

    return 0;
    */
}
