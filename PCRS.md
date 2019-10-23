# ARCHIVED
The below is archived. Please do not use this or expect changes, as it will not be edited going forward.
------
# <a name="top"></a>PCRS - a mini-specification

PCRS files are pretty simple. As we're tracking picross (and not something
more complex), we can store a bunch of information in discrete chunks, and
not worry too terribly much about the footprint of our file.

## <a name="sec1"></a>1. Language in this specification

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

## <a name="sec2"></a>2. Technical Information

### <a name="sec2.1"></a>2.1 Basic PCRS file

The maximum size of a single default PCRS "puzzle" is 255x255 - 
one byte per axis. This is a hard limit, and a compiler/parser
**MUST NOT** implement bigger blocks within the default framework - use
an extension.

The first four bytes of a PCRS file are the magic number - appropriately
"PCRS". This is followed by the block index, which is layed out like so:

- 2 bytes - number of blocks (n) in file
- 4 bytes repeated n times - block offsets

This index is then used to parse out the seperate sections of the file,
listed below.

### <a name="sec2.2"></a>2.2 Data Blocks

#### <a name="bNULL"></a>NULL block - 0x00

**Header Size:** N/A

**Body Size:** N/A 

The NULL block isn't rigidly defined, but it's assumed that if an offset lands
on 0x00, no data will be following this byte. 

##### Parser Rules

The parser **MUST** bypass this block, and pick the next block in the list. 
It **MUST** not add the block to any block list or count, and **MAY** report 
a warning stating that one of the requested blocks was found to be empty.
The parser **MUST NOT** throw an error or fail to parse the file upon 
encountering a NULL block.

##### Compiler Rules

There's no reason for a NULL block to be in any file - a PCRS compiler
**MUST NOT** compile a .PCRS file with a NULL block, under any circumstances.


#### <a name="bDATA"></a>DATA block - 0x01

**Header Size:** 2 bytes

**Body Size:** variable, dependent on header

The DATA block is the primary component of the PCRS format - it defines
a picross puzzle, and the data that entails.

It's compromised of an array of bitfields that define the state of
each square of a typical picross puzzle. This means that the body size
**MUST** be `ceil((xy)/8)`, in addition to a CRC-32 checksum appended to
the main body.

##### Parser Rules

A parser will first verify the information according to the attached checksum, 
a CRC-32 sum appended to the body. If an error is detected, the parser **MUST**
throw an error, and [disregard](#g_disregard) the block. If no errors are 
detected, **MUST** be assumed that the block's data is indeed intact, and 
therefore be parsed literally. The parser **MUST NOT** perform any error 
correction at this stage.

##### Compiler Rules

A PCRS compiler is expected to assemble a DAT block according to the above
specification. A compiler **MUST NOT** try to assemble coordinates over
255, as these will be parsed incorrectly and blindly. A compiler **MUST**
literally interpret whatever format the suggested puzzle is recieved in,
and convert it to a set of bitflags.

##### Other Notes

As a quick example of the requeste encoding sceheme, a puzzle that would appear
as the following to the end-user,

□□□■■□□□

■□□■■□□■

□□□■■□□□

, would be encoded

`0x08 0x03 0x18 0x99 0x18`

for all intents and purposes.

The CRC-32 algorithm used for the example library (libpcrs) is the zlib CRC-32 implementation.

#### <a name="bMETA"></a>META Block - 0x02

**Header Size:** 3 Bytes
**Body Size:** variable

A META block is intended for storing arbitrary metadata about DATA blocks.
The first two bytes are the internal index of the block being referenced (i.e.
the block's position in the file-level block index. The next byte is the
identifier for the type of metadata being encoded. Different metadata types
and their identifiers are discussed further in the section on [metadata](#sec4)
later in this document.

##### Parser Rules

A parser **SHOULD** keep a rigidly defined table of possible metadata types, and
work upon the assumption that any valid metadata will be using an approved type
identifier. If a metadata type is unrecognized by the parser at hand, the parser
**MUST** throw a warning, and [disregard](#g_disregard) the block. If the 
metadata type is recognized, the parser **MAY** perform parser-side validation,
and **MAY** [disregard](#g_disregard) the block with a warning if it fails this
validation.

##### Compiler Rules

A compiler **MUST** keep a rigidly defined table metadata types, and implement
distinct validation procedures for the data it's interpreting. If a piece of
metadata fails a validation step, the compiler **MUST** throw a fatal error,
and refuse to continue compilation.

##### Other Notes

If a particular type of metadata is unrepresented by this standard,
but a parser or compiler would like to implement support for this metadata, 
it **MAY** use unreserved metadata identifiers for this purpose, through an 
[extension](#sec3).

## <a name="sec3"></a>3. Extensions

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

## <a name="sec4"></a>4. Metadata

**TBW**

Notes:

* Name
* Suggested Difficulty
* Description
* Author
* Date Published
* Notes?

## <a name="sec5"></a>5. Glossary

<a name="g_disregard"></a>**disregard** - A parser, when asked to **disregard**
a block, will immediately stop parsing any further information related with
the block. It will proceed to move onto parsing the next block in the index,
without either counting the disregarded block, or adding any information about
its existance into the set of blocks maintained by the parser. A parser **MAY**
keep an arbitrarily defined count of disregarded blocks, and **MUST** follow any
other requirements on warning/error dispatching wherever a disregard is
requested.
