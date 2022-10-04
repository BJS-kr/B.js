#ifndef INFO
#define INFO

#include "Color.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::make_pair;
using std::string;

auto info(string s) {
    cout << def << s << endl;
}

auto error(string s) {
    cout << red << s;
    exit(1);
}

#endif