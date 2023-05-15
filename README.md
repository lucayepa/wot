# Web of Trust
A decentralized web of trust is a graph between users of a community. Nodes and links of the graph are maintained by single users in "node objects", without a central authority, and without a central database.

## Using `wot`

```
Usage: wot [OPTIONS] command [parameter]

Commands:
  help                     This help message
  sign-toml                Sign a node in TOML format
  sign                     Sign a node
  verify                   Verify a node object
  add                      Add a node object to the internal db
  ls-on                    List all the `on` found in the links of the objects of the internal db
  ls                       List of the objects in the internal db that match the requested filters.
  add-sig                  Add a trusted signature to the internal db
  rm-sig                   Remove a trusted signature from the internal db
  ls-sig                   List all the knwon signatures of the internal cache
  view                     View a node from internal db
  template-options-file    Print a template options file
  compose                  Compose a new node object
```

Create an electrum test wallet to sign messages (optional)
```
  electrum restore --offline -w /tmp/wallet_wot_test "hope chase slush laptop wrestle main trouble project dismiss chalk click drift"
  export WALLET=/tmp/wallet_wot_test
```

Compose a node object and add it to the local database
```
  wot compose --start | \
  wot compose --circle circle2 --profile-name name2 --serial 15 | \
  wot compose --unit "USD" --value 50 --since 3 --to aa --on a --on b | \
  wot compose --profile-key bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq | \
  wot compose --version v0.1 | \
  wot sign | wot add
```

List all the node objects contained in the local database:
```
  wot ls
```

View a single node:
```
  wot view 0c70a23dadc5b8c7467c3a02b812cd130d45b8d080bffed86ca1141603578861
```

## Docs
See [doc/README.md](doc/README.md)

See [doc/command_help.txt](doc/command_help.txt) for the help output from the program.

## Build and run
```
cmake -S . -B build
cmake --build build
build/src/wot help
```

## Technical documentation
Technical docs are generated with Doxygen: `cmake --build build --target doxygen && x-www-browser build/doc/html/index.html`

## Tests
See [test/README.md](test/README.md)
