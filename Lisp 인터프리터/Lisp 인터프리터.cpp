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
#include <algorithm>
#include <map>
#include <regex>
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
float calc(int token);
int eval(int token);

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define SYMBOL 11
#define POINT 12
#define IF 13
#define PRINT 14
#define COND 15
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
#define CAR 41
#define CDR 42
#define CADR 43
#define NTH 44
#define CONS 45
#define REVERSE 46
#define APPEND 47
#define ENTER 00

#define SETQ 50 //SETQ token number

// predicate function token number
#define ATOM 80
#define NULL 81
#define NUMBERP 82
#define ZEROP 83
#define MINUSP 84
#define EQUAL 85
#define STRINGP 88
#define NIL 89
#define GOE_OP 32
#define QUOTE 33
#define LOE_OP 34

//multiple variable을 넣을 수 있는 map 구현
//string 과 map 을 사용하였고 symbol과 그 symbol에 저장될 SETQval로 구성됨.
//SETQval은 실제 value의 값과 그 value의 자료형을 각각 string과 int로 저장함.
typedef struct SETQval {
	int val_type;
	string val;
}SETQval;

map<string, SETQval> symbols;

//함수선언
map<string, SETQval>::iterator FindSymbol(string findsym, int yongdo);
string ExcuteCARCDR(int token);
string EXcuteCONS();
string ExcuteREVERSE();
string ExcuteAPPEND();


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
			else if (token == SYMBOL) { //symbol일때: 변수도 연산 가능
				if (symbols.find(lexeme) != symbols.end()) {//이미 존재하는 symbol일 때
					if (symbols.find(lexeme)->second.val_type == INT_LIT) { //그 symbol의 값이 숫자일때
						result += atof(symbols.find(lexeme)->second.val.c_str());
						token = lex();
					}
					else {
						cout << "Syntax Error : Symbol is not a number" << endl; //계산하려는 symbol이 숫자가 아닐때
						return NAN;
						break;
					}
				}
				else {
					cout << "Syntax Error : Cannot Find symbol" << endl; //계산하려는 symbol이 존재하지 않을 때
					return NAN;
					break;
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
			else if (token == SYMBOL) { //symbol일때: 변수도 연산 가능
				if (symbols.find(lexeme) != symbols.end()) {//이미 존재하는 symbol일 때
					if (symbols.find(lexeme)->second.val_type == INT_LIT) { //그 symbol의 값이 숫자일때
						result *= atof(symbols.find(lexeme)->second.val.c_str());
						token = lex();
					}
					else {
						cout << "Syntax Error : Symbol is not a number" << endl; //계산하려는 symbol이 숫자가 아닐때
						return NAN;
						break;
					}
				}
				else {
					cout << "Syntax Error : Cannot Find symbol" << endl; //계산하려는 symbol이 존재하지 않을 때
					return NAN;
					break;
				}
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
			else if (token == SYMBOL) { //symbol일때: 변수도 연산 가능
				if (symbols.find(lexeme) != symbols.end()) {//이미 존재하는 symbol일 때
					if (symbols.find(lexeme)->second.val_type == INT_LIT) { //그 symbol의 값이 숫자일때
						result /= atof(symbols.find(lexeme)->second.val.c_str());
						token = lex();
					}
					else {
						cout << "Syntax Error : Symbol is not a number" << endl; //계산하려는 symbol이 숫자가 아닐때
						return NAN;
						break;
					}
				}
				else {
					cout << "Syntax Error : Cannot Find symbol" << endl; //계산하려는 symbol이 존재하지 않을 때
					return NAN;
					break;
				}
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
			else if (token == SYMBOL) { //symbol일때: 변수도 연산 가능
				if (symbols.find(lexeme) != symbols.end()) {//이미 존재하는 symbol일 때
					if (symbols.find(lexeme)->second.val_type == INT_LIT) { //그 symbol의 값이 숫자일때
						result -= atof(symbols.find(lexeme)->second.val.c_str());
						token = lex();
					}
					else {
						cout << "Syntax Error : Symbol is not a number" << endl; //계산하려는 symbol이 숫자가 아닐때
						return NAN;
						break;
					}
				}
				else {
					cout << "Syntax Error : Cannot Find symbol" << endl; //계산하려는 symbol이 존재하지 않을 때
					return NAN;
					break;
				}
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
		break;
	}
	return result;
}

int eval(int token) {
	if (token == ADD_OP || token == SUB_OP || token == MULT_OP || token == DIV_OP) { //사칙연산
		float calc_result = calc(token);
		if (isnan(calc_result)) {
			while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
			return -1;
		}
		else {
			cout << ">" << calc_result << endl;
			return -1;
		}
	}
	else if (token == SETQ) { //SETQ
		token = lex();
		if (token == SYMBOL) {
			string s(lexeme);//symbol string화하기
			token = lex();
			string v(lexeme);//value string화하기
			if (v == ")") {
				cout << "Syntax Error" << endl;
				return -1;
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
				map<string, SETQval>::iterator it = FindSymbol(symbol, 2);
				if (it == symbols.end()) {
					cout << "No" << lexeme << "Symbol exist" << endl;
					while (token != RIGHT_PAREN) token = lex(); //나머지 토큰들 다 처리하기
				}
				list.append(it->second.val);
			}
			token = lex();
			if (token != RIGHT_PAREN)
				list.append(" ");
		}
		cout << "(" << list << ")" << endl;
	}
	else if (token == CAR || token == CDR || token == CADR) { //CAR 과 CDR 처리
		ExcuteCARCDR(token);
	}
	else if (token == NTH) {
		token = lex();
		string num(lexeme);
		int n = atoi(num.c_str()); //숫자로 변환

		token = lex();
		string list;
		if (lexeme[0] == '\'') {//리스트 바로 입력일때 
			token = lex();
			if (token != LEFT_PAREN) {
				cout << "ERROR" << endl;
				token = lex(); //우괄호 처리
				return 0;
			}
			token = lex();
			while (token != RIGHT_PAREN) {
				list.append(lexeme);
				token = lex();
				if (token == RIGHT_PAREN) break;
				list.append(" ");
			}
		}
		else { //심볼일때 
			map<string, SETQval>::iterator it = FindSymbol(lexeme, 2);
			list = it->second.val;
		}


		for (int i = 0; i < n; i++) {

			int cnt = 0;
			while (list[cnt] != ' ') {
				if (cnt >= list.length()) {
					cout << "NIL" << endl;
					lex(); //우괄호처리
					return 0;
				}
				cnt++;
			}
			list = list.erase(0, cnt + 1);
		}
		cout << list.substr(0, 2) << endl;

		token = lex(); //우괼호 처리
	}
	else if (token == CONS) {
		EXcuteCONS();
	}
	else if (token == REVERSE) {
		ExcuteREVERSE();
	}
	else if (token == APPEND) {
		ExcuteAPPEND();
	}

	else if (token == IF) { // if
		int test_expression = -1;
		token = lex();
		if (token == LEFT_PAREN) {
			token = lex();
			test_expression = eval(token); // test statement : 참거짓을 판단해야함. 가정(~한다면)
			if (test_expression == 1) {//test statement이 참일때
				token = lex();
				if (token == LEFT_PAREN) {
					token = lex();
					eval(token);
					token = lex();
					if (token == RIGHT_PAREN) { //(IF (TEST) (EXTR1))형태일 때
						return 1;
					}
					else { // (IF (TEST) (EXPR 1) (EXPR 2)) 형태일 때
						while (token != RIGHT_PAREN) token = lex(); //참일때 실행할 명령만 실행한 후 그 뒤의 문장은 무시.
						token = lex(); //전체 문장 닫는 괄호 한번 더 처리
					}
				}
				else {
					cout << "Syntax Error : too few elements in IF statement" << endl;
					while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
					return -1;
				}
			}
			else if (test_expression == 0) {//test expression 이 거짓일때
				token = lex();
				while (token != RIGHT_PAREN) { //(EXPR1) 는 뛰어넘기
					token = lex();
				}
				token = lex();
				if (token == LEFT_PAREN) { //(EXPR2)가 있다면, 실행
					token = lex();
					eval(token);
				}
				else { //(EXPR2)가 없다면, 아무것도 하지 않음.
					return 0;
				}
			}
			else { //참거짓을 가릴 수 없는 evaluation일 때
				cout << "Error : test expression is not correct" << endl;
				while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
			}
		}
		else {
			cout << "Syntax Error" << endl;
			while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
		}
	}

	else if (token == PRINT) { //출력하는 명령어
		token = lex();
		if (token == SYMBOL) { //심볼일 떄
			string l(lexeme);
			map<string, SETQval>::iterator sym_val = FindSymbol(l, 2);
			if (sym_val == symbols.end()) {
				cout << "Error : variable " << l << "is unbound" << endl;
				while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
				return -1;
			}
			else {
				cout << ">" << sym_val->second.val << endl;
				token = lex(); //닫는 괄호 처리
			}
		}
		else if (token == INT_LIT) { //숫자일 때
			cout << ">" << atof(lexeme) << endl;
			token = lex(); //닫는 괄호 처리
		}
		else if (token == LEFT_PAREN) {
			token = lex();
			if (token == LIST) { //리스트일 때
				eval(token);
				token = lex(); //닫는 괄호 처리
			}
			else { //다른 명령일 때
				cout << "Syntax Error : cannot print" << endl;
				while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
				token = lex(); //닫는괄호처리 한번 더
				return -1;
			}
		}
		else if (token == QUOTE) { // '로 시작하는 리스트일 때
			token = lex(); // ( 괄호
			token = LIST;
			eval(token);
			token = lex(); // 닫는 괄호 처리
		}
		else {
			cout << "Syntax Error : cannot print" << endl;
			while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
			token = lex(); //닫는 괄호 처리
			return -1;
		}
	}
	else if (token == COND) {
		int test_statement = -1;
		token = lex();
		if (token != LEFT_PAREN) {
			cout << "Syntax Error : COND" << endl;
			rewind(stdin); nextChar = '\n'; charClass = 99; //문장 전체 오류처리
			return -1;
		}
		while (token == LEFT_PAREN && nextChar == '(') { //((statement)(expression))일 때
			token = IF;
			test_statement = eval(token);
			if (test_statement == 1) {
				rewind(stdin); nextChar = '\n'; charClass = 99; //남은 문장은 버리기
				return -1;
			}
			else{
				token = lex();
			}
		}
		if (token == LEFT_PAREN) {
			token = lex();
			eval(token);
			token = lex(); //닫는 괄호 처리
		}
		else {
			cout << "Syntax Error : COND" << endl;
			rewind(stdin); nextChar = '\n'; charClass = 99; //문장 전체 오류처리
			return -1;
		}

	}
	else if (token == ATOM) {
		token = lex(); //symbol
		if (token == SYMBOL) {
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return 1;
		}
		else {
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return 0;
		}

	}
	else if (token == NULL) {
		token = lex();
		string s;
		map<string, SETQval>::iterator i;
		if (token == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s = i->second.val;
		}
		if (s == "nil" || token == NIL) {
			cout << "> T" << endl;
			token = lex(); // 닫는 괄호 처리
			return 1;
		}
		else {
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return 0;
		}
	}
	else if (token == NUMBERP) {
		token = lex();
		string s;
		map<string, SETQval>::iterator i;
		if (token == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s = i->second.val;
		}
		if (atoi(s.c_str()) != 0 || s.compare("0") == 0 || token == INT_LIT)		// 숫자
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return 1;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return 0;
		}
	}
	else if (token == ZEROP) {
		token = lex();
		string s;
		map<string, SETQval>::iterator i;
		if (token == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s = i->second.val;
		}
		if (atoi(s.c_str()) != 0 || s.compare("0") == 0 || token == INT_LIT) {
			if (s.compare("0") == 0 || string(lexeme) == "0")	// 숫자이고, 0인것
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else
			cout << "Error! " << lexeme << " is not DIGIT" << endl;

	}
	else if (token == MINUSP) {
		token = lex();
		string s;
		map<string, SETQval>::iterator i;
		if (token == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s = i->second.val;
		}
		if (atoi(s.c_str()) != 0 || s.compare("0") == 0 || token == INT_LIT) {

			if (atoi(s.c_str()) < 0 || atoi(lexeme) < 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else
			cout << "Error! " << lexeme << " is not DIGIT" << endl;
	}
	else if (token == EQUAL) {
		string p1, p2;
		map<string, SETQval>::iterator i;
		string s1, s2;
		int t1, t2;

		token = lex();
		p1 = lexeme;
		t1 = token;
		if (token == QUOTE) {			// 첫번째 비교인자가 '0 같은 경우 
			token = lex();
			p1 += lexeme;
			if (token == LEFT_PAREN) {		// 리스트가 비교인자로 들어오는 경우, '(리스트) 를 인식
				while (token != RIGHT_PAREN) {
					token = lex();
					p1 += " ";
					p1 += lexeme;
				}
			}
		}
		if (t1 == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s1 = i->second.val;
		}

		token = lex();
		p2 = lexeme;
		t2 = token;
		if (token == QUOTE) {	// 첫번째 비교인자가 '0 같은 경우 	
			token = lex();
			p2 += lexeme;
			if (token == LEFT_PAREN) {			// 리스트가 비교인자로 들어오는 경우, '(리스트) 를 인식
				while (token != RIGHT_PAREN) {
					token = lex();
					p2 += " ";
					p2 += lexeme;
				}
			}
		}
		if (t2 == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s2 = i->second.val;
		}

		// EQUAL 함수로 들어올 수 있는 파라미터의 경우의 수.
		if (t1 == INT_LIT && t2 == INT_LIT) {		// DIGIT == DIGIT
			if ((p1.compare(p2) == 0))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == SYMBOL && t2 == SYMBOL) {		// SYMBOL == SYMBOL
			if ((s1.compare(s2) == 0) && s1 != " " && s2 != " ")
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT == SYMBOL	
			if (p1.compare(s2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL == DIGIT
			if (s1.compare(p2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == QUOTE && t2 == QUOTE) {	// LIST == LIST	    --->    (EQUAL '(1 2) '(1 3)) 같은 경우
			p2 = p2.substr(1, p2.length() - 1);
			p1 = p1.substr(1, p1.length() - 1);
			if ((p1.compare(p2) == 0))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == INT_LIT && t2 == QUOTE) {		// DIGIT == LIST -->  (EQUAL 0 '0)
			p2 = p2.substr(2, p2.length() - 1);
			if (p1.compare(p2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == QUOTE && t2 == INT_LIT) {		// LIST == DIGIT -->  (EQUAL '0 0)
			p1 = p1.substr(2, p1.length() - 1);
			if (p1.compare(p2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == SYMBOL && t2 == QUOTE) {		// SYMBOL == LIST
			s1.insert(0, "'( ");
			s1.append(" )");
			p2 = p2.substr(1, p2.length() - 1);
			if (s1.compare(p2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}
		else if (t1 == QUOTE && t2 == SYMBOL) {		// LIST == SYMBOL
			s2.insert(0, "'( ");
			s2.append(" )");
			p1 = p1.substr(1, p1.length() - 1);
			if (p1.compare(s2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return 1;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return 0;
			}
		}

		//cout << "p1 ----> " << p1 << endl;
		//cout << "p2 ----> " << p2 << endl;
		//cout << "s1 ----> " << s1 << endl;
		//cout << "s2 ----> " << s2 << endl;
		//cout << "t1 ----> " << t1 << endl;
		//cout << "t2 ----> " << t2 << endl;

	}
	else if (token == LT_OP) {		// --> real number만 가능.
		string p1, p2;
		map<string, SETQval>::iterator i;
		string s1, s2;
		int t1, t2;

		token = lex();
		p1 = lexeme;
		t1 = token;
		if (t1 == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s1 = i->second.val;
		}

		token = lex();
		p2 = lexeme;
		t2 = token;
		if (t2 == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s2 = i->second.val;
		}

		// 계산
		if (t1 == INT_LIT && t2 == INT_LIT) {		// DIGIT < DIGIT
			if (atoi(p1.c_str()) < atoi(p2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}
		else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT < SYMBOL
			if (atoi(p1.c_str()) < atoi(s2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}
		else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL < DIGIT
			if (atoi(s1.c_str()) < atoi(p2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}
		else if (t1 == SYMBOL && t2 == SYMBOL) {			// SYMBOL < SYMBOL
			if (atoi(s1.c_str()) < atoi(s2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}

		//cout << "p1 ----> " << p1 << endl;
		//cout << "p2 ----> " << p2 << endl;
		//cout << "s1 ----> " << s1 << endl;
		//cout << "s2 ----> " << s2 << endl;
		//cout << "t1 ----> " << t1 << endl;
		//cout << "t2 ----> " << t2 << endl;

	}
	else if (token == GOE_OP) {

		string p1, p2;
		map<string, SETQval>::iterator i;
		string s1, s2;
		int t1, t2;

		token = lex();
		p1 = lexeme;
		t1 = token;
		if (t1 == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s1 = i->second.val;
		}

		token = lex();
		p2 = lexeme;
		t2 = token;
		if (t2 == SYMBOL) {
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s2 = i->second.val;
		}

		// 계산
		if (t1 == INT_LIT && t2 == INT_LIT) {		// DIGIT < DIGIT
			if (atoi(p1.c_str()) >= atoi(p2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}
		else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT < SYMBOL
			if (atoi(p1.c_str()) >= atoi(s2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}
		else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL < DIGIT
			if (atoi(s1.c_str()) >= atoi(p2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}
		else if (t1 == SYMBOL && t2 == SYMBOL) {			// SYMBOL < SYMBOL
			if (atoi(s1.c_str()) >= atoi(s2.c_str()))
				cout << "> T" << endl;
			else
				cout << "> F" << endl;
		}

		//cout << "p1 ----> " << p1 << endl;
		//cout << "p2 ----> " << p2 << endl;
		//cout << "s1 ----> " << s1 << endl;
		//cout << "s2 ----> " << s2 << endl;
		//cout << "t1 ----> " << t1 << endl;
		//cout << "t2 ----> " << t2 << endl;

	}
	else if (token == STRINGP) {
		//if ()
		//	cout << "> T" << endl;
		//else
		//	cout << "> F" << endl;
	}

	else { //여기에 계속 다른 연산 추가

	}
}


string ExcuteCARCDR(int token) {

	string ultimate;

	if (token == CAR) {
		token = lex();
		int cntl = 0; //괄호 개수 세기
		if (lexeme[0] == '\'') { //그냥 리스트인경우
			token = lex(); //왼쪽 괄호 처리 (
			cntl++;
			token = lex();
			string v;
			if (token == LEFT_PAREN) {
				v.append(lexeme);
				while (token != RIGHT_PAREN) {
					token = lex();
					v.append(lexeme);
				}
			}
			else {
				v.append(lexeme);
			}
			cout << v << endl;

			while (cntl != -1) { //남은 토큰들 처리
				token = lex();
				if (token == RIGHT_PAREN) cntl--;
				else if (token == LEFT_PAREN) cntl++;
			}

			return v;
		}
		else if (lexeme[0] == '(') { //괄호인 경우 즉 재귀적인경우
			token = lex();
			if (token == CDR) {
				string v = ExcuteCARCDR(token);
				int cnt = 0;
				int vlen = v.length();
				for (; cnt < vlen; cnt++) {
					if (v[cnt] == ' ') break;
				}
				if (cnt == vlen) //처번쨰 원소가 전부인 경우
					cout << v << endl;
				else {
					v = v.erase(cnt + 1, vlen);
					cout << v << endl;
				}
				token = lex(); //오른괄호 처리
				token = lex();
				return v;
			}
		}
		else { //심볼인 경우
			//예제에 없었으니까 일단 안함
		}
	}
	else if (token == CDR) {
		token = lex();
		string s;
		if (lexeme[0] == '\'') { //그냥 리스트인경우
			token = lex(); //왼괄호 처리
			token = lex(); //첫번쨰 원소 
			token = lex(); //두번쨰 원소
			while (token != RIGHT_PAREN) {
				s.append(lexeme);
				token = lex();
				if (token == RIGHT_PAREN) { break; }
				s.append(" ");
			}
			token = lex(); //나머지 오른괄호 처리
			cout << '(' << s << ')' << endl;
			return s;
		}
		else if (lexeme[0] == '(') { //왼괄호인 경우 즉 재귀적 호출인 경우
			token = lex();
			if (token == CDR) {
				int cnt = 0;
				string v = ExcuteCARCDR(token);
				while (1) {
					if (v[cnt] == ' ') break;
					cnt++;
				}
				token = lex(); //오른괄호 처리
				v = v.erase(0, cnt + 1);
				//cout << v << endl;
				return v;
			}

		}
		else if (token == SYMBOL) { //심볼인 경우
			map<string, SETQval>::iterator it = FindSymbol(lexeme, 2);
			string v = it->second.val;
			//cout << v << endl;
			int cnt = 0;
			while (1) {
				if (v[cnt] == ' ') break;
				cnt++;
			}
			v = v.erase(0, cnt + 1);
			//cout << v <<cnt<< endl;
			return v;
		}
		else { // 나머지

		}
		if (ultimate == "^C[AD]R") {

		}
	}
	else if (token == CADR) {
		string oder(lexeme);
		oder = oder.erase(0, 1);
		oder = oder.erase(oder.length() - 1, oder.length());

		token = lex();
		string v;
		if (token == SYMBOL) { //심볼일경우
			map<string, SETQval>::iterator it = FindSymbol(lexeme, 2);
			v = it->second.val;
		}
		else { //그냥 리스트일 경우
			token = lex(); //좌괄호 처리
			token = lex();
			while (token != RIGHT_PAREN) {
				v.append(lexeme);
				token = lex();
				if (token != RIGHT_PAREN) break;
				v.append(" ");
			}
		}

		for (int i = oder.length() - 1; i >= 0; i--) {
			if (oder[i] == 'D') { //D인 경우
				int cnt = 0;
				while (1) {
					if (v[cnt] == ' ') break;
					cnt++;
				}
				v = v.erase(0, cnt + 1);
			}
			else { //C인 경우
				int cnt = 0;
				int vlen = v.length();
				for (; cnt < vlen; cnt++) {
					if (v[cnt] == ' ') break;
				}
				if (cnt == vlen) {
				}//처번쨰 원소가 전부인 경우 아무것도 안함 
				else {
					v = v.erase(cnt + 1, vlen);
					//cout << v << endl;
				}
			}
		}
		cout << v << endl;
		token = lex(); //우괄호 처리
		return v;
	}
	return ultimate;
}
string EXcuteCONS() {
	token = lex();
	string frontlist;
	string backlist;
	bool symbolExist = true;
	map<string, SETQval>::iterator it;
	if (token == QUOTE) {
		token = lex();
		frontlist.append(lexeme);
	}
	else { // 심볼일 경우
		it = FindSymbol(lexeme, 2);
		if (it == symbols.end()) symbolExist = false;
		frontlist = it->second.val;
	}

	token = lex(); //두번째 쿼트
	token = lex(); //좌괄호
	token = lex(); //첫뻔재 원소
	while (token != RIGHT_PAREN) {
		backlist.append(lexeme);
		token = lex();
		if (token == RIGHT_PAREN) break;
		backlist.append(" ");
	}
	token = lex(); //우괄호 처리
	if (symbolExist == false) {
		cout << "This Symbol dose not exist" << endl;
		return " ";
	}
	cout << "(" << frontlist << " " << backlist << ")" << endl;
	return frontlist + " " + backlist;
}
string ExcuteREVERSE() {
	token = lex();
	string list;
	bool symbolExist = true;
	map <string, SETQval>::iterator it;
	if (token == QUOTE) { //그냥 리스트인경우
		token = lex();//좌괄호 처리
		token = lex(); //첫뻔재 원소
		while (token != RIGHT_PAREN) {
			list.append(lexeme);
			token = lex();
			if (token == RIGHT_PAREN) break;
			list.append(" ");
		}
	}
	else { //심볼인 경우
		it = FindSymbol(lexeme, 2);
		if (it == symbols.end()) symbolExist == false;
		list = it->second.val;
	}
	lex(); //우괄호 처리
	if (symbolExist == false) {
		cout << "This Symbol dose not exist" << endl;
		return " ";
	}
	reverse(list.begin(), list.end());
	cout << '(' << list << ")" << endl;
	return list;
}
string ExcuteAPPEND() {
	string list;
	map<string, SETQval>::iterator it;
	bool symbolExist = true;
	token = lex(); //이게 쿼트이거나 심볼임
	while (token != RIGHT_PAREN) {

		if (token == QUOTE) { //쿼트인 경우
			token = lex();//좌괄호 처리
			token = lex(); //첫뻔재 원소
			while (token != RIGHT_PAREN) {
				list.append(lexeme);
				token = lex();
				if (token == RIGHT_PAREN) break;
				list.append(" ");
			}
		}
		else { //심볼인경우
			it = FindSymbol(lexeme, 2);
			if (it == symbols.end()) symbolExist = false;
			list.append(it->second.val);
		}
		token = lex();
		if (token == RIGHT_PAREN) break;
		list.append(" ");
	}
	if (symbolExist == false) {
		cout << "This Symbol dose not exist" << endl;
		return " ";
	}
	cout << '(' << list << ")" << endl;
	return list;
}


/******************************************
 * lookup - a function to lookup operators
 * and parentheses and return the token
 ******************************************/
int lookup(char ch) {
	switch (ch) {
	case '(':
		nextToken = LEFT_PAREN;
		break;
	case ')':
		nextToken = RIGHT_PAREN;
		break;
	case '+':
		nextToken = ADD_OP;
		break;
	case '-':
		nextToken = SUB_OP;
		break;
	case '*':
		nextToken = MULT_OP;
		break;
	case '/':
		nextToken = DIV_OP;
		break;
	case '=':
		nextToken = ASSIGN_OP;
		break;
	case ';':
		nextToken = SEMI_COLON;
		break;
	case '<':
		nextToken = LT_OP;
		break;
	case '>':
		nextToken = GT_OP;
		break;
	case '{':
		nextToken = LEFT_BR;
		break;
	case '}':
		nextToken = RIGHT_BR;
		break;
	case '.':
		nextToken = POINT;
		break;
	case '>=':
		addChar();
		nextToken = GOE_OP;
		break;
	case '\'':
		addChar();
		nextToken = QUOTE;
		break;
	default:
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

	//CADR에개한 정규표현식
	regex card("C[AD]*R");

	switch (charClass) {
		/* Parse identifiers */
	case LETTER:
		addChar();
		getChar();
		while (charClass == LETTER || charClass == DIGIT) {
			addChar();
			getChar();
		}
		if (stricmp(lexeme, "COND") == 0) {
			nextToken = COND;
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
		else if (stricmp(lexeme, "SETQ") == 0) {
			nextToken = SETQ;
		}
		else if (stricmp(lexeme, "LIST") == 0) {
			nextToken = LIST;
		}
		else if (stricmp(lexeme, "IF") == 0) {
			nextToken = IF;
		}
		else if (stricmp(lexeme, "PRINT") == 0) {
			nextToken = PRINT;
		}
		else if (stricmp(lexeme, "CAR") == 0) {
			nextToken = CAR;
		}
		else if (stricmp(lexeme, "CDR") == 0) {
			nextToken = CDR;
		}
		else if (regex_match(lexeme, card)) {
			nextToken = CADR;
		}
		else if (stricmp(lexeme, "NTH") == 0) {
			nextToken = NTH;
		}
		else if (strcmp(lexeme, "CONS") == 0) {
			nextToken = CONS;
		}
		else if (strcmp(lexeme, "REVERSE") == 0) {
			nextToken = REVERSE;
		}
		else if (strcmp(lexeme, "APPEND") == 0) {
			nextToken = APPEND;
		}
		else if (strcmp(lexeme, "ATOM") == 0) {
			nextToken = ATOM;
		}
		else if (stricmp(lexeme, "NULL") == 0) {
			nextToken = NULL;
		}
		else if (stricmp(lexeme, "nil") == 0) {
			nextToken = NIL;
		}
		else if (stricmp(lexeme, "NUMBERP") == 0) {
			nextToken = NUMBERP;
		}
		else if (stricmp(lexeme, "ZEROP") == 0) {
			nextToken = ZEROP;
		}
		else if (stricmp(lexeme, "MINUSP") == 0) {
			nextToken = MINUSP;
		}
		else if (stricmp(lexeme, "EQUAL") == 0) {
			nextToken = EQUAL;
		}
		else if (stricmp(lexeme, "STRINGP") == 0) {
			nextToken = STRINGP;
		}
		else if (strcmp(lexeme, "'") == 0) {
			nextToken = QUOTE;
		}
		else {
			nextToken = SYMBOL;
		}
		break;

		/* Parse integer&float literals */
	case DIGIT:
		addChar();
		getChar();
		while (charClass == DIGIT || charClass == UNKNOWN) {
			if (charClass == DIGIT) {
				addChar();
				getChar();
			}
			else if (charClass == UNKNOWN) {
				lookup(nextChar);
				if (nextToken == POINT) {
					addChar();
					getChar();
				}
				else {
					break;
				}
			}
		}
		nextToken = INT_LIT;
		break;

		/* Parentheses and operators */
	case UNKNOWN:
		lookup(nextChar);
		addChar();
		getChar();
		if (charClass == DIGIT && nextToken == SUB_OP) { //음수인식
			while (charClass == DIGIT) {
				addChar();
				getChar();
			}
			nextToken = INT_LIT;
		}
		else if (charClass == UNKNOWN && nextToken == LT_OP) { //<= 인식
			lookup(nextChar);
			if (nextToken == ASSIGN_OP) {
				addChar();
				getChar();
				nextToken = LOE_OP;
			}
			else {
				nextToken = lookup(lexeme[0]);
			}
		}
		else if (charClass == UNKNOWN && nextToken == GT_OP) { //>= 인식
			lookup(nextChar);
			if (nextToken == ASSIGN_OP) {
				addChar();
				getChar();
				nextToken = GOE_OP;
			}
			else {
				nextToken = lookup(lexeme[0]);
			}
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
	//if ((out_fp = fopen("code.out", "a")) == NULL) {
	//	printf("ERROR - cannot open front.in \n");
	//}
	//else {
	//	printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme);
	//}
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



map<string, SETQval>::iterator FindSymbol(string findsym, int yong_do) { //사용자가 입력한 symbol의 값을 찾아주는 함수.map<string, SETQval>::

	map<string, SETQval>::iterator it;

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
	else { // 이함수를 사용하는 용도가 main이 아니라 다른 작업일때 이 경우에는 콘솔에 프린트가 필요 없음

		if (symbols.find(findsym) != symbols.end()) { //입력된 symbol이 이미 존재할때
			it = symbols.find(findsym); //map에서 symbol을 찾아서

		}
		else {//입력된 symbol이 존재하지 않을 때
			it = symbols.end();
		}
	}
	return it;
}