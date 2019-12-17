#!/bin/bash
rm -rf TestFiles
mkdir TestFiles

for i in {0..100} ;
do
val=`expr $RANDOM % 100`
echo $val
dd if=/dev/zero of=TestFiles/dummy$i.txt bs=1M count=$val 2>/dev/null 1>/dev/null
done
cd TestFiles
for fil in `ls `
do
md5sum $fil >> md5sum.txt
done
cd -
