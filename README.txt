# Lab 07

Matthew Ingram (811-887-516)
Patrick Macedo (811-813-842)

## Instructions

To clean our code (i.e., remove any compiled files), type:

```
$ make clean
```

To compile my code, type:

```
$ make
```

To run my code (after compiling it), type:

```
$ ./1730ed somefile
```
## Notes
Despite streneous troubleshooting, our text editor still has the following bugs\
:

1)      The way the text editor is set up, you cannot TAB an unaltered full lin\
e of text from a file; If you wish to TAB, you must delete or insert a characte\
r onto the full line first, then TAB.

2)    The very first edit onto a unaltered full line of text from a file will o\
verlay the character under the cursor, not insert before.

Non-bug notes:

1) The extra files in our project folder are admiral.txt, the test file we used to run and testy.txt, a test file we chmod'd to 0000 to get a permissions error, 
