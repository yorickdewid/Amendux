# Amendux

Copyright © 2016 The Amendux Project.

[TOC]

## Changelog

*   Version 1.3.8.0
    *   Encrypted traffic 
    *   FTP Client
    *   Internal ScreenMod
    *   Configuration builder
    *   Backend compatible with PHP 5.2+
*   Version 1.3.7.4
    *   WebClient
    *   Client side auto update
    *   Remove CryptoMod
    *   ROT strings

Overview
------------

The **Amendux** project ... 

*Definition classification*: Win32/Adux.VX

*Please, read the disclaimer below before proceeding*.

## Requirements

In order to build, test and run the entire project, at least the following requirements must be met:

- Microsoft Visual C++ 10.0+
- At least PHP 5.2 with SQLite 2+
- i386 or amd64 processor
- Microsoft Windows:
  - Windows 7
  - Windows 8
  - Windows 8.1
  - Windows 10
  - Windows Server 2012,
  - Windows Server 2012 RC,
  - Windows Server 2016

Optional but **recommended** for *testing* and *development*:

- [SysInteral's Process Explorer](https://technet.microsoft.com/en-us/sysinternals/processexplorer.aspx)
- [Dependency walker](http://www.dependencywalker.com/)
- A resource editor
- Amendux ROTR (included)
- Amendux YTEA (included)

### Environment setup

It is recommended to use virtual hardware in combination with a clean Microsoft Windows install. Setup two (virtual) machines, one for building and another for testing. Make sure they are connected to an internal network and are isolated from any other machines, networks or data sources. 

Client
------------

Note again how the actual text starts at 4 columns in (4 characters
from the left side). Here's a code sample:

    # Let me re-iterate ...
    for i in 1 .. 10 { do-something(i) }

As you probably guessed, indented 4 spaces. By the way, instead of
indenting the block, you can use delimited blocks, if you like:

~~~
define foobar() {
    print "Welcome to flavor country!";
}
~~~

(which makes copying & pasting easier). You can optionally mark the
delimited block for Pandoc to syntax highlight it:

~~~python
import time
# Quick, count to ten!
for i in range(10):
    # (but not *too* quick)
    time.sleep(0.5)
    print i
~~~

### Infect

### WebClient

### FTPClient

### Candcel ###

The **Candcel** module providers all communication to and from the *Command & Control* server.

1.  First, get these ingredients:

    * carrots
    * celery
        * lentils

2.  Boil some water.

3.  Dump everything in the pot and follow
      this algorithm:

          find wooden spoon
          uncover pot
          stir
          cover pot
          balance wooden spoon precariously on pot handle
          wait 10 minutes
          goto first step (or shut off burner when done)

      Do not bump wooden spoon or it will fall.

Notice again how text always lines up on 4-space indents (including
that last line which continues item 3 above).

Here's a link to [a website](http://foo.bar), to a [local
doc](local-doc.html), and to a [section heading in the current
doc](#an-h2-header). Here's a footnote [^1].

[^1]: Footnote text goes here.

Tables can look like this:

size  material      color
----  ------------  ------------
9     leather       brown
10    hemp canvas   natural
11    glass         transparent

Table: Shoes, their sizes, and what they're made of

(The above is the caption for the table.) Pandoc also supports
multi-line tables:

--------  -----------------------
keyword   text
--------  -----------------------
red       Sunsets, apples, and
          other red or reddish
          things.

green     Leaves, grass, frogs
          and other things it's
          not easy being.
--------  -----------------------

A horizontal rule follows.

***

Here's a definition list:

apples
  : Good for making applesauce.
oranges
  : Citrus!
tomatoes
  : There's no "e" in tomatoe.

### Module loader

### Internal modules

#### Shell

#### Screen

## External modules

## Backend

(rely service, AVC, XTEA)

## Operation

### Building client

If the minimal requirements are met and the environment is setup, proceed the following. Please read the steps **carefully**. Save passwords and tokens to a secure place.

**Building:**

1. Use the Builder to generate a `vaconfig.h` file, provide settings where required. For further explanation about the Builder see the *Appendix I*.
2. Move the `vaconfig.h` into the client source directory.
3. Select the build (Release), target and architecture in the compiler. If you target Windows 8 or above, ignore x86 builds.
4. Compile the client and check that there are no errors shown during the compilation and linkage process. If all good, it will output a `.pex` file.
5. Feed the resulting `.pex` file to Pexor. (*Beware*: Do **NOT** try to execute the `.pex` file in any way!)
6. Change settings if required, but default settings should suffice. Generate the executable. If no errors prompt, the client is build successfully and is ready for deployment.

**Testing:**



(Builder, deploy, PEX, admin console)

## Development

(AVC extending, client mods internal, external)

# Appendix I: Tools

(How to use Builder)

# Appendix II: Internals

(modified XTEA, PE segment encryption, WebAPI)

# Appendix III: Security

# Definitions

`vaconfig.h` 					Variant configuration file used to build the client

`AVC`						Adaptive Vector Controller, the endpoint for both client and admin console

`Checkin Pace`				The rate at which clients checkin measured in minutes

`Scheme`						Protocol used for network connections, for example *HTTP* or *FTP*

`PEX`						Portable Executable Extension, the intermediate executable

# Disclaimer

This source is offered to you, the user ("User"), conditioned on acceptance of the terms, conditions, and notices contained herein, without modification. User access and use of this source constitutes acceptance of these terms and conditions. 

...

