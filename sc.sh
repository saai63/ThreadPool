for i in 1 2 3 4 5;
do
rm -rf Dst
sleep 5
time ./fileCopy $i
done
