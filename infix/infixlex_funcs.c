/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * infixlex_funcs.c is part of 3D Meta-Object-based Modelling Syste          *
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

/*
void printTokens()
{
    printf("Num of tokens = %i\n", tokenCount);
    int i = 0;
    for(; i < tokenCount; i++)
    {
        Token token = tokens[i];
        switch (token.type)
        {
            case OPERATOR:
                printf("OP[%c]_", token.oper);
            break;

            case VARIABLE:
                printf("VAR[%s]_", token.variableName);
            break;

            case NUMBER:
                printf("NUM[%4.4f]_", token.number);
            break;
        }
    }
    putchar('\n');
}
*/

/* Called by yyparse on error.  */
void yyerror (char const *s)
{
    sprintf(errorMsgBuffer, "%s", s);
	errorFlag = 1;
}

int parse(char *string, Token **outTokens, char **errString)
{
	int result = 0;

	errorFlag = 0;
	input = NULL;
	tokenCount = 0;
	input = string;
    yyparse();
	if (errorFlag)
	{
		result = -1;
		*errString = errorMsgBuffer;
	}
	else
	{
    	*outTokens = tokens;
		result = tokenCount;
	}

    return result;
}


int yylex (void)
{
    int c;

    /* Ignore white space, get first nonwhite character. */
	while ((c = *input++) == ' ' || c == '\t')
	{
		;
	}

    /* end of parse string */
    if (c == '\0')
    {
        return 0;
    }

    /* Char starts a number => parse the number. */
	int dotFlag = 0;
    if ((dotFlag = (c == '.')) || isdigit (c))
    {
		input;
        double value;
        sscanf (input - 1, "%lf", &value);
		/* skip digits we have just read */
		while ( isdigit((c = *input++)) || (!dotFlag && (dotFlag = (c == '.'))) )
		{
			;
		}
		input--;

        Token token = { NUMBER, value, "", ' ' };
        tokens[tokenCount] = token;
        tokenCount++;
        return NUM;
    }

    /* Char starts an identifier => read the name. */
    if (isalpha (c))
    {
        static char *symbuf = 0;
        static int length = 0;
        int i;

        /* Initially make the buffer long enough
            for a 40-character symbol name.  */
        if (length == 0)
        {
            length = 40;
            symbuf = (char *) malloc (length + 1);
        }

        i = 0;
        do
        {
            /* If buffer is full, make it bigger. */
            if (i == length)
            {
                length *= 2;
                symbuf = (char *) realloc (symbuf, length + 1);
            }
            /* Add this character to the buffer. */
            symbuf[i++] = c;
            /* Get another character. */
			c = *input++;
        }
        while (isalnum (c));

		input--;
        symbuf[i] = '\0';
        Token token = { VARIABLE, 0.0, "", ' ' };
        strcpy(token.variableName, symbuf);

        tokens[tokenCount] = token;
        tokenCount++;

        return VAR;
    }

    /* Any other character is a token by itself. */
    Token token = { OPERATOR, 0.0, "", c };
    tokens[tokenCount] = token;
    yylval = &(tokens[tokenCount]);
    tokenCount++;

    return c;
}

/*
int main(int argc, char *argv[])
{
	Token *t = NULL;
	parse(argv[1], &t);
	return 0;
}
*/
