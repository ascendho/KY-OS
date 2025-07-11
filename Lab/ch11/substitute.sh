#!/bin/bash
SCRIPT=substitute.sh
for f in * 
do 
if [ $f = $SCRIPT ]
then
        echo "skip $f"
        continue
    fi
    echo -n "substitute ext2 to myext2 in $f..."
    cat $f | sed 's/ext2/myext2/g' > ${f}_tmp
    mv ${f}_tmp $f
    echo "done"
    echo -n "substitute EXT2 to MYEXT2 in $f..."
    cat $f | sed 's/EXT2/MYEXT2/g' > ${f}_tmp
    mv ${f}_tmp $f
    echo "done"
done