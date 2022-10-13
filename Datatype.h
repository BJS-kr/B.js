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

auto isString(any)->bool;
auto isNumber(any)->bool;
auto isGetVariable(any)->bool;
auto isFunctionExpression(any)->bool;
auto isArray(any)->bool;
auto isObject(any)->bool;
auto isConsole(any)->bool;

auto toNumber(any)->double;
auto toString(any)->string;
auto toBool(any)->bool;
auto toFunctionExpression(any)->FunctionExpression*;
auto toArray(any)->ArrayLiteral*;
auto toObject(any)->ObjectLiteral*;
auto toConsole(any)->Console*;
auto toGetVariable(any)->GetVariable*;

auto doubleToString(double)->string;

ostream& operator<<(ostream&, any&);
string& operator+=(string&, any&);

#endif