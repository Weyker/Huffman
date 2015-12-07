#!/bin/bash

FILENAME="bible.txt"
FILESIZE= "$(ls -lah "$FILENAME" | awk '{ print $5}')"

FILENAMECOMPRESS="bible.char"
FILESIZETEST= "(ls -lah "$FILENAMECOMPRESS" | awk '{ print $5}')"
TYPE="-c"

clear

./../build/compressor $TYPE $FILENAME $FILENAMECOMPRESS
echo "$FILESIZETEST vs $FILESIZE"
./../build/decompressor $FILENAMECOMPRESS bible.orig
echo "diff: "
diff bible.orig $FILENAME
echo "\n"

FILENAMECOMPRESS="bible.short"
FILESIZETEST= $(ls -lah "$FILENAMECOMPRESS" | awk '{ echo $5}')
TYPE="-s"

./../build/compressor $TYPE $FILENAME $FILENAMECOMPRESS
./../build/decompressor $FILENAMECOMPRESS bible.orig
echo "diff: "
diff bible.orig $FILENAME
echo "\n"

FILENAMECOMPRESS="bible.int"
FILESIZETEST= $(ls -lah "$FILENAMECOMPRESS" | awk '{ echo $5}')
TYPE="-i"

./../build/compressor $TYPE $FILENAME $FILENAMECOMPRESS
./../build/decompressor $FILENAMECOMPRESS bible.orig
echo "diff: "
diff bible.orig $FILENAME
echo "\n"

FILENAMECOMPRESS="bible.long"
FILESIZETEST= $(ls -lah "$FILENAMECOMPRESS" | awk '{ echo $5}')
TYPE="-l"

./../build/compressor $TYPE $FILENAME $FILENAMECOMPRESS
./../build/decompressor $FILENAMECOMPRESS bible.orig
echo "diff: "
diff bible.orig $FILENAME
echo "\n"