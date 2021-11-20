#!/usr/bin/env sh
#example:
#./run.sh ../../../Ring/dat/wikidata-filtered-enumerated.dat
#or alternatively:
#time ./run.sh ../../../Ring/dat/wikidata-filtered-enumerated.dat
./build_release.sh
file="test.dat"
if [ -n "$1" ];
then
    file=$1
fi
echo "working file : $file"
echo "Processing..."
./compress_table $file

#ls -lh $file
rm -f "$file.elias_delta"
#ls -lh "$file.elias_gamma"
rm -f "$file.elias_gamma"
#ls -lh "$file.fibonacci"
rm -f "$file.fibonacci"