/**
 * This the example lexical analyzer code in pages 173 - 177 of the
 * textbook,
 *
 * Sebesta, R. W. (2012). Concepts of Programming Languages.
 * Pearson, 10th edition.
 *
 */

 /* front.c - a lexical analyzer system for simple arithmetic expressions */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#pragma warning(disable: 4996)

/* Global Variable */
int nextToken;
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;

FILE* in_fp;
FILE* out_fp;

/* Local Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define FOR 12
#define IF 13
#define ELSE 14
#define WHILE 15
#define DO 16
#define INT 17
#define FLOAT 18
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define SEMI_COLON 27
#define LT_OP 28
#define GT_OP 29
#define LEFT_BR 30
#define RIGHT_BR 31
#define ENTER 00




float calc(int token) {//사칙연산
	float result = 0;
	int op = token;
	switch (op) {
	case ADD_OP:
		token = lex();
		while (token != RIGHT_PAREN) {
			if (token == INT_LIT) {
				result += atof(lexeme);
				token = lex();
			}
			else if (token == LEFT_PAREN) {//안에 괄호 있을 때
				token = lex();
				result += calc(token);
				token = lex();
			}
		}
		break;
	case MULT_OP:
		result = 1;
		token = lex();
		while (token != RIGHT_PAREN) {
			if (token == INT_LIT) {
				result *= atof(lexeme);
				token = lex();
			}
			else if (token == LEFT_PAREN) {
				token = lex();
				result *= calc(token);
				token = lex();
			}
		}
		break;
	case DIV_OP:
		token = lex();
		result = atof(lexeme);
		token = lex();
		while (token != RIGHT_PAREN) {
			if (token == INT_LIT) {
				result /= atof(lexeme);
				token = lex();
			}
			else if (token == LEFT_PAREN) {
				token = lex();
				result /= calc(token);
				token = lex();
			}
		}
		break;
	case SUB_OP:
		token = lex();
		result = atof(lexeme);
		token = lex();
		while (token != RIGHT_PAREN) {
			if (token == INT_LIT) {
				result -= atof(lexeme);
				token = lex();
			}
			else if (token == LEFT_PAREN) {
				token = lex();
				result -= calc(token);
				token = lex();
			}
		}
		break;
	}
	return result;
}

/******************************************
 * lookup - a function to lookup operators
 * and parentheses and return the token
 ******************************************/
int lookup(char ch) {
	switch (ch) {
	case '(':
		addChar();
		nextToken = LEFT_PAREN;
		break;
	case ')':
		addChar();
		nextToken = RIGHT_PAREN;
		break;
	case '+':
		addChar();
		nextToken = ADD_OP;
		break;
	case '-':
		addChar();
		nextToken = SUB_OP;
		break;
	case '*':
		addChar();
		nextToken = MULT_OP;
		break;
	case '/':
		addChar();
		nextToken = DIV_OP;
		break;
	case '=':
		addChar();
		nextToken = ASSIGN_OP;
		break;
	case ';':
		addChar();
		nextToken = SEMI_COLON;
		break;
	case '<':
		addChar();
		nextToken = LT_OP;
		break;
	case '>':
		addChar();
		nextToken = GT_OP;
		break;
	case '{':
		addChar();
		nextToken = LEFT_BR;
		break;
	case '}':
		addChar();
		nextToken = RIGHT_BR;
		break;
	default:
		addChar();
		nextToken = EOF;
		break;
	}
	return nextToken;
}

/**************************************************/
/* addChar - a function to add nextChar to lexeme */
/**************************************************/
void addChar() {
	if (lexLen <= 98) {  // max length of Lexime is 99
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = 0; // '\0'
	}
	else {
		printf("Error - lexeme is too long \n");
	}
}

/*****************************************************/
/* getChar - a function to get the next character
		  of input and determine its character class */
		  /*****************************************************/
void getChar() {
	if ((nextChar = getc(stdin)) != EOF) {
		if (isalpha(nextChar))
			charClass = LETTER;
		else if (isdigit(nextChar))
			charClass = DIGIT;
		else
			charClass = UNKNOWN;
	}
	else {
		charClass = EOF;
	}
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it
		   returns a non-whitespace character        */
		   /*****************************************************/
void getNonBlank() {
	while (isspace(nextChar))
		getChar();
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
		 expressions                                 */
		 /*****************************************************/
int lex() {
	lexLen = 0;
	getNonBlank();

	switch (charClass) {
		/* Parse identifiers */
	case LETTER:
		addChar();
		getChar();
		while (charClass == LETTER || charClass == DIGIT) {
			addChar();
			getChar();
		}
		if (strcmp(lexeme, "for") == 0) {
			nextToken = FOR;
		}
		else if (strcmp(lexeme, "if") == 0) {
			nextToken = IF;
		}
		else if (strcmp(lexeme, "else") == 0) {
			nextToken = ELSE;
		}
		else if (strcmp(lexeme, "while") == 0) {
			nextToken = WHILE;
		}
		else if (strcmp(lexeme, "do") == 0) {
			nextToken = DO;
		}
		else if (strcmp(lexeme, "int") == 0) {
			nextToken = INT;
		}
		else if (strcmp(lexeme, "float") == 0) {
			nextToken = FLOAT;
		}
		else {
			nextToken = IDENT;
		}
		break;

		/* Parse integer literals */
	case DIGIT:
		addChar();
		getChar();
		while (charClass == DIGIT) {
			addChar();
			getChar();
		}
		nextToken = INT_LIT;
		break;

		/* Parentheses and operators */
	case UNKNOWN:
		lookup(nextChar);
		getChar();
		break;

		/* EOF */
	case EOF:
		nextToken = EOF;
		lexeme[0] = 'E';
		lexeme[1] = 'O';
		lexeme[2] = 'F';
		lexeme[3] = 0;
		break;
	} /* End of switch */
	if ((out_fp = fopen("code.out", "a")) == NULL) {
		printf("ERROR - cannot open front.in \n");
	}
	else {
		//printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
	}
	/* Write to code.out */
	return nextToken;
} /* End of function lex */



/******************************************/
/* main driver                            */
/******************************************/
int main()
{
	getChar();
	do {
		int token = lex();
		if (token == LEFT_PAREN) {
			token = lex();
			printf("%f\n", calc(token));
		}
	} while (nextToken != EOF);

	return 0;
}