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
#include <iostream>
#include <map>
#pragma warning(disable: 4996)
using namespace std;

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
#define SYMBOL 11
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
#define LIST 40
#define ENTER 00

#define SETQ 50 //SETQ token number

//multiple variable을 넣을 수 있는 map 구현
//string 과 map 을 사용하였고 symbol과 그 symbol에 저장될 SETQval로 구성됨.
//SETQval은 실제 value의 값과 그 value의 자료형을 각각 string과 int로 저장함.
typedef struct SETQval {
	int val_type;
	string val;
}SETQval;

map<string, SETQval> symbols;


string FindSymbol(string findsym, int yongdo);

float calc(int token) {//사칙연산
	float result = 0;
	int op = token;
	switch (op) {
	case ADD_OP:
		token = lex();
		while (token != RIGHT_PAREN) {
			if (token == INT_LIT) {//숫자일때
				result += atof(lexeme);
				token = lex();
			}
			else if (token == SYMBOL) { //symbol일때: 이미 존재하는 symbol이고, 그 값이 숫자일때
				if (symbols.find(lexeme) != symbols.end()) {//이미 존재하는 symbol일 때
					if (symbols.find(lexeme)->second.val_type == INT_LIT) { //그 symbol의 값이 숫자일때
						result += atof(lexeme);
						token = lex();
					}
					
				}
				
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
	default:
		return -1;
	}
	return result;
}

int eval(int token) {
	if (token == ADD_OP || token == SUB_OP || token == MULT_OP || token == DIV_OP) { //사칙연산
		cout << ">" << calc(token) << endl;
	}
	else if (token == SETQ) { //SETQ
		token = lex();
		if (token == SYMBOL) {
			string s(lexeme);//symbol string화하기
			token = lex();
			string v(lexeme);//value string화하기

			if (v == ")") {
				cout << "Syntax Error" << endl;
				return 0;
			}

			if (v[0] == '\'') { //리스트를 setq할때 
				token = lex(); // 현재 왼쪽 괄호
				string list;
				token = lex(); //원소 찾기
				list.append(lexeme);
				token = lex();
				while (token != RIGHT_PAREN) {
						list.append(" ");
						list.append(lexeme);
						token = lex();
				}
				SETQval newval;
				newval.val = list;
				newval.val_type = LIST;
				symbols.insert(make_pair(s, newval)); //값 저장하기
				token = lex(); //닫는 괄호 처리하기
				cout << ">" << "(" << list << ")" << endl;

			}
			else { //리스트 아닌걸 SETQ 할떄
				SETQval newval;
				newval.val = v;
				newval.val_type = token;
				symbols.insert(make_pair(s, newval));//값 저장하기
				token = lex(); // 닫는 괄호 처리하기
				cout << ">" << v << endl;
			}
			
		}
	}
	else if (token == LIST) { //LIST
		string list;
		token = lex();
		while (token != RIGHT_PAREN) {
			string v(lexeme);
			//cout << "확인용" << v << endl;
			if (v[0] == '\'') { //심볼이 아닌 경우
				token = lex();
				string a(lexeme);
				list.append(a);
			}
			else {//심볼인 경우
				string symbol(lexeme);
				string v = FindSymbol(symbol, 2);
				list.append(v);
			}
			token = lex();
			if (token != RIGHT_PAREN)
				list.append(" ");
		}
		cout << list << endl;
	}
	else { //여기에 계속 다른 연산 추가

	}
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
		else if (strcmp(lexeme, "SETQ") == 0) {
			nextToken = SETQ;
		}
		else if (strcmp(lexeme, "LIST") == 0) {
			nextToken = LIST;
		}
		else {
			nextToken = SYMBOL;
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
		if (charClass == DIGIT && nextToken == SUB_OP) {
			while (charClass == DIGIT) {
				addChar();
				getChar();
			}
			nextToken = INT_LIT;
		}
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
		if (token == LEFT_PAREN) {//여는 괄호로 시작할 때
			token = lex();
			eval(token);//연산 들어가기
		}
		else {//여는 괄호로 시작하지 않을 때: SYMBOL 혹은 syntax error 임. 둘이 구분해야함.
			if (token == SYMBOL) {//symbol로 시작할 때: 존재하는 symbol의 값을 알고싶을때
				string findsym(lexeme);
				FindSymbol(findsym, 1);

			}
			else {
				printf("Syntax Error\n");
				fflush(stdin);
			}
		}
	} while (nextToken != EOF);

	return 0;
}



string FindSymbol(string findsym, int yong_do) {

	map<string, SETQval>::iterator it;
	
	string returnval;
	if (yong_do == 1) { //이 함수를 사용하는 용도가 main에서일때 
		if (symbols.find(findsym) != symbols.end()) { //입력된 symbol이 이미 존재할때
			it = symbols.find(findsym); //map에서 symbol을 찾아서

			if (it->second.val_type == LIST) { //리스트의 경우
				cout << ">" << "(" << it->second.val << ")" << endl;
			}
			else { //리스트 제외 나머지 경우
				cout << ">" << it->second.val << endl; //그 value를 출력함.
			}
		}
		else {//입력된 symbol이 존재하지 않을 때
			cout << "symbol not found." << endl;
			fflush(stdin);
		}
	}
	else { // 이함수를 사용하는 용도가 main이 아니라 다른 작업일때 이 경우에는 콘손에 프린트가 필요 없음

		if (symbols.find(findsym) != symbols.end()) { //입력된 symbol이 이미 존재할때
			it = symbols.find(findsym); //map에서 symbol을 찾아서

			if (it->second.val_type == LIST) { //리스트의 경우
				returnval.append("(");
				returnval.append(it->second.val);
				returnval.append(")");
			}
			else { //리스트 제외 나머지 경우
				returnval.append(it->second.val);
			}
		}
		else {//입력된 symbol이 존재하지 않을 때
			returnval.append(" "); //이떄 공백을 리턴해서 외부에서 처리하도록함 
		}
	}	
	return returnval;
}