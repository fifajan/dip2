/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * variablesmanager.h is part of 3D Meta-Object-based Modelling Syste        *
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

#ifndef VARIABLESMANAGER_H
#define VARIABLESMANAGER_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>

class QBuffer;
class QByteArray;
class QStringList;

// Represents variables value wrapper. Needed to connect Qt signals to wrapper's
// slots to dinamically change variable value.
class VariableValueWrapper : public QObject
{
    Q_OBJECT

public:
    VariableValueWrapper();
    VariableValueWrapper(const VariableValueWrapper &copyee);
    VariableValueWrapper(double value);
    VariableValueWrapper(const QSharedPointer<double> &valuePtr);

    double value();
    QSharedPointer<double> valuePtr();

signals:
    void valueChanged(double value);

public slots:
    void setValue(double value);
    void setValuePtr(const QSharedPointer<double> &valuePtr);

private:
    QSharedPointer<double> fValuePtr;
};

// Represents variables manager which supports convinient ways to get and set
// variable values.
class VariablesManager
{
public:
    VariablesManager() {}
    VariablesManager(const VariablesManager &copyee);

    bool setVariableValue(const QString &name, double value);
    int setVariableValues(const QMap<QString, double> &values);
    int setVariableValuesWithXML(QBuffer *xmlData);

    QByteArray XMLRepresentation();

    double variableValue(const QString &name);
    QSharedPointer<double> variableValuePtr(const QString &name);
    QSharedPointer<VariableValueWrapper> variableValueWrapper(
                const QString &name);

    QStringList variableNames();

    bool containsVariable(const QString &name);

    bool addVariable(const QString &name, double value);
    bool addVariable(const QString &name,
                const QSharedPointer<double> &valuePtr);

    bool removeVariable(const QString &name);

private:
    QMap<QString, QSharedPointer<VariableValueWrapper> > fVariables;
};

#endif // VARIABLESMANAGER_H
