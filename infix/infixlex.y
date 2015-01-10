/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * infixlex.y is part of 3D Meta-Object-based Modelling Syste                *
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

%{
#include <math.h>  /* For math functions, cos(), sin(), etc.  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "infixlex_types.h"

#define YYSTYPE Token*

Token tokens[1024];
unsigned int tokenCount = 0;

char *input = NULL;

char errorFlag = 0;
char errorMsgBuffer[256];

int yylex (void);
void yyerror (char const *);
//void printTokens();

//returns -1 if error ocured, errox message will be in errString
int parse(char *string, Token **outTokens, char **errString);

%}
%token NUM        /* Simple double precision number.  */
%token VAR FNCT   /* Variable and Function.  */

%left '-' '+'
%left '*' '/'
%left NEG     /* negation--unary minus */
%right '^'    /* exponentiation */
%right '@'	  /* atan2 */
%% /* The grammar follows.  */

line: 	  exp 	{ /*printTokens();*/ }

exp:      NUM                { ; }
        | VAR                { ; }
        | VAR '(' exp ')'    { ; }
        | exp '+' exp        { ; }
        | exp '-' exp        { ; }
        | exp '*' exp        { ; }
        | exp '/' exp        { ; }
        | '-' exp  %prec NEG { $1->oper = '$'; }
        | exp '^' exp        { ; }
        | exp '@' exp        { ; }
        | '(' exp ')'        { ; }
;

/* End of grammar.  */
%%

#include "infixlex_funcs.c"
