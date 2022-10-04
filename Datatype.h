#ifndef DATA_TYPE
#define DATA_TYPE

#include<any>
#include<string>
#include<iostream>

using std::any;
using std::string;
using std::any_cast;
using std::ostream;

bool isString(any);
double toNumber(any);
bool isNumber(any);
string toString(any);
// cout을 위한 << 연산자 오버로딩
ostream& operator<<(ostream&, any&);

#endif