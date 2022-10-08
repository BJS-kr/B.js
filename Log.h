#ifndef INFO
#define INFO

#include "Color.h"
#include <string>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::make_pair;
using std::string;
using std::stringstream;

auto info(string s) {
    cout << def << s << endl;
}

auto error(string s) {
    cout << red << s;
    exit(1);
}

auto lexical_environment_to_string(LexicalEnvironment* lexical_environment) {
    const void * address = static_cast<const void *>(lexical_environment);
    stringstream ss;
    ss << address;

    return ss.str();
}

#endif