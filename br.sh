#! /usr/bin/env bash

eval /usr/bin/clang++ \
-std=c++17 \
-fcolor-diagnostics \
-fansi-escape-codes \
-g ./Main.cpp ./Scanner.cpp ./Parser.cpp ./Interpreter.cpp ./Datatype.cpp ./Token.cpp \
-o ./BJS && ./BJS;