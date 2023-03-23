# Web of Trust
A decentralized web of trust is a graph between users of a community. Nodes and links of the graph are maintained by single users in "node objects", without a central authority, and without a central database.

## Docs
See [doc/README.md](doc/README.md)

See [doc/command_help.txt](doc/command_help.txt) for the help output from the program.

## Build
cmake -S . -B build
cmake --build build

## Doxygen docs
cmake --build build --target doxygen
x-www-browser build/doc/html/index.html

## Tests
See [test/README.md](test/README.md)

## Dependences
```
npm install yaml@2.1.3
npm install -g quicktype
quicktype -s schema src/schema.json -l C++ -o src/generated/node.cpp

apt install nlohmann-json-dev

wget https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp
```
