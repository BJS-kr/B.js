#include <map>
#include <string>
#include <iostream>
using std::string;
using std::map;
using std::cout;
using std::endl;

map<string, int> M = {
{"god", 1}
};

int main() {
    cout << M.at("god") << endl;
    if (M.find("gdo") == M.end()) {
        cout << "key not found" << endl;
    }
}