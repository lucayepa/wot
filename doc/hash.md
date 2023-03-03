# How the hash of an object is calculated

# TOML file
Implementation should define if the TOML file should be canonicalized before of generating the hash. The current behaviour is described in <canonic.md>

The node object is represented as a TOML file. The field _signature.hash_ is the lowercase hex-encoded sha256 hash of the TOML file without the last two lines.

Note: a TOML file is by definition UTF-8.

To generate _signature.hash_ of the node object that is in $FILE:

```bash
FILE=trust.toml
SIGNED_FILE=$FILE.signed
cat $FILE | head -n -2 > $SIGNED_FILE
HASH=`cat $SIGNED_FILE | sha256sum | cut -f1 -d' ' | tr -d '\n'`
echo "hash = \"$HASH\"" >> $SIGNED_FILE

export `grep profile.key $SIGNED_FILE | tr -d ' ' | tr -d '"' | tr '.' '_'`

WALLET=YOUR_WALLET_PATH
SIG=`echo -n $HASH | electrum --offline -w $WALLET signmessage $profile_key -`
echo "sig = \"$SIG\"" >> $SIGNED_FILE
```

## Hash verify
To verify that the node object in obj.toml contains the right hash:

```bash
FILE=trust.toml.signed
export `egrep '^hash = "' $FILE | tr -d ' ' | tr -d '"'`
HASH=`cat $FILE | head -n -2 | sha256sum | cut -f1 -d' ' | tr -d '\n'`
echo "$HASH should be equal to $hash"
```

# JSON file
_signature.hash_ is the lowercase base64 sha256 of the UTF-8 JSON string containing all the fields of the node objects, but the _signature_ table. The string is first canonicalized as explained in canonic.md, then the hash is calculated.
