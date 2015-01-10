/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * postfixexpr.cpp is part of 3D Meta-Object-based Modelling System          *
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
#include <limits>

#include <QRegExp>
#include <QDebug>

#include "infixlex_types.h"
#include "postfixtoken.h"
#include "postfixexpr.h"

const int kExecutionStackSize = 120;

const char kFunctionSin[] = "sin";
const char kFunctionCos[] = "cos";
const char kFunctionArcCos[] = "arccos";
const char kFunctionArcTg[] = "arctg";
const char kFunctionSqrt[] = "sqrt";
const char kFunctionExp[] = "exp";
const char kFunctionAbs[] = "abs";

namespace Infix
{
    extern "C" int parse(char *string, Token **outTokens, char **errString);
}

PostfixExpr::PostfixExpr(const QString &infixString)
            : fSuccessfullyParsed(false)
{
    // probably will speed up execution
    fCalculationStack.reserve(kExecutionStackSize);

    parse(infixString);
}

PostfixExpr::~PostfixExpr()
{
    int tokenCount = fInfixOrderedTokens.count();
    for (int i = 0; i < tokenCount; i++)
    {
        delete fInfixOrderedTokens[i];
    }
}

QString PostfixExpr::infixString()
{
    fInfixString.clear();

    unsigned int tokensCount = fInfixOrderedTokens.count();
    for (unsigned int i = 0; i < tokensCount; i++)
    {
        PostfixToken *tokenPtr = fInfixOrderedTokens[i];

        // if it is a variable we trim it's numeric value.
        QString tokenString(tokenPtr->string().remove(QRegExp("=.*$")));

        fInfixString.append(tokenString);
    }

    return fInfixString;
}

VariablesManager PostfixExpr::variablesManager()
{
    return fVariablesManager;
}

double PostfixExpr::execute()
{
    double result = 0.0;

    unsigned int tokensCount = fTokens.count();

    if (tokensCount > 0)
    {
        for (unsigned int i = 0; i < tokensCount; i++)
        {
            fTokens[i]->effect(&fCalculationStack);
        }
        result = (static_cast<PostfixOperandToken *>(
                    fCalculationStack.pop()))->value();

        fCalculationStack.clear();
        fCalculationStack.reserve(kExecutionStackSize);
        freeTemporaries();
    }
    else
    {
        result = std::numeric_limits<double>::min(); // error
    }

    return result;
}

void PostfixExpr::parse(const QString &infixString)
{
    Token *tokens = 0;
    char *errorString = 0;
    int tokenCount = Infix::parse(infixString.toAscii().data(),
                &tokens, &errorString);
    if (tokenCount > 0)
    {
        makePostfix(tokens, tokenCount);
        fSuccessfullyParsed = true;
    }
    else // error, writting message to fInfixString
    {
        fInfixString = errorString;
    }
}

void PostfixExpr::freeTemporaries()
{
    int tokenCount = fTokens.count();
    for (int i = 0; i < tokenCount; i++)
    {
        fTokens[i]->freeTemporaies();
    }
}

PostfixToken* PostfixExpr::createToken(const Token& token)
{
    PostfixToken *result = NULL;
    switch (token.type)
    {
        case NUMBER:
        {
            result = new PostfixNumberToken(token);
            break;
        }
        case VARIABLE:
        {
            QString variableName(token.variableName);
            // check if it's not a function name
            result = createFunctionToken(variableName);
            if (!result) // no, it's a variable
            {
                result = createVariableToken(variableName);
            }
            break;
        }
        case OPERATOR:
        {
            result = createOperatorToken(token);
            break;
        }
        default: // should never get here
        {
            break;
        }
    }

    Q_CHECK_PTR(result);
    return result;
}

PostfixToken *PostfixExpr::createOperatorToken(const Token &token)
{
    PostfixToken *result = NULL;
    switch (token.oper)
    {
        case '^': // involution
        {
            result = new PostfixInvolutionOperatorToken();
            break;
        }
        case '@': // atan2
        {
            result = new PostfixArcTangent2FunctionToken();
            break;
        }
        case '+':
        {
            result = new PostfixPlusOperatorToken();
            break;
        }
        case '-':
        {
            result = new PostfixMinusOperatorToken();
            break;
        }
        case '*':
        {
            result = new PostfixMultiplyOperatorToken();
            break;
        }
        case '/':
        {
            result = new PostfixDivideOperatorToken();
            break;
        }
        case '$': // unary -
        {
            result = new PostfixUnaryMinusOperatorToken();
            break;
        }
        case '(':
        {
            result = new PostfixOpenBacketOperatorToken();
            break;
        }
        case ')':
        {
            result = new PostfixCloseBacketOperatorToken();
            break;
        }
        default: // should never get here
        {
            break;
        }
    }
    return result;
}

PostfixToken *PostfixExpr::createVariableToken(const QString &name)
{
    if (!fVariablesManager.containsVariable(name))
    {
        fVariablesManager.addVariable(name, 0.0);
    }

    return new PostfixVariableToken(name,
                fVariablesManager.variableValuePtr(name));

}

PostfixToken *PostfixExpr::createFunctionToken(const QString &name)
{
    PostfixToken *result = NULL;
    if (kFunctionSin == name)
    {
        result = new PostfixSineFunctionToken();
    }
    else if (kFunctionCos == name)
    {
        result = new PostfixCosineFunctionToken();
    }
    else if (kFunctionArcCos == name)
    {
        result = new PostfixArcCosineFunctionToken();
    }
    else if (kFunctionArcTg == name)
    {
        result = new PostfixArcTangentFunctionToken();
    }
    else if (kFunctionSqrt == name)
    {
        result = new PostfixSquareRootFunctionToken();
    }
    else if (kFunctionExp == name)
    {
        result = new PostfixExponentFunctionToken();
    }
    else if (kFunctionAbs == name)
    {
        result = new PostfixAbsoluteFunctionToken();
    }
    return result;
}

void PostfixExpr::makePostfix(Token *tokens, unsigned int tokenCount)
{
    unsigned int i = 0;
    for (; i < tokenCount; i++)
    {
        Token token = tokens[i];
        PostfixToken *tokenPtr = createToken(token);

        fInfixOrderedTokens.append(tokenPtr);

        tokenPtr->parseEffect(&fOperatorStack, &fTokens);
    }
    while (!fOperatorStack.empty())
    {
        fTokens.append(fOperatorStack.pop());
    }
    fOperatorStack.clear();
}
