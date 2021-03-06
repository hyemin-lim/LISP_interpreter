﻿/**
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
string eval(int token);

/* Character classes */
#define LETTER 105
#define DIGIT 205
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define SYMBOL 11
#define POINT 12
#define IF 13
#define PRINT 14
#define COND 15
#define HASHTAG 16
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
#define LENGTH 48
#define MEMBER 49
#define SETQ 50 //SETQ token number
#define ASSOC 51
#define REMOVE 52
#define SUBST 53
#define ENTER 00





// predicate function token number
#define ATOM 80
#define NULL 81
#define NUMBERP 82
#define ZEROP 83
#define MINUSP 84
#define EQUAL 85
#define STRINGP 88
string TRUE("TRUE");
string FALSE("NIL");
#define NIL 0
#define GOE_OP 32
#define QUOTE 33
#define LOE_OP 34
#define DOUBLE_QUOTE 35

#define STRING 50

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
string ExcuteASSOC();
string ExcuteMEMBER(int token);

/******************************************/
/* main driver                            */
/******************************************/
int main()
{
	/* Open the input data file and process its contents */
	if ((in_fp = fopen("testcase.txt", "r")) == 0) {
		printf("ERROR - cannot open front.in \n");
	}
	else {
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
				else if (token == EOF) {
					break;
				}
				else {
					printf("Syntax Error\n");
					while (nextChar != '\n') token = lex();//문장 전체 오류처리
				}
			}
		} while (nextToken != EOF);
	}


	return 0;
}


//사칙연산에 쓰이는 함수. 더하기, 곱하기, 나누기, 빼기를 지원한다.
//문장 안에 여러 번의 operator가 출현해도 작동한다.
//(단, operator가 사칙연산 operator로 시작했을 시 다음에 오는 operator는 무조건 사칙연산 관련이어야 한다.)
//(예시 : (+ 5 (- 5 2)) 는 가능, (+ 5 (LIST A B))는 불가능)
float calc(int token) {
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

//토큰을 분석하여 operator에 맞는 기능을 구현하는 함수.
string eval(int token) {
	if (token == ADD_OP || token == SUB_OP || token == MULT_OP || token == DIV_OP) { //사칙연산 operator일 때
		float calc_result = calc(token);
		if (isnan(calc_result)) {
			while (token != RIGHT_PAREN) token = lex(); //문장 오류처리
			return "";
		}
		else {
			cout << ">" << calc_result << endl;
			return "";
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
				return "";
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
			else if (v[0] == '\"') { //string을 setq할때 
				string str;
				token = lex(); //원소 찾기
				str.append(lexeme);
				token = lex();
				while (token != DOUBLE_QUOTE) {
					str.append(" ");
					str.append(lexeme);
					token = lex();
				}
				SETQval newval;
				newval.val = str;
				newval.val_type = STRING;
				symbols.insert(make_pair(s, newval)); //값 저장하기
				token = lex(); //닫는 괄호 처리하기
				cout << ">" << "\"" << str << "\"" << endl;

			}
			else { //리스트 아닌걸 SETQ 할떄
				SETQval newval;
				newval.val = v;
				newval.val_type = token;
				if (symbols.find(s) == symbols.end()) { //setq 할 symbol이 이미 존재하지 않을 때
					symbols.insert(make_pair(s, newval));//값 저장하기
					token = lex(); // 닫는 괄호 처리하기
					cout << ">" << v << endl;
				}
				else { //setq 할 symbol이 이미 존재할 때
					symbols.find(s)->second = newval;
					token = lex(); // 닫는 괄호 처리하기
					cout << ">" << v << endl;
				}
				
			}

		}
		return "";
	}
	else if (token == LIST) { //LIST
		string list;
		token = lex();
		while (token != RIGHT_PAREN) {
			string v(lexeme);
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
		return list;
	}
	else if (token == CAR || token == CDR || token == CADR) { //CAR 과 CDR 처리
		return ExcuteCARCDR(token);
	}
	else if (token == NTH) { //n번째 원소를 반환
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
				return "";
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
			while (list[cnt] != ' ') { //n번째 원소가 존재하지 않는 경우
				if (cnt >= list.length()) {
					cout << "NIL" << endl;
					lex(); //우괄호처리
					return FALSE;
				}
				cnt++;
			}
			list = list.erase(0, cnt + 1);
		}
		cout << list.substr(0, 2) << endl;

		token = lex(); //우괼호 처리
		return "";
	}
	else if (token == CONS) { //기존 리스트에 새 원소를 추가한 리스트 생성 : CONS
		return EXcuteCONS();
	}
	else if (token == REVERSE) { //리스트 안의 원소의 순서를 거꾸로 바꾸기 : REVERSE
		return ExcuteREVERSE();
	}
	else if (token == APPEND) {
		return ExcuteAPPEND();
	}
	else if (token == ASSOC) {
		return ExcuteASSOC();
	}
	else if (token == REMOVE) {
		token = lex();
		string removestring;
		string list;
		map<string, SETQval>::iterator it;
		bool symbolExist = true;
		if (token == QUOTE) { //쿼트로 들어올 경우
			token = lex();
			removestring = lexeme;
		}
		else {
			it = FindSymbol(lexeme, 2);
			removestring = it->second.val;
		}

		token = lex();
		//뒤의 리스트 처리
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
			if (it == symbols.end()) { symbolExist = false; }
			list.append(it->second.val);
		}

		token = lex(); //우괄호 처리 토큰들은 다 읽음
		string key1 = removestring + " ";
		string key2 = " " + removestring;

		int index = 0;
		while (index != string::npos) {
			index = list.find(key1);
			if (index == string::npos) break;

			list.replace(index, key1.length(), "");
		}
		while (index != string::npos) {
			index = list.find(key2);
			if (index == string::npos) break;

			list.replace(index, key2.length(), "");
		}
		cout << list << endl;
		return list;
	}
	else if (token == SUBST) {
		string first;
		string second;
		string list;
		map<string, SETQval>::iterator it;

		//first 처리
		token = lex();
		if (token == QUOTE) {
			token = lex();
			first = lexeme;
		}
		else {
			it = FindSymbol(lexeme, 2);
			first = it->second.val;
		}

		//second 처리
		token = lex();
		if (token == QUOTE) {
			token = lex();
			second = lexeme;
		}
		else {
			it = FindSymbol(lexeme, 2);
			second = it->second.val;
		}

		//리스트처리
		token = lex();
		if (token == QUOTE) {
			lex();//좌괄호 처리
			lex();
			while (token != RIGHT_PAREN) {
				list.append(lexeme);
				token = lex();
				if (token == RIGHT_PAREN) break;
				list.append(" ");
			}
		}
		else {
			it = FindSymbol(lexeme, 2);
			list = it->second.val;
		}
		lex(); //우괄호 처리

		int index = list.find(second);
		if (index == string::npos) {
		}
		else {
			list.replace(index, second.length(), first);
		}
		cout << "(" << list << ")" << endl;
		return list;
	}

	else if (token == IF) { // if
		string test_expression = "";
		token = lex();
		if (token == LEFT_PAREN) {
			token = lex();
			cout << "test expression ";
			test_expression = eval(token); // test statement : 참거짓을 판단해야함. 가정(~한다면)
			if (test_expression == TRUE) {//test statement이 참일때
				token = lex();
				if (token == LEFT_PAREN) {
					token = lex();
					eval(token);
					token = lex();
					if (token == RIGHT_PAREN) { //(IF (TEST) (EXTR1))형태일 때
						return TRUE;
					}
					else { // (IF (TEST) (EXPR 1) (EXPR 2)) 형태일 때
						while (token != RIGHT_PAREN) token = lex(); //참일때 실행할 명령만 실행한 후 그 뒤의 문장은 무시.
						token = lex(); //전체 문장 닫는 괄호 한번 더 처리
					}
				}
				else {
					cout << "Syntax Error : too few elements in IF statement" << endl;
					while (nextChar != '\n') token = lex();//문장 전체 오류처리
					return "";
				}
			}
			else if (test_expression == FALSE) {//test expression 이 거짓일때
				token = lex();
				if (token == LEFT_PAREN) {
					while (token != RIGHT_PAREN) { //(EXPR1) 는 뛰어넘기
						token = lex();
					}
					token = lex();
					if (token == LEFT_PAREN) { //(EXPR2)가 있다면, 실행
						token = lex();
						eval(token);
						token = lex();//닫는 괄호 처리
						return FALSE;
					}
					else { //(EXPR2)가 없다면, 아무것도 하지 않음.
						return FALSE;
					}
				}
				else {
					cout << "Syntax Error : too few elements in IF statement" << endl;
					while (nextChar != '\n') token = lex();//문장 전체 오류처리
					return "";
				}
				
			}
			else { //참거짓을 가릴 수 없는 evaluation일 때
				cout << "Error : test expression is not correct" << endl;
				while (nextChar != '\n') token = lex();//문장 전체 오류처리
				return "";
			}
			
		}
		else {
			cout << "Syntax Error" << endl;
			while (nextChar != '\n') token = lex();//문장 전체 오류처리
			return "";
		}
	}

	else if (token == PRINT) { //출력하는 명령어
		token = lex();
		if (token == SYMBOL) { //심볼일 떄
			string l(lexeme);
			map<string, SETQval>::iterator sym_val = FindSymbol(l, 2);
			if (sym_val == symbols.end()) {
				cout << "Error : symbol " << l << "is unbound" << endl;
				while (token != RIGHT_PAREN) token = lex();//PRINT 명령 전체 오류처리
				return "";
			}
			else {
				cout << ">" << sym_val->second.val << endl;
				token = lex(); //닫는 괄호 처리
				return sym_val->second.val;
			}
		}
		else if (token == INT_LIT) { //숫자일 때
			cout << ">" << atof(lexeme) << endl;
			token = lex(); //닫는 괄호 처리
			return "";
		}
		else if (token == LEFT_PAREN) {
			token = lex();
			if (token == LIST) { //리스트일 때
				eval(token);
				token = lex(); //닫는 괄호 처리
			}
			else { //다른 명령일 때
				cout << "Syntax Error : cannot print" << endl;
				while (token != RIGHT_PAREN) token = lex();//PRINT 명령 전체 오류처리
				return "";
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
			return "";
		}
	}
	else if (token == COND) {
		string test_statement = "";
		token = lex();
		if (token != LEFT_PAREN) {
			cout << "Syntax Error : COND" << endl;
			while (nextChar != '\n') token = lex();//문장 전체 오류처리
			return "";
		}
		while (token == LEFT_PAREN && nextChar == '(') { //((statement)(expression))일 때
			token = IF;
			test_statement = eval(token);
			if (test_statement == TRUE) {
				while (nextChar != '\n') token = lex();//문장 전체 오류처리
				return "";
			}
			else {
				if(nextChar == '(' || nextChar == ')') token = lex();
			}
		}
		if (test_statement == FALSE && token == LEFT_PAREN) {
			token = lex();
			cout << "Execute default statement: COND" << endl;
			eval(token);
			token = lex();
			return "";
		}

	}
	else if (token == MEMBER) {
		ExcuteMEMBER(token);
	}
	else if (token == ATOM) {
		token = lex(); //symbol
		if (token == SYMBOL) {
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else {
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
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
		else if (token == LEFT_PAREN) {
			token = lex();
			cout << "isNIL(F means NIL): ";
			s = eval(token);
		}
		else {}
		if (s == "NIL" || s == "nil" || token == NIL) {
			cout << "> T" << endl;
			if(nextChar == ')') token = lex(); // 닫는 괄호 처리
			return TRUE;
		}
		else {
			cout << "> F" << endl;
			if (nextChar == ')') token = lex(); //닫는 괄호 처리
			return FALSE;
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
			if (atoi(s.c_str()) != 0 || s.compare("0") == 0 || token == INT_LIT)		// 숫자
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
		}
		else if (token == INT_LIT) {
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
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
			if (atoi(s.c_str()) != 0 || s.compare("0") == 0 || token == INT_LIT) {
				if (s.compare("0") == 0 || string(lexeme) == "0")	// 숫자이고, 0인것
				{
					cout << "> T" << endl;
					token = lex(); //닫는 괄호 처리
					return TRUE;
				}
				else
				{
					cout << "> F" << endl;
					token = lex(); //닫는 괄호 처리
					return FALSE;
				}
			}
		}
		else if (token == INT_LIT) {
			if (atof(lexeme) == 0) {
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else {
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
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
			if (atoi(s.c_str()) != 0 || s.compare("0") == 0 || token == INT_LIT) {

				if (atoi(s.c_str()) < 0 || atoi(lexeme) < 0)
				{
					cout << "> T" << endl;
					token = lex(); //닫는 괄호 처리
					return TRUE;
				}
				else
				{
					cout << "> F" << endl;
					token = lex(); //닫는 괄호 처리
					return FALSE;
				}
			}
		}
		else if (token == INT_LIT) {
			if (atof(lexeme) < 0) {
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else {
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else
			cout << "Error! " << lexeme << " is not DIGIT" << endl;
	}
	else if (token == EQUAL || token == ASSIGN_OP) {
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
			if (i == symbols.end()) {
				cout << "EQUAL Error : Symbol does not exist" << endl;
				s1 = "";
			}
			else {
				s1 = i->second.val;
			}
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
			if (i == symbols.end()) {
				cout << "EQUAL Error : Symbol does not exist" << endl;
				s2 = " ";
			}
			else {
				s2 = i->second.val;
			}
		}

		// EQUAL 함수로 들어올 수 있는 파라미터의 경우의 수.
		if (t1 == INT_LIT && t2 == INT_LIT) {		// DIGIT == DIGIT
			if ((p1.compare(p2) == 0))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == SYMBOL && t2 == SYMBOL) {		// SYMBOL == SYMBOL
			if ((s1.compare(s2) == 0) && s1 != " " && s2 != " ")
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT == SYMBOL	
			if (p1.compare(s2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL == DIGIT
			if (s1.compare(p2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == QUOTE && t2 == QUOTE) {	// LIST == LIST	    --->    (EQUAL '(1 2) '(1 3)) 같은 경우
			p2 = p2.substr(1, p2.length() - 1);
			p1 = p1.substr(1, p1.length() - 1);
			if ((p1.compare(p2) == 0))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == INT_LIT && t2 == QUOTE) {		// DIGIT == LIST -->  (EQUAL 0 '0)
			p2 = p2.substr(2, p2.length() - 1);
			if (p1.compare(p2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == QUOTE && t2 == INT_LIT) {		// LIST == DIGIT -->  (EQUAL '0 0)
			p1 = p1.substr(2, p1.length() - 1);
			if (p1.compare(p2) == 0)
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
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
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
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
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}

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
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT < SYMBOL
			if (atoi(p1.c_str()) < atoi(s2.c_str()))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL < DIGIT
			if (atoi(s1.c_str()) < atoi(p2.c_str()))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == SYMBOL && t2 == SYMBOL) {			// SYMBOL < SYMBOL
			if (atoi(s1.c_str()) < atoi(s2.c_str()))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}

	}
	else if (token == GT_OP) {		// --> real number만 가능.
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
		if (atoi(p1.c_str()) > atoi(p2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}
	else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT < SYMBOL
		if (atoi(p1.c_str()) > atoi(s2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}
	else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL < DIGIT
		if (atoi(s1.c_str()) > atoi(p2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}
	else if (t1 == SYMBOL && t2 == SYMBOL) {			// SYMBOL < SYMBOL
		if (atoi(s1.c_str()) > atoi(s2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}

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
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT < SYMBOL
			if (atoi(p1.c_str()) >= atoi(s2.c_str()))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL < DIGIT
			if (atoi(s1.c_str()) >= atoi(p2.c_str()))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}
		else if (t1 == SYMBOL && t2 == SYMBOL) {			// SYMBOL < SYMBOL
			if (atoi(s1.c_str()) >= atoi(s2.c_str()))
			{
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else
			{
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;
			}
		}

	}
	else if (token == LOE_OP) {

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
		if (atoi(p1.c_str()) <= atoi(p2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}
	else if (t1 == INT_LIT && t2 == SYMBOL) {		// DIGIT < SYMBOL
		if (atoi(p1.c_str()) <= atoi(s2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}
	else if (t1 == SYMBOL && t2 == INT_LIT) {		// SYMBOL < DIGIT
		if (atoi(s1.c_str()) <= atoi(p2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}
	else if (t1 == SYMBOL && t2 == SYMBOL) {			// SYMBOL < SYMBOL
		if (atoi(s1.c_str()) <= atoi(s2.c_str()))
		{
			cout << "> T" << endl;
			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else
		{
			cout << "> F" << endl;
			token = lex(); //닫는 괄호 처리
			return FALSE;
		}
	}

	}
	else if (token == STRINGP) {
		string s;
		map<string, SETQval>::iterator i;

		token = lex();
		if (token == DOUBLE_QUOTE) {		// String 직접 들어오는 경우
			token = lex();
			token = lex();
			if (token != RIGHT_PAREN) {
				while (token != DOUBLE_QUOTE) {
					token = lex();
				}
				cout << "> T" << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else {
				cout << "> F" << endl;
				token = lex(); //닫는 괄호 처리
				return FALSE;

			}
		}
		else {	// String이 symbol로 들어오는 경우
			if (token == SYMBOL) {
				s += lexeme;
				token = lex();
				if (token == RIGHT_PAREN) {
					string findsym(s);
					i = FindSymbol(findsym, 2);

					if (i->second.val_type == STRING) {
						cout << "> T" << endl;
						return FALSE;

					}
					else
					{
						cout << "> F" << endl;
						return FALSE;

					}
				}
				else
				{
					cout << "> F" << endl;
					while (token != RIGHT_PAREN) token = lex();//문장 전체 오류처리
					if (nextChar == ')') token = lex();
					return FALSE;

				}
			}
			else
			{
				cout << "> F" << endl;
				while (token != RIGHT_PAREN) token = lex();//문장 전체 오류처리
				if (nextChar == ')') token = lex();
				return FALSE;

			}
		}

	}
	else if (token == LENGTH) {		// 파라미터는 하나만 가능
		token = lex();

		if (token == QUOTE) {		// 리스트인 경우.
			token = lex();	// 시작괄호
			int lp = 0;//여는 괄호의 개수
			int rp = 0;//닫는 괄호의 개수
			int count = 0;//원소의 개수

			if (token == LEFT_PAREN)
				lp++;
			else if (token == RIGHT_PAREN)
				rp++;

			token = lex();
			int inner = 0; //중복 괄호의 안쪽에 있는 원소의 개수
			bool inside = false; //중복 괄호의 안쪽에 있는지 확인하는 변수

			 while (lp > rp ) {

				 if (token == LEFT_PAREN)
					 lp++;
				 else if (token == RIGHT_PAREN)
					 rp++;

				count++;

				if (token == LEFT_PAREN) {//중복괄호가 있을 시
					inside = true;
				}
				else if (token == RIGHT_PAREN) { //중복괄호가 끝나면
					count = count - inner; //전체 원소의 개수에서 중복괄호 안에 있었던 원소의 개수를 뺀다.
					inside = false;
				}

				if (inside == true) {
					inner++;
				}
				else 
					inner = 0;

				token = lex();
			}

			cout << count - 1 << endl;
			return TRUE;

		}
		else if (token == DOUBLE_QUOTE) {		// 스트링인 경우
			token = lex();
			string s = lexeme;
			token = lex();
			while (token != DOUBLE_QUOTE) {
				s += " ";
				s += lexeme;
				token = lex();
			}
			cout << s.length() << endl;

			token = lex(); //닫는 괄호 처리
			return TRUE;
		}
		else if (token == SYMBOL) {		// 심볼이 숫자는 불가. 스트링이거나 리스트면 가능.
			string s;
			int type;
			map<string, SETQval>::iterator i;
			string findsym(lexeme);
			i = FindSymbol(findsym, 2);
			s = i->second.val;
			type = i->second.val_type;

			if (type == INT_LIT || token == INT_LIT)		// 숫자 불가
				return FALSE;
			else  if (type == LIST){		// 심볼의 값이 스트링이거나 리스트인 경우
				int count=1;
				for (int i = 0; s[i]; i++) {
					if (' ' == s[i])
						count++;
				}

				cout << count << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}
			else if (type == STRING) {
				cout << s.length() << endl;
				token = lex(); //닫는 괄호 처리
				return TRUE;
			}

		}

	}
	else { //여기에 계속 다른 연산 추가

	}
}
//MEMBER연산에 쓰이는 함수.
//eval에 넣으면 너무 길어져서 따로 뺌.
string ExcuteMEMBER(int token) {
	token = lex();
	string syml;
	/*
	찾아야 하는 원소 스트링 형태로 저장하기
	*/
	if (token == QUOTE) { //찾아야하는 원소가 '붙은 심볼일때
		token = lex();
		string s(lexeme);
		syml = s;
		token = lex();
	}
	else if (token == SYMBOL) { //찾아야하는 원소가 심볼일때
		string s(lexeme); //찾아야 하는 원소의 symbol
		map<string, SETQval>::iterator it = FindSymbol(s, 2);
		if (it != symbols.end()) {
			syml = it->second.val;//찾아야 하는 원소의 symbol안에 저장된 값
			token = lex();
		}
		else {
			cout << "Symbol " << s << " is not bound : MEMBER" << endl;
			while (token != RIGHT_PAREN) token = lex();
			if (nextChar == ')') token = lex();//문장 전체 오류처리
			return "";
		}
	}
	else { //원소 형태가 아닐때
		cout << "Syntax Error : MEMBER" << endl;
		while (token != RIGHT_PAREN) token = lex();
		if (nextChar == ')') token = lex();//문장 전체 오류처리
		return "";
	}
	/*
	주어진 리스트 안에서 찾기
	*/
	if (token == QUOTE) { //'로 시작하는 리스트 중에서 찾기
		token = lex();
		if (token == LEFT_PAREN) {
			vector<string> list;
			token = lex();
			while (token != RIGHT_PAREN) { //리스트를 스트링 벡터로 변환하기
				if (token == QUOTE) { // '붙은 심볼일때
					token = lex();
					string a(lexeme);
					list.push_back(a);
				}
				else {//심볼인 경우
					string symbol(lexeme);
					map<string, SETQval>::iterator it = FindSymbol(symbol, 2);
					if (it == symbols.end()) {
						list.push_back(symbol);
					}
					else {
						list.push_back(it->second.val);
					}
					
				}
				token = lex();
			}
			vector<string>::iterator i = find(list.begin(), list.end(), syml); //스트링 vector로 변환한 리스트에서 찾아야 하는 원소 찾기
			if (i != list.end()) { //찾은 위치부터 끝까지 자르기
				string result = "";
				while (i != list.end()) {
					result += (*i);
					result += " ";
					i++;
				}
				result.pop_back();
				cout << ">(" << result << ")" << endl; //출력
				token = lex(); //닫는 괄호 처리
				return "'(" + result + ")";
			}
			else {
				cout << "NIL" << endl;
				token = lex();//닫는 괄호 처리
				return "";
			}
		}
		else { //'다음에 (가 안올때 : 리스트가 아닌 것으로 판단하여 오류 처리
			cout << "Not a list Error : MEMBER" << endl;
			while (token != RIGHT_PAREN) token = lex();
			if (nextChar == ')') token = lex();//문장 전체 오류처리
			return "";
		}
	}
	else if (token == SYMBOL) { //변수로 표현된 리스트 중에서 찾기
		string listname(lexeme); //입력받은 리스트의 변수 이름
		map<string, SETQval>::iterator iter = FindSymbol(listname, 2); //변수 이름을 통하여 리스트를 찾기
		if ((iter != symbols.end()) && (iter->second.val_type == LIST)) { //존재하는 리스트일 때
			string listcontent = iter->second.val;//리스트의 내용은 스트링으로 저장되어있다.
			//string parsing
			size_t prev = 0;
			size_t curr = listcontent.find(' '); //공백문자를 delim으로 하여 파싱한다.
			while (curr != string::npos) {
				string substring = listcontent.substr(prev, curr - prev); //리스트의 원소를 앞에서부터 하나씩 자른다.
				if (syml.compare(substring) == 0) { //만약 찾고 있는 원소와 자른 원소가 같다면
					cout << "(" << listcontent.substr(prev, string::npos) << ")" << endl;//그 원소부터 리스트 끝까지 출력한다.
					token = lex();//닫는 괄호 처리
					return "'(" + listcontent.substr(prev, string::npos) + ")";
				}
				//찾고있는 원소와 같지 않다면 다음 원소를 파싱한다.
				prev = curr + 1;
				curr = listcontent.find(' ', prev);
			}
			if (listcontent.substr(prev, curr - prev).compare(syml) == 0) { //마지막 원소와 찾고 있는 원소가 같을 때
				cout << ">(" << syml << ")" << endl;
				return "'(" + syml + ")";
			}
			else { //끝까지 찾았는데 찾는 원소가 없을 때
				cout << "NIL" << endl;
				token = lex(); //닫는 괄호 처리
				return "";
			}
		}
		else { //존재하지 않는 리스트일때
			cout << "List Not exist Error : MEMBER" << endl;
			while (token != RIGHT_PAREN) token = lex();
			if (nextChar == ')') token = lex();//문장 전체 오류처리
			return "";
		}
	}
}

//CARCDR연산에 쓰이는 함수.
//eval에 넣으면 너무 길어져서 따로 뺌.
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
				for (; cnt < vlen; cnt++) { //결과로 나온 리스트에서 첫 번째 원소 까지만 잘라내기 위함.
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
		else {

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
				while (1) { //결과로 나온 스트링에서 첫 번쨰 원소만 뺴고 잘라내기 위함.
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
//CONS연산에 쓰이는 함수.
//eval에 넣으면 너무 길어져서 따로 뺌.
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
		if (it == symbols.end()) {
			symbolExist = false;
		}
		else {
			frontlist = it->second.val;
		}
		
	}

	token = lex();
	if (token == QUOTE) {//리스트가 ' 형태로 주어질 때
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
			cout << "CONS Error : This Symbol dose not exist" << endl;
			return " ";
		}
		cout << "(" << frontlist << " " << backlist << ")" << endl;
		return frontlist + " " + backlist;
	}
	else if (token == SYMBOL) {//리스트가 심볼 형태로 주어질 때
		it = FindSymbol(lexeme, 2);
		if (it == symbols.end()) {
			token = lex();//닫는 괄호 처리
			cout << "Cons Error : This List does not exist." << endl;
			return " ";
		}
		else {
			backlist = it->second.val;
			token = lex(); //닫는 괄호 처리
			cout << "(" << frontlist << " " << backlist << ")" << endl;
			return frontlist + " " + backlist;
		}
	}
	else {

	}
}
//REVERSE연산에 쓰이는 함수.
//eval에 넣으면 너무 길어져서 따로 뺌.
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
//APPEND연산에 쓰이는 함수.
//eval에 넣으면 너무 길어져서 따로 뺌.
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
			if (it == symbols.end()) {
				symbolExist = false;
			}
			else {
				list.append(it->second.val);
			}
			
		}
		token = lex();
		if (token == RIGHT_PAREN) break;
		list.append(" ");
	}
	if (symbolExist == false) {
		cout << "APPEND Error : This Symbol dose not exist" << endl;
		return " ";
	}
	cout << '(' << list << ")" << endl;
	return list;
}
//ASSOC연산에 쓰이는 함수.
//eval에 넣으면 너무 길어져서 따로 뺌.
string ExcuteASSOC() {
	string key;
	string list;
	int pastToken;
	int nowToken;
	token = lex();
	if (token == QUOTE) { //쿼드로 키가 들어올 경우
		token = lex();
		key = lexeme;
	}
	else { //심볼로 키가 들어올 경우
		map<string, SETQval>::iterator it = FindSymbol(lexeme, 2);
		if (it == symbols.end()) {
			//남은 토큰들 다 읽고 끝내기
			int cnt = 1;
			while (cnt > 0) {
				token = lex();
				if (token == LEFT_PAREN) cnt++;
				if (token == RIGHT_PAREN) cnt--;
			}
			cout << "No Symbol";
			return " ";
		}
		key = it->second.val;
	}

	token = lex(); // 쿼트 읽기
	lex(); //좌괄호처리

	//리스스들 읽기
	pastToken = lex(); //좌괄호
	nowToken = lex(); // 첫번쨰 리스트의 첫번쨰 원소
	while (!((pastToken == RIGHT_PAREN) && (nowToken == RIGHT_PAREN))) {
		if (lexeme == key) {
			list.append(key);
			while (nowToken != RIGHT_PAREN) {
				pastToken = nowToken;
				nowToken = lex();
				if (nowToken == RIGHT_PAREN) break;
				list.append(" ");
				list.append(lexeme);
			}
		}
		else {
			while (nowToken != RIGHT_PAREN) {
				pastToken = nowToken;
				nowToken = lex();
			}
		}
		pastToken = nowToken;
		nowToken = lex(); //우괄호 인식
		pastToken = nowToken;
		nowToken = lex(); //그다음 리스트의 첫번쨰 원소
	}

	cout << '(' << list << ')' << endl;
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
		nextToken = GOE_OP;
		break;
	case '\'':
		nextToken = QUOTE;
		break;
	case '\"':
		nextToken = DOUBLE_QUOTE;
		break;
	case '#':
		nextToken = HASHTAG;
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
	if ((nextChar = getc(in_fp)) != EOF) {
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
/* lex - a simple lexical analyzer for
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
		else if (stricmp(lexeme, "member") == 0) {
			nextToken = MEMBER;
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
		else if (stricmp(lexeme, "CONS") == 0) {
			nextToken = CONS;
		}
		else if (stricmp(lexeme, "REVERSE") == 0) {
			nextToken = REVERSE;
		}
		else if (stricmp(lexeme, "APPEND") == 0) {
			nextToken = APPEND;
		}
		else if (stricmp(lexeme, "ATOM") == 0) {
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
		else if (stricmp(lexeme, "ASSOC") == 0) {
			nextToken = ASSOC;
		}
		else if (stricmp(lexeme, "REMOVE") == 0) {
			nextToken = REMOVE;
		}
		else if (stricmp(lexeme, "SUBST") == 0) {
			nextToken = SUBST;
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
		else if (strcmp(lexeme, "\"") == 0) {
			nextToken = DOUBLE_QUOTE;
		}
		else if (stricmp(lexeme, "LENGTH") == 0) {
			nextToken = LENGTH;
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
			if (charClass == DIGIT) { //양의 정수
				addChar();
				getChar();
			}
			else if (charClass == UNKNOWN) {
				lookup(nextChar);
				if (nextToken == POINT) { //양의 실수
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
		while (nextChar == ';') {//주석처리
			while (nextChar != '\n') getChar();
			getChar();
		}
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


//symbol을 symbols map(심볼과 관련 정보를 보관하는 자료형)에서 찾는 함수.
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