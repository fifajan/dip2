/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * postfixexpr.h is part of 3D Meta-Object-based Modelling System            *
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

#ifndef POSTFIXEXPR_H
#define POSTFIXEXPR_H

#include <QStack>
#include <QList>
#include <QString>
#include <QSharedPointer>

#include "infixlex_types.h"
#include "variablesmanager.h"

class PostfixToken;
class PostfixOperatorToken;

// Represent matematical function expression written in Reverse Polish Notation
// (RPN). Can be created using regular infix expression. Can be executed over
// and over giving different result if any variables present in expression have
// changed their values. Retains sequence of tokens.
class PostfixExpr
{
public:
    PostfixExpr(const QString &infixString);
    ~PostfixExpr();

    bool successfullyParsed() { return fSuccessfullyParsed; }
    QString infixString();// { return fInfixString; }
    void postfixString() {}

    VariablesManager variablesManager();

    double execute();

protected:
    void parse(const QString &infixString);
    void freeTemporaries();
    PostfixToken *createToken(const Token &token);
    PostfixToken *createOperatorToken(const Token &token);
    PostfixToken *createVariableToken(const QString &name);
    PostfixToken *createFunctionToken(const QString &name);

    void makePostfix(Token *tokens, unsigned int tokenCount);

private: // data

    bool fSuccessfullyParsed;

    QString fInfixString;

    QList<PostfixToken *> fInfixOrderedTokens;
    QList<PostfixToken *> fTokens;
    QStack<PostfixToken *> fCalculationStack;
    QStack<PostfixOperatorToken *> fOperatorStack;
    VariablesManager fVariablesManager;
};

#endif // POSTFIXEXPR_H
