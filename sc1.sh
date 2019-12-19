rm -rf Dst/
sleep 5
echo "************ CURRENT_CODE *********************"
time ./fileCopy_CURRENT_CODE

rm -rf Dst/
sleep 5
echo "************ PARALLEL_CODE *********************"
time ./fileCopy_PARALLEL_CHECKSUM

