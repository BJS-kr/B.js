#ifndef DATA_TYPE
#define DATA_TYPE

#include<any>
#include<string>
#include<iostream>
#include <sstream>
#include <iomanip>
#include "Node.h"

using std::any;
using std::string;
using std::any_cast;
using std::ostream;
using std::to_string;
using std::fixed;
using std::setprecision;
using std::stringstream;

bool isString(any);
double toNumber(any);
bool isNumber(any);
string toString(any);
bool toBool(any);
bool isFunctionExpression(any);
auto toFunctionExpression(any)->FunctionExpression*;
bool isArray(any);
auto toArray(any)->ArrayLiteral*;
bool isObject(any);
auto toObject(any)->ObjectLiteral*;
bool isConsole(any);
auto toConsole(any)->Console*;
bool isGetVariable(any);
auto toGetVariable(any)->GetVariable*;
auto doubleToString(double)->string;

// cout을 위한 << 연산자 오버로딩
ostream& operator<<(ostream&, any&);
string& operator+=(string&, any&);

#endif