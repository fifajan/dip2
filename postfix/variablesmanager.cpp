/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * variablesmanager.cpp is part of 3D Meta-Object-based Modelling Syste      *
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

#include <QString>
#include <QStringList>
#include <QDebug>
#include <QXmlQuery>
#include <QXmlItem>
#include <QXmlSerializer>
#include <QBuffer>
#include <QXmlResultItems>

#include "variablesmanager.h"

VariableValueWrapper::VariableValueWrapper() : QObject(), fValuePtr(new double)
{
}

VariableValueWrapper::VariableValueWrapper(double value) : QObject(),
            fValuePtr(new double)
{
    setValue(value);
}

VariableValueWrapper::VariableValueWrapper(const QSharedPointer<double>
            &valuePtr) : QObject(), fValuePtr(new double)
{
    setValuePtr(valuePtr);
}

double VariableValueWrapper::value()
{
    return *(fValuePtr.data());
}

QSharedPointer<double> VariableValueWrapper::valuePtr()
{
    return fValuePtr;
}

void VariableValueWrapper::setValue(double value)
{
    *(fValuePtr.data()) = value;
    emit valueChanged(value);
}

void VariableValueWrapper::setValuePtr(const QSharedPointer<double> &valuePtr)
{
    fValuePtr = valuePtr;
}

VariablesManager::VariablesManager(const VariablesManager &copyee)
{
    fVariables = copyee.fVariables;
}

bool VariablesManager::setVariableValue(const QString &name, double value)
{
    bool result = false;
    QMap<QString, QSharedPointer<VariableValueWrapper> >::const_iterator
                varIt(fVariables.find(name));
    if (varIt != fVariables.end())
    {
        varIt.value()->setValue(value);
        result = true;
    }

    return result;
}

int VariablesManager::setVariableValues(const QMap<QString, double> &values)
{
    int valuesSetCount = 0;

    QMap<QString, double >::const_iterator valueIt;
    for (valueIt = values.begin(); valueIt != values.end(); valueIt++)
    {
        valuesSetCount += setVariableValue(valueIt.key(), valueIt.value());
    }

    return valuesSetCount;
}

int VariablesManager::setVariableValuesWithXML(QBuffer *xmlData)
{
    xmlData->open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.bindVariable("variables", xmlData);

    QXmlResultItems resultItems;
    query.setQuery("fn:doc($variables)//var/fn:data((@name, @value))");
    query.evaluateTo(&resultItems);
    xmlData->close();

    QMap<QString, double> variables;
    int variableCount = 0;
    bool isVarName = true;
    QString varName;
    double varValue = 0.0;
    for (QXmlItem item = resultItems.next(); !item.isNull();
                item = resultItems.next(), isVarName = !isVarName)
    {
        if (isVarName)
        {
            varName = item.toAtomicValue().toString();
        }
        else // it's variable's value
        {
            varValue = item.toAtomicValue().toDouble();
            variables[varName] = varValue;
            variableCount++;
        }
    }

    int result = setVariableValues(variables);
    qDebug() << "Successfully set" << result << "of" << variableCount
                << "specified variables.";

    return result;
}

QByteArray VariablesManager::XMLRepresentation()
{
    QByteArray variablesXMLData;
    variablesXMLData.append("<variables>");

    QStringList varNames = variableNames();
    QString varName;
    double varValue = 0.0;
    char varFormat[] = "<var name=\"%s\" value=\"%6.6f\"/>";
    QByteArray varXMLData;
    int variableCount = varNames.count();
    for (int i = 0; i < variableCount; i++)
    {
        varName = varNames[i];
        varValue = variableValue(varName);
        varXMLData = QString().sprintf(varFormat,
                    varName.toAscii().data(), varValue).toAscii();
        variablesXMLData.append(varXMLData);
    }

    variablesXMLData.append("</variables>");

    return variablesXMLData;
}

double VariablesManager::variableValue(const QString &name)
{
    return containsVariable(name) ? fVariables[name]->value() : 0.0;
}

QSharedPointer<double> VariablesManager::variableValuePtr(const QString &name)
{
    return containsVariable(name) ? fVariables[name]->valuePtr()
                : QSharedPointer<double>(0);
}

QSharedPointer<VariableValueWrapper> VariablesManager::variableValueWrapper(
            const QString &name)
{
    return containsVariable(name) ? fVariables[name]
                : QSharedPointer<VariableValueWrapper>(0);
}

QStringList VariablesManager::variableNames()
{
    return fVariables.keys();
}

bool VariablesManager::containsVariable(const QString &name)
{
    return fVariables.contains(name);
}

bool VariablesManager::addVariable(const QString &name, double value)
{
    QSharedPointer<VariableValueWrapper> valueWrapperPtr(
                new VariableValueWrapper(value));
    QMap<QString, QSharedPointer<VariableValueWrapper> >::const_iterator
            varIt(fVariables.insert(name, valueWrapperPtr));

    return varIt != fVariables.end();
}

bool VariablesManager::addVariable(const QString &name,
            const QSharedPointer<double> &valuePtr)
{
    QSharedPointer<VariableValueWrapper> valueWrapperPtr(
                new VariableValueWrapper(valuePtr));
    QMap<QString, QSharedPointer<VariableValueWrapper> >::const_iterator
            varIt(fVariables.insert(name, valueWrapperPtr));

    return varIt != fVariables.end();
}

bool VariablesManager::removeVariable(const QString &name)
{
    return fVariables.remove(name);
}
