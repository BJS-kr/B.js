#pragma once
#include<any>
#include<string>
#include<iostream>

using std::any;
using std::string;
using std::any_cast;
using std::ostream;

bool isString(any value);
string toString(any value);
// cout을 위한 << 연산자 오버로딩
ostream& operator<<(ostream& stream, any& value);