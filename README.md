# Web of Trust
A decentralized web of trust is a graph between users of a community. Nodes and links of the graph are maintained by single users in "node objects", without a central authority, and without a central database.

## Docs
See [doc/README.md](doc/README.md)

See [doc/command_help.txt](doc/command_help.txt) for the help output from the program.

## Build
```
cmake -S . -B build
cmake --build build
```

## Doxygen docs
```
cmake --build build --target doxygen
x-www-browser build/doc/html/index.html
```

## Tests
See [test/README.md](test/README.md)
