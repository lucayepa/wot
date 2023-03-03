# Identity implementation - bitcoin
This implementation of identity uses bitcoin addresses as identity keys. The identity key is a bitcoin address that has never been spent on the Bitcoin time chain.

Any implementation should specify:
* How to generate a key
* How to revoke a key
* How to change a key with a link to a new key
* How to verify if a key is valid, and, if not, how to check if it has been changed to a new one
* Signature: how to use the key to sign the hash of a node object
* Signature: how to verity if the hash of a node object has been signed by a key

## Create a key
* Generate a new bitcoin address and save the private key
* The address should be newly generated and never used as input of a transaction (spent)

## Revoke a key
* Spend some satoshis from a UTXO containing the bitcoin address, and burn it by sending it to an address without corresponding private key

## Identity key change
In order to change the private key of a user, a special bitcoin transaction should enter the bitcoin time chain. The transaction should contain the legacy id of the user as an input and should have only one output. The output address of the transaction will be the new identity of the user. Depending on the rules of the community, the legacy address can still be considered valid for a certain time period, or forever, provided that it is considered valid only for the link received by other users, and not for generate new objects.

Steps:

* Generate a new identity
* Send some value to the legacy id
* Spend the UTXO by sending the value to a single address (no rest)
* The new address will be the new identity key

## Identity validity verification
Verify that the identity key in _profile.key_ is still valid:

If the identity key has been used as an input in a bitcoin transaction, the identity key is NOT valid. In this case, the process should check if there is a single output transaction that spends a UTXO of the address. If so, the address receiving the transaction is the new identity of the user. Depending on the rules, every link that pointed to the former identity key, can be considered as pointing to the new identity.

## Signature algorithm
The signature algorithm will be the one used in Bitcoin.

With openssl:
```bash
#TODO: This is not ok (maybe better to remove)
#Bitcoin signature with openssl using key.pem as private key:
printf $HASH | xxd -r -p | sha256sum -b | xxd -r -p | sha256sum -b | xxd -r -p > /tmp/a
openssl pkeyutl -inkey key.pem -sign -in /tmp/a -pkeyopt digest:sha256 | xxd -p -c 256
```

With electrum: see signature document for sign and verify process.

## Notes

### X-pub instead of a transaction spending
As a side note, a different implementation is possible with users that publish an x-pub that can be used as a method for changing the identity key. 

### Switch from one implementation to another
If permitted, or requested, by the community, a user can switch from this identity key implementation to another. To do so, the user should publish on the Bitcoin time chain a transaction spending a UTXO from the identity key of this implementation. The transaction should have more than one output and one or more `op_return` containing the public key of the other implemtantion (for example, a Nostr protocol key).

