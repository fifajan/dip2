/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * postfixtoken.cpp is part of 3D Meta-Object-based Modelling System         *
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QDebug>

#include "postfixtoken.h"

void PostfixOperandToken::parseEffect(QStack<PostfixOperatorToken *> *stack,
            QList<PostfixToken *> *tokens)
{
    tokens->append(this);
}

void PostfixOperandToken::effect(QStack<PostfixToken *> *stack)
{
    stack->push(this);
}

void PostfixOperatorToken::parseEffect(QStack<PostfixOperatorToken *> *stack,
            QList<PostfixToken *> *tokens)
{
    while(!stack->empty() && (this->priority() <= stack->top()->priority()))
    {
        tokens->append(stack->pop());
    }
    stack->push(this);
}

QString PostfixOperandToken::string()
{
    return QString().sprintf("%3.3f", value());
}

void PostfixOpenBacketOperatorToken::parseEffect(QStack<PostfixOperatorToken *>
            *stack, QList<PostfixToken *> *tokens)
{
    stack->push(this);
}

QString PostfixOpenBacketOperatorToken::string()
{
    return QString("(");
}

void PostfixCloseBacketOperatorToken::parseEffect(QStack<PostfixOperatorToken *>
            *stack, QList<PostfixToken *> *tokens)
{
    while(!stack->empty() && (LOWEST != stack->top()->priority()))
    {
        tokens->append(stack->pop());
    }
    stack->pop();
}

QString PostfixCloseBacketOperatorToken::string()
{
    return QString(")");
}

PostfixNumberToken::PostfixNumberToken(const Token& token)
            : PostfixOperandToken()
{
    fValue = token.number;
}

PostfixNumberToken::PostfixNumberToken(const double& number)
            : PostfixOperandToken()
{
    fValue = number;
}

double PostfixNumberToken::value()
{
    return fValue;
}

PostfixVariableToken::PostfixVariableToken(const QString &name,
            const QSharedPointer<double> &valuePtr) : PostfixOperandToken()
{
    fName = name;
    fValuePtr = valuePtr;
}

double PostfixVariableToken::value()
{
    return *(fValuePtr.data());
}

QString PostfixVariableToken::string()
{
    double value = this->value();
    bool minus = (value < 0.0);

    return QString().sprintf("%s=%s%3.3f%s",
                fName.toAscii().data(), (minus ? "(" : ""),
                value, (minus ? ")" : ""));
}

void PostfixUnaryOperatorToken::effect(QStack<PostfixToken *> *stack)
{
    PostfixOperandToken* operand =
                static_cast<PostfixOperandToken *>(stack->pop());
    fResult = new PostfixNumberToken(execute(operand->value()));
    stack->push(fResult);
}

void PostfixBinaryOperatorToken::effect(QStack<PostfixToken *> *stack)
{
    PostfixOperandToken* operand2 =
                static_cast<PostfixOperandToken *>(stack->pop());
    PostfixOperandToken* operand1 =
                static_cast<PostfixOperandToken *>(stack->pop());
    fResult = new PostfixNumberToken(
                execute(operand1->value(), operand2->value()));
    stack->push(fResult);
}

double PostfixUnaryMinusOperatorToken::execute(double operand)
{
    return -operand;
}

QString PostfixUnaryMinusOperatorToken::string()
{
    return QString("-");
}

double PostfixSineFunctionToken::execute(double operand)
{
    return sin(operand);
}

QString PostfixSineFunctionToken::string()
{
    return QString("sin");
}

double PostfixCosineFunctionToken::execute(double operand)
{
    return cos(operand);
}

QString PostfixCosineFunctionToken::string()
{
    return QString("cos");
}

double PostfixArcCosineFunctionToken::execute(double operand)
{
    return acos(operand);
}

QString PostfixArcCosineFunctionToken::string()
{
    return QString("arccos");
}

double PostfixArcTangentFunctionToken::execute(double operand)
{
    return atan(operand);
}

QString PostfixArcTangentFunctionToken::string()
{
    return QString("arctg");
}

double PostfixSquareRootFunctionToken::execute(double operand)
{
    return sqrt(operand);
}

QString PostfixSquareRootFunctionToken::string()
{
    return QString("sqrt");
}

double PostfixExponentFunctionToken::execute(double operand)
{
    return exp(operand);
}

QString PostfixExponentFunctionToken::string()
{
    return QString("exp");
}

double PostfixAbsoluteFunctionToken::execute(double operand)
{
    return fabs(operand);
}

QString PostfixAbsoluteFunctionToken::string()
{
    return QString("abs");
}

double PostfixInvolutionOperatorToken::execute(double operand1, double operand2)
{
    return pow(operand1, operand2);
}

QString PostfixInvolutionOperatorToken::string()
{
    return QString("^");
}

double PostfixArcTangent2FunctionToken::execute(double operand1,
            double operand2)
{
    return atan2f(operand1, operand2);
}

QString PostfixArcTangent2FunctionToken::string()
{
    return QString("_atan2_");
}

double PostfixMinusOperatorToken::execute(double operand1, double operand2)
{
    return operand1 - operand2;
}

QString PostfixMinusOperatorToken::string()
{
    return QString("-");
}

double PostfixPlusOperatorToken::execute(double operand1, double operand2)
{
    return operand1 + operand2;
}

QString PostfixPlusOperatorToken::string()
{
    return QString("+");
}

double PostfixMultiplyOperatorToken::execute(double operand1, double operand2)
{
    return operand1 * operand2;
}

QString PostfixMultiplyOperatorToken::string()
{
    return QString("*");
}

double PostfixDivideOperatorToken::execute(double operand1, double operand2)
{
    return operand1 / operand2;
}

QString PostfixDivideOperatorToken::string()
{
    return QString("/");
}
