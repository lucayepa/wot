# Web of Trust
A decentralized web of trust is a graph between users of a community. Nodes and links of the graph are maintained by single users in "node objects", without a central authority, and without a central database. [Here](../doc/examples/example.toml) is an example of node object.

## Design notes

* A user that wants to receive a link can do it by just communicating a public key to other users.
* A user that wants to have links that connect to other users, needs to maintain one or more node objects.
* There is a program that helps, but a user can create and maintain node objects using the command line.
* Signatures and verifications can happen on a separate computer
* The specification is general. Different implementations can use different signing algorithms

## FAQ - general

* Is there a token? Is there a way to get rich quick?
  * No
* Is there a blockchain?
  * No
* So, what is the web of trust?
  * It is a distributed graph that contains information on the relationships of a group of people. Any user can maintain one or more nodes of the graph, with links to other nodes.
* Is it a P2P network?
  * Not in the sense of a network to communicate between peers. The web of trust is based on node objects. Users can send information on their node objects using any known method of communication.
* What is a node?
  * A node object contains the information of a user, along with links to other users. The links contain a value of how much a user trust the other users.
* Where is the web of trust?
  * Any user can have node objects of other users on a hard disk, on a USB key, or on any other support.
* Node objects are public or private?
  * Anyone can decide to share any node objects with other users. For example, some communities can mandate that the node objects are public to access some services, or are contained in a central server. Other communities can suggest to their users to present a set of node object to attend an event.
* This means that any community can use the web of trust as needed. Correct?
  * Exactly. The web of trust is a way of maintaining relationships. It can be used by communities as they want.
* Do I need to be online to participate in a web of trust?
  * No. A web of trust object is given by a user to another user and is used at will. Without any need to be online. A web connection can be useful to check if there are new versions of an object by a user, but only if the user set some _sources_ in the object node.

## FAQ - identity key
This section assumes that the user is part of a community that uses the bitcoin identity method. Other identity implementations are possible, like GPG, Nostr, IPNS, ...

* Is there a username?
  * Each user has an identity key. The identity key is used to sign the nodes maintained by the user.
* How is the identity key generated?
  * The identity key is a bitcoin address. It can be created using any software that generates a bitcoin address, and uses it to sign, or verify, a message. For example Electrum.

## FAQ - link
* What is the meaning of a link from one user to another?
  * The meaning is defined by the community the users are in. For example, a community can decide to consider the links as a "sponsorship" from one user to another.
* Can different communities use the same web of trust?
  * Yes. For example, a community that wants to play tennis every Thursday, can share parts of a web of trust with a community that manages meetings to trade collectible cards. And both can use the web of trust already in place for a community that manages a bitcoin-to-fiat exchange market. 
* What if a user is a member of multiple communities?
  * Every link between users contains an array named _on_ that defines against what "tags", or "rules" the user is trusting the other users. This field can be used as a sort of agreement, meaning that a user is linked to another user with an agreement. For example, a community that manages a market can define that a user is liable for another user up to the value contained in the link between the users.

## FAQ - node
* What is the content of a node?
  * A node contains your profile, the ways to find an updated version of the same object, the trust links to other users, and a signature.
* Is there a profile?
  * Yes. The information on your profiles is contained in your node objects.
* Objects? Plural? Does this mean that a user can have more than one node?
  * Yes. A single identity key can sign and maintain multiple nodes. Any node contains a field named _circle_. Any identity key can manage how many nodes as needed, provided that they have different _circle_ fields. For example, a user can have a node for circle _work_ and another for circle _family_. The two nodes will have different profiles. So, for example, there can be different _about_ for work and friends, or different photos. Even different names.
* Is it mandatory to separate the node into multiple objects?
  * No. A user can have all the trust links in the same object. Obviously, this means that someone that has the node, can see any trust link of the node. If a user wants to conceal part of the links to someone, then it is needed to send different files to different persons. This means that a certain file will contain only some trust links. The _circle_ field permits the separation of different files inside the same namespace.
* A node contains trust links. What if I change my mind and I don't trust a user anymore?
  * Since any link from one user to another user contains a value, the easiest way to nullify a link, is to add a new link to the same person, that contains a negative value, so that the sum of the two numbers will be zero.
* Wait. This means that a single node can contain multiple links to the same person. Correct?
  * Yes. A single node can contain multiple links to the same user. For example, a user can "trust" another user for 100 USD while playing poker, and 25 EUR to pay a shared padel court.
* What if I don't want to have the person in my node at all?
  * This can happen, for example, if a user doesn't want to share with other users that a trust link ever existed. In this case, you have to publish a new version of your node. Any node contains a serial number. Nodes with a new serial number invalidate the nodes of the same user, and the same circle, with a old serial number.
* Publish? How can I publish a node object?
  * This depends on the rules of your community. Any node object contains a list of _sources_. These _sources_ are supposed to be checked by persons that have the node object, to check if a new version of the same node object has been published by the user. Sources are optional. For example, if a user wants to share a node object privately, the node object can be sent as an attachment over email.
* How can I choose a serial number?
  * You can choose any integer, but sometimes the community you are part of asks you to use some serial numbers. For example, a community can ask to use the number of the last bitcoin block mined (as far as you know). The important part here is that the _serial_ number is an integer, and new versions of the same object must contain a greater serial number.

## FAQ - operations
This section assumes that the user is part of a community that uses the bitcoin identity method. Other identity implementations are possible, like GPG, Nostr, IPNS, ...

* How can I start? A friend of mine trusts me. How can we insert this information into the web of trust?
  * You don't need to maintain any node object to receive a trust link from someone. So you simply generate a bitcoin address, and give the bitcoin address to your friend. Your friend will send you the node object containing the trust link to you. You can show this node object to anyone that you want to deal with.
* Wow. What if instead, I want to give a trust link to someone?
  * If you want to have a trust link from your node to another node, you need not only an identity key but even to generate a node object.
* Do I need to download something?
  * No. A node object can be a TOML file. [Here](../doc/examples/example.toml) is a simple file. You can edit the file and sign it. Then you can send it to your friends, or to the administrator of your community, or you can publish it as you want. The file contains the trust links that connect you to your friends.
* How do I sign the node file?
  * Every node file contains a hash and a signature. The hash is calculated using sha256 on the node object. The hash is then signed using a Bitcoin wallet, and the signature is added to the file. This can be done with a few commands. You can find the way of signing a TOML node object <here>.
* What if I prefer to use a program to generate a valid node?
  * In this case, you are in the right place. This package contains a program that generates valid nodes. Since the program is not a bitcoin wallet and does not contain any form of private keys, you still need to use an external program to sign the node. The program will ask for the signature and will explain how to do it step by step.
* How do I "hold" all the node objects that I have?
  * You can simply save them in one or more text files on disk. You can also use the software provided in this package to hold and maintain the node objects.

## FAQ - implementation
This section assumes that there are different communities using different consensus methods on how to use the web of trust. For the moment it can be skipped unless you want to help with the porting to different identity algorithms.

* What if I have node objects from different communities, that use different algorithms for identity keys?
  * Every node object contains a field named _version_. It contains a reference to the implementation of that node object.
* What if I am in different communities, having different implementations?
  * If this case you need to maintain a node for every implementation. Since nodes are identified by identity keys, different identity keys are different nodes.

## Communities
A community is a group of users that use the web of trust. The community agrees on some rules that define a consensus on accepted node objects.

Examples of constraints that can be added by a community:

* Only node objects containing a timestamp in a certain range are accepted.
* The community mandates an increasing timestamp by the same user.
* Only certain _link_ _units_ are accepted.
* Only _link_ _values_ in a certain range are accepted.
* Accepted ranges can be different based on the number and quality of links received by the user.

For more information, and an example of a community, please check the rules of the Bitcoin Italia Market at the following address: <https://gist.github.com/lucayepa/3bec3ba83fcc9a576aa79a7f16326be3>

If you want to build a community, you are supposed to:

* define an implementation of this web of trust data structure.
* define a set of extra rules that your users should follow when they create and edit their node objects.
* define a way of sharing the node object between members of your community.

In order to build a community based on the web of trust, every user should share with the rest of the community, or with only some other users, a node object. In order to be valid for a certain community of users, a node object should be valid against three different layers of rules.

1. the generic web of trust object definition, contained in this package
2. the implementation of the protocol, as decided by the consensus of the community. This defines the algorithm used for the identity key, the algorithm used for the signature, and the revoke method of the identity key.
3. rules of the community that define how to use the web of trust. For example, how and when a user should share some objects.

As a rule of thumb, implementations should define how the program should validate a single node object, while the community should define how a set of node objects can regulate the reletionships between users.

The communication between users is defined by the rules of the community and is considered outside of the scope of this document.

## Identity
Every user has a unique identity defined by an identity key (for example a Bitcoin address, an IPNS address, a GPG public key, or a Nostr public key).

Every user can create and maintain a set of node objects, signed by the current identity key of the user. Each node object contains a set of links to other users. Every link contains a timestamp, the identity key of the receiving user, a value, and a unit.

The same web of trust can be used by multiple communities, using different rules on the same set of data. For example, a community can use the link between users as a form of sponsorship of one user to the other.

The identity key is embedded in the node object and is used to sign the object itself.

Different people can have the same profile name defined in the node object. Also, an identity can have different profiles, one for each node object. Profiles and identities are separate things.

Every implementation (NOTE: at the moment there is only the bitcoin one) should specify:

* How to generate a key
    * Every implementation should state how to create a new identity key for a user. Users that do not have links directed to other users are supposed to have no node object. Usually, the implementation permits to generate an identity key without a node object. This also means that these users have no profile data.
* How to revoke a key
    * Implementation can specify a way of revoking the identity key. A revoked identity is not considered anymore in the web of trust and any node object signed with that key is considered invalid.
* How to change an identity key
    * At any time, the user can pick up a new identity key. The implementation should define a process to port the identity key. Communities should define a way to publish the information on the change of identity key to the users. Changing the identity key invalidates all the node objects that are public and contain an old identity key. It is useful to remove _links_ to users that are not trusted anymore.
* How to verify if a key is valid, and, if not, how to check if it has been changed to a new one
    * Every implementation can specify a way of verifying if the identity key of a user is still valid and if there is a valid port to a new identity key.
* Signature: how to use the key to sign a node object _signature.hash_
* Signature: how to verify if a node object _signature.hash_ has been signed by a key

## Node object
The element of the web of trust is the node object. Any node object follows the rules of a certain implementation. The node object is signed by the user as defined in the implementation and contains the links to the identity keys of the other users. A user can maintain multiple node objects. The object can be shared publicly, within a group of users, or sent privately. The object contains the name and the way the user wants to be contacted (_profile_), along with the suggested ways to find new versions of the same node object (_sources_).

The generic node can be seen [here](../doc/examples/example.toml).

Two hash tables are used to keep the node object as compact as possible. They are removed before signing the object. They are:

* _defaults_, if present, contains the values to be considered when a trust _link_ does not contain a certain field.
* _ref_, if present, contains references to versions of rules adopted by the community joined by the user, contained in the array _on_. Keys of this table are local only and can be used in the _on_ array of a _link_ of the _trust_ array.

_version_ refers to a document describing the implementation schema that this object adheres to.

No string can contain the double quote character (").

### Signature hash table
Every node object has a _hash_ that is calculated as defined in the document [hash.md](hash.md).

The node object is signed using the object _hash_ and the private key of the user, using the algorithm defined by the implementation. See [hash.md](hash.md).

### Serial number
_serial_ is an integer. Different implementations can use this field to validate the node object. A higher timestamp means newer versions of the node object (see "circle" below). Implementations can require the users to insert here the bitcoin time chain block height, the unix epoch, or a value like yyyymmdd.

### Circle: multiple objects by the same user
Every identity can maintain more than one node object. A field named _circle_ identifies the node object in the set of node objects maintained by a user. A user is supposed to have only one version of a node object having a certain _cirle_ at any time. A node object of the same user and the same _circle_ is supposed to be a newer version of the same node object, only if it contains a greater _serial_. Some implementations can mandate that the timestamps of the valid objects maintained by a single user at a certain time are the same. Implementations should use the _circle_ field only to check the unicity of the node object in the namespace of node objects of a user. So that the user can choose the names of the _circle_ field as desired.

Rationale. Since every identity can maintain multiple node objects, without this field, any edit containing a new _serial_ invalidates all the other objects signed by the same user. Using this unique name to identify a node object, permits us to consider it expired when a new one has a new _serial_, only if it has the same _circle_ object name.

A node object having the same _id_ and the same _circle_ string, with a greater _serial_ is considered a newer version of the same object.

### Sources array
A node object can contain some information on where new versions of the same node object can be found.

If the array _sources_ is present, it contains information on where to find new versions of the same node object.

Any user or system receiving a new node object, or periodically re-checking some node objects already in memory, is supposed to check all the sources, to verify if there is a newer version of the same object. If a new object is found with the same _circle_ and a _serial_ greater than the _serial_ of the object in memory, the legacy object should be discarded and substituted with the new one. This behavior can be changed by implementations. For example, an implementation can state that a node object expires after a certain period of time.

The array _sources_ can contain the following type of elements:

ipns://ipns-address: contains a pointer to the IPFS hash of the node object in the form of an IPNS address or a domain name. For example: `luca.yepa.com` or `k51qzi5uqu5dlvj2baxnqndepeb86cbk3ng7n3i46uzyxzyqj2xjonzllnv0v8`.

URL (http://..., https://..., ...): contains a URL of the node object (for example, `http://luca.yepa.com/friends.toml`).

nostr://nostr-address: contains the nostr id where new versions of the node object can be found. The IPFS hash will be in the Nostr field _name_, in the kind 0 event. For example, `npub1gl23nnfmlewvvuz7xgrrauuexx2xj70whdf5yhd47tj0r8p68t6sww70gt`.

In order to publish a node object, first the hash should be calculated as specified in [hash.md](hash.md), then the hash should be signed as specified in implementation -> sign. Then _hash_ and _sig_ should be added to the object in the _signature_ table.

Example of publication to some sources:
* The node object can be published on IPFS by pinning it. For example: `ipfs add node.toml`.
* Then, depending on the source:
   * URL: change your web server and set the content of the URL to the new object
   * IPNS: publish it to your ipns: `ipfs name publish /ipfs/<ipfs key returned from the pinning command>
   * Nostr: change _sources.nostr_ event 0 _name_ and set it to a web URL, or to an IPFS hash

### Profile hash table
The _profile_ hash table, if present, contains information on how the user wants to be contacted. This information can be used by communities to define protocols of communication between users.

_profile.aka_ contains an internet identifier, similar to an email address. If present, whoever receives a node object, is suggested to do an HTTP query to check if the key of the user is still the same contained in the node object. The protocol to send the query is defined in another document and is meant to translate an address like `satoshin@gmx.com` to an identity key. If the key is different, the community can define a process that maps the key used in the node object to the new key found. If the two keys are different, the community can decide that the object is not valid.

The email-like address is not used in the rest of the web of trust. This information is meant to be used only to simplify the way of communicating identity keys between the users.

Beware of _profile_: fake accounts can use the fields of the profile part to impersonate another user. Don't trust this information to find who the user is. The only valid information to know who is the person that signed an object is the identity key used to sign it.

### Trust array
The most important part of the node object is the set of links to other users. Every _link_ contains the identity key of the user trusted by the author of the node object, along with a _value_ of the link (weight) and the _unit_. Since any community interprets differently the meaning of a link between two users, every link contains also a reference to the rules that the link adheres to. Every link contains also a timestamp: _since_.

_trust_ is a key of the node object. The value is an array having _link_ elements. These are the links of the graph, containing the weight ("value") expressed in "unit"s. As an example, the _trust_ array contains the sponsorships given by the owner of this node to other users.

### Link
Every element of the _trust_ array has the following properties: _on_, _since_, _to_, _unit_, _value_.

_on_ is an array of links to the rules that the user accepts to adhere to, with this link. It can be a URL, an IPFS address, an IPNS address, or a reference to a key of the helper table _ref_, that contains a URL, IPFS or IPNS string.

_since_ (integer) is a timestamp of the creation of the link. For example, depending on implementations, it can be set to the block number height of the Bitcoin time chain at the time this link has been created. On the community layer, it can be used to avoid circular sponsorships, self sponsorships, and some Sybil attacks. Also, by using this number, a community can slow the pace of new links on a single path to a value of 10 minutes for each new node added to the path (on average).

Other implementations can use the unix epoch timestamp, or other formats, like yyyymmdd.

A good rule is not to change this number when a user simply increases the "value" field of the link. In fact, some communities, can mandate that a user gives a link to another user, only after having received a certain link. Changing this number to something greater will invalidate the downstream links.

_to_ is the identity key of the person that we are linking to.

A single user can link to the same _to_ user multiple times in the same object.

For example, the same object can be used by multiple communities, or a community can allow different kinds of sponsorships, denominated in different currencies.

_unit_ is the measurement unit of the field _value_. The community can mandate the use of a certain unit. For example, the rules of Bitcoin Italia Market mandate the use of "EUR".

_value_ is the weight of the link. It is a signed integer. Usually, it describes how much trust there is between the users. Depending on the rules of the community, _value_ can be the amount of _unit_s that a user agrees to pay on behalf of another user.

Two or more links that are identical by any property but the _value_ field will be summed to define the unique value of the link.
