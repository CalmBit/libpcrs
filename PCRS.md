# .PCRS file - a mini-specification

PCRS files are pretty simple. As we're tracking picross (and not something
more complex), we can store a bunch of information in discrete chunks, and
not worry too terribly much about the footprint of our file.

## 1. Language in this specification

The use of language in this specification is deliberate: we adhere to the
RFC2119 provisions in regards to language. To sum up:

`MUST/-NOT | SHALL/-NOT` indicate binding requirements in order to be
considered a "standards compliant" implementation.

`SHOULD/-NOT | RECOMMENDED` indictate requirements that are expected to
be present, but are allowed to be ignored if the cause is great enough.
Beware - many of these features may be tacitly expected.

`MAY | OPTIONAL` indicate suggestions for behaviour that may be
benefical to the task at hand, but are not imperative. Note that
these are not necessarily expected, but can be thought of as a
"wishlist" of possible additions to a more-fully featured implementation.

## 2. Technical Information

### 2.1 Basic PCRS file

The maximum size of a single default PCRS "board" is 255x255 - 
one byte per axis. This is a hard limit, and a compiler/parser
**MUST NOT** implement bigger blocks within the default framework - use
an extension.

The first four bytes of a PCRS file are the magic number - appropriately
"PCRS". This is followed by the block index, which is layed out like so:

2 bytes - number of blocks in file
-(repeat)-
1 byte - block type
4 bytes - block offset
-(repeat)-

This index is then used to parse out the seperate sections of the file,
listed below.

### 2.2 Data Blocks

#### NULL block - 0x00

**Header Size:** N/A

**Body Size:** N/A 

##### Parser Rules

The NULL block isn't rigidly defined, but it's assumed that if an offset lands
on 0x00, the following data does not exist. In this case, the parser **MUST**
bypass this block, and pick the next block in the list. It **MUST** not add the
block to any block list or count, and **MAY** report a warning stating that one
of the requested blocks was found to be empty. The parser **MUST NOT** throw
an error or fail to parse the file upon encountering a NULL block.

##### Compiler Rules

There's no reason for a NULL block to be in any file - any PCRS compiler
**MUST NOT** compile a .PCRS file with a NULL block, under any circumstances.


#### DATA block - 0x01

**Header Size:** 2 bytes

**Body Size:** variable, dependent on header

The DATA block is the bread and butter of the PCRS format - it defines
a picross "board", and the data that entails.

A PCRS board is simply an array of bitfields that define the state of
each square on a typical picross puzzle. This means that the body size
**MUST** be `ceil((xy)/8)`.

##### Parser Rules

All a PCRS parser needs to assume in interpreting DATA blocks is that
all of the information provided is intended: DATA blocks do not
implement checksumming (unlike [CDAT](#CDAT) blocks). Therefore, all
information **MUST** be parsed literally. A malformed file will
inevitably produce malformed output, but the parser **MUST NOT**
try to correct any percieved errors.

##### Compiler Rules

A PCRS compiler is expected to assemble a DAT block according to the above
specification. A compiler **MUST NOT** try to assemble coordinates over
255, as these will be parsed incorrectly and blindly. A compiler **MUST**
literally interpret whatever format the suggested board is recieved in,
and convert it to a set of bitflags.

As an example, a board that would appear as the following to the end-user,

□□□■■□□□

■□□■■□□■

□□□■■□□□

, would be encoded

`0x08 0x03 0x18 0x99 0x18`

for all intents and purposes.


## 3. Extensions

An attempt has been made to slowly but surely cover most traditional use-cases
for a format like this, including realsitic, non-obtrusive constraints that
allow for flexibility while also giving reasonable performance. However, it
will inevitably come about that the framework set up here doesn't cover
an intended use-case.

Therefore, a framework by which compiler/parser specific extensions can be
defined and articulated seems at least partially necessary.

_**WIP** - This will be fairly barebones, as this is simply just advice and
guidelines for how to write "extensions" into this specification._

Notes:

* Reserve block IDs 00-1F as libpcrss-only
* Mark 20-7F as unallocated space.
* Reserve 80-FF as "extension space."
