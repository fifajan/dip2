/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * postfixtoken.h is part of 3D Meta-Object-based Modelling System           *
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

#ifndef POSTFIXTOKEN_H
#define POSTFIXTOKEN_H

#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QStack>

#include "infixlex_types.h"

typedef enum
{
    LOWEST = 1,
    LOW,
    MIDDLE,
    HIGH
} OperatorPriority;

class PostfixOperatorToken;

// Represents a token (lexeme) of a postfix expression. Token itself defines
// how it should behave on phase of converting infix notation to postfix and
// on phase of execution.
class PostfixToken
{
public:
    PostfixToken() {}
    virtual ~PostfixToken() {}
    virtual void parseEffect(QStack<PostfixOperatorToken *> *stack,
                QList<PostfixToken *> *tokens) = 0;
    virtual void effect(QStack<PostfixToken *> *stack) = 0;
    virtual void freeTemporaies() {}
    virtual QString string() = 0;
};

class PostfixOperandToken : public PostfixToken
{
public:
    PostfixOperandToken() : PostfixToken() {}
    virtual ~PostfixOperandToken() {}
    virtual void parseEffect(QStack<PostfixOperatorToken *> *stack,
                QList<PostfixToken *> *tokens);
    virtual void effect(QStack<PostfixToken *> *stack);
    virtual double value() = 0;
    virtual QString string();
private:
};

class PostfixOperatorToken : public PostfixToken
{
public:
    PostfixOperatorToken() : PostfixToken(), fResult(0) {}
    virtual ~PostfixOperatorToken() { freeTemporaies(); }
    virtual void parseEffect(QStack<PostfixOperatorToken *> *stack,
                QList<PostfixToken *> *tokens);
    virtual void effect(QStack<PostfixToken *> *stack) {}
    virtual OperatorPriority priority() { return LOWEST; }
    virtual QString string() = 0;
    virtual void freeTemporaies() { delete fResult; fResult = 0; }
protected:
    PostfixToken *fResult;
};

class PostfixOpenBacketOperatorToken : public PostfixOperatorToken
{
public:
    virtual ~PostfixOpenBacketOperatorToken() {}
    virtual void parseEffect(QStack<PostfixOperatorToken *> *stack,
                QList<PostfixToken *> *tokens);
    virtual QString string();
};

class PostfixCloseBacketOperatorToken : public PostfixOperatorToken
{
public:
    virtual ~PostfixCloseBacketOperatorToken() {}
    virtual void parseEffect(QStack<PostfixOperatorToken *> *stack,
                QList<PostfixToken *> *tokens);
    virtual QString string();
};

class PostfixNumberToken : public PostfixOperandToken
{
public:
    virtual ~PostfixNumberToken() {}
    PostfixNumberToken(const Token& token);
    PostfixNumberToken(const double& number);
    virtual double value();
private:
    double fValue;
};

class PostfixVariableToken : public PostfixOperandToken
{
public:
    PostfixVariableToken(const QString &name,
                const QSharedPointer<double> &valuePtr);
    virtual ~PostfixVariableToken() {}
    virtual double value();
    virtual QString string();

private:
    QString fName;
    QSharedPointer<double> fValuePtr;
};

class PostfixUnaryOperatorToken : public PostfixOperatorToken
{
public:
    virtual ~PostfixUnaryOperatorToken() {}
    virtual void effect(QStack<PostfixToken *> *stack);
private:
    virtual double execute(double operand) = 0;
};

class PostfixBinaryOperatorToken : public PostfixOperatorToken
{
public:
    virtual ~PostfixBinaryOperatorToken() {}
    virtual void effect(QStack<PostfixToken *> *stack);
private:
    virtual double execute(double operand1, double operand2) = 0;
};

class PostfixUnaryMinusOperatorToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixUnaryMinusOperatorToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixSineFunctionToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixSineFunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixCosineFunctionToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixCosineFunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixArcCosineFunctionToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixArcCosineFunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixArcTangentFunctionToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixArcTangentFunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixSquareRootFunctionToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixSquareRootFunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixExponentFunctionToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixExponentFunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixAbsoluteFunctionToken : public PostfixUnaryOperatorToken
{
public:
    virtual ~PostfixAbsoluteFunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand);
};

class PostfixInvolutionOperatorToken : public PostfixBinaryOperatorToken
{
public:
    virtual ~PostfixInvolutionOperatorToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand1, double operand2);
};

class PostfixArcTangent2FunctionToken : public PostfixBinaryOperatorToken
{
public:
    virtual ~PostfixArcTangent2FunctionToken() {}
    virtual OperatorPriority priority() { return HIGH; }
    virtual QString string();
private:
    virtual double execute(double operand1, double operand2);
};

class PostfixMinusOperatorToken : public PostfixBinaryOperatorToken
{
public:
    virtual ~PostfixMinusOperatorToken() {}
    virtual OperatorPriority priority() { return LOW; }
    virtual QString string();
private:
    virtual double execute(double operand1, double operand2);
};

class PostfixPlusOperatorToken : public PostfixBinaryOperatorToken
{
public:
    virtual ~PostfixPlusOperatorToken() {}
    virtual OperatorPriority priority() { return LOW; }
    virtual QString string();
private:
    virtual double execute(double operand1, double operand2);
};

class PostfixMultiplyOperatorToken : public PostfixBinaryOperatorToken
{
public:
    virtual ~PostfixMultiplyOperatorToken() {}
    virtual OperatorPriority priority() { return MIDDLE; }
    virtual QString string();
private:
    virtual double execute(double operand1, double operand2);
};

class PostfixDivideOperatorToken : public PostfixBinaryOperatorToken
{
public:
    virtual ~PostfixDivideOperatorToken() {}
    virtual OperatorPriority priority() { return MIDDLE; }
    virtual QString string();
private:
    virtual double execute(double operand1, double operand2);
};

#endif // POSTFIXTOKEN_H
