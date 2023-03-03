# Test

```
# Create a test electrum wallet
electrum restore -w /tmp/wallet_wot_test "hope chase slush laptop wrestle main trouble project dismiss chalk click drift"

cp -a doc/examples/example.toml test/generated/node1.toml
bin/wot solve < test/generated/node1.toml > test/generated/node1s.toml
diff test/node1s.toml test/generated/node1s.toml

#Simple verification
bin/wot verify < test/generated/node1.toml | grep ok

#Remove signature
head -n -2 test/generated/node1.toml > test/generated/node1-nosign.toml
bin/wot verify < test/generated/node1-nosign.toml | grep NOT

#Add wrong hash and sig
echo "hash = \"\"" >> test/generated/node1-nosign.toml
echo "sig = \"\"" >> test/generated/node1-nosign.toml
#Verify everything but hash and signature
bin/wot --force-accept-hash --force-accept-sig verify < test/generated/node1-nosign.toml | grep ok

#Sign
export WALLET=/tmp/wallet_wot_test
bin/wot sign-toml < test/generated/node1.toml 2> /dev/null > test/generated/node1s.toml
bin/wot verify < test/generated/node1s.toml | grep ok

#Hash on file
grep hash test/generated/node1s.toml
#Hash CLI
head -n -2 test/generated/node1s.toml | sha256sum

#Signature on file
grep sig test/generated/node1s.toml
#Signature CLI
echo -n 7fba4f494e78dd102c6fad4d6ab360e8618a00195889e162701ffac0addc2b52 | electrum --offline verifymessage bc1qa37y5tnfcg84k5df3sejn0zy2htpax0cmwyzsq IE9TDJi7ECSljHGCKB6ER2oRefjgReNuc/lPlzFtqEuHK2hX1z3ZRotx0VdqMPWwBqktOKFWSXFvBI8MSlVfd5Q= -

#Objects internal db
unset WALLET
bin/wot add < test/generated/node1s.toml
bin/wot ls

#Signatures cache
bin/wot ls-sig
bin/wot rm-sig 7fba4f494e78dd102c6fad4d6ab360e8618a00195889e162701ffac0addc2b52
bin/wot ls-sig
bin/wot verify < test/generated/node1s.toml | grep ok
bin/wot ls-sig

#List of all rules contained in nodes
bin/wot ls-rules
```

## Other tools

To test toml to json conversions: <https://pseitz.github.io/toml-to-json-online-converter/>.

To compact a json object: `cat test/object.json.helped | jq -c > test/object.json.helped.compacted`
