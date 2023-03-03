# Canonicalization

There are two types of objects: JSON and TOML. _signature.hash_ is calculated on a canonicalized object.

The node object MUST contain the fields defined in the JSON schema, even if it is a TOML object.

In case of JSON object, the fields must be in the order defined in the schema.

There are no optional fields. All the strings can be equal to the empty string, but three.

_profile_key_, _signature.hash_ and _signature.sig_ cannot be empty strings

_serial_ must be greater than zero.

# Canonicalization of a JSON node object

## White spaces and \n
Whitespace between JSON tokens are removed. Any carriage return is removed.

## Strings
String are serialized as defined in RFC 8785 - 3.2.2.2. Optional string fields not defined by the user but defined in the main sorting order below, will be encoded as "". The double quote character (") is not permitted in a string (this is defined in the main generic document).

## Hash tables
Hash tables should be lexically sorted by key, as explained in RFC 8785 - 3.2.3. No hash element is optional. If some field is not defined by the user, it is considered as an empty hash, empty array, or empty string. All the integers are mandatory. The only accepted type of hash table keys are ascii strings.

## Integers
Integers are signed decimals. The + in front of a positive integer, if present, will be removed. There are no optional integers. No zero padding allowed.

## Helpers
Internal links of the object to the two helpers hashes _defaults_ and _rules_ should be solved according to the main definition of a generic node object. Values should be inserted in the JSON object

## Notes
For anything not written here, please use RFC 8785: <https://www.rfc-editor.org/rfc/rfc8785>

# Canonicalization of a TOML node object

We assume that a TOML object is meant to be read by humans. So the idea is not to modify it when possible. _Suggest_ is used in this document to define actions that can be suggested by a TOML editor.

## Comments and \n
Comments and empty lines will not be removed.

## Strings
Strings will be between double quotation marks ("). The same character is not allowed in a string as stated in the generic document.

## Hash tables
It is suggested, that every element of a hash tables will be listed using the dotted notation. No square parentesis notation.

It is enforced, that every element is present.

A TOML editor must substitute any value not defined by the user, but defined in the schema, with an empty strings ("").

Keys of _profile_, _sources_ and _trust_ will have ascii pre-defined keys. These keys do not use quotation marks.

The hash table _signature_, that is added at the end of the document after the calculation of the hash, is added with the square parenthesis notation:

```
[signature]
hash = "..."
sig = "..."
```

## Integers
Integers are signed decimals. The '+' in front of a positive integer is not used. When an optional integer is not defined, it will be set to zero. No zero padding allowed.

## Arrays
Should be suggested to have arrays on a single line, without whitespaces. They should be lexically sorted.

Exception: the _trust_ array must be written with the double square parenthesis notation:

```
[[trust]
to = ...
since = ...
...
[[trust]]
to = ...
since = ...
...
```

## Helpers
Internal links of the object to the two helpers tables _defaults_ and _rules_ can be solved and removed.
