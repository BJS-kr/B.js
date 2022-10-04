#ifndef INFO
#define INFO

#include "Color.h"
#include <string>
#include <iostream>

using std::cout;
using std::endl;
using std::make_pair;
using std::string;

// static Color::Modifier blue(Color::FG_BLUE);

map<string, string> log_stash = {
        {"info" , ""},
        {"error", ""}
    };

// auto info(string s) {
//     auto original = log_stash.find("info")->second;
//     log_stash.insert({"info", original + "\n" + s});
// }

// auto error(string s) {
//     auto original = log_stash.find("error")->second;
//     log_stash.insert({"error", original + "\n" + s});
// }

auto print_log() {
    for (auto&& log:log_stash) {
        cout << log.first << ": " << endl;
        cout << log.second << endl;
    }
}

auto info(string s) {
    cout << s << endl;
}

#endif