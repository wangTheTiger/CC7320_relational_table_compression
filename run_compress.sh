#!/bin/bash

die () {
    echo >&2 "$@"
    exit 1
}
[ "$#" -eq 5 ] || die "5 arguments required, $# provided. example: ./compress_table input_file.dat column_1 column_2 column_3 column_4"


calc_compress() {
    ./compress_table $1 $2 $3 $4 $5

    echo "column order: $2 $3 $4 $5"
    du -c ${1}_* | grep "total" # -h 
}
echo "Building..."
./build_release.sh
file="test.dat"
if [ -n "$1" ];
then
    file=$1
fi

column_1=$2
column_2=$3
column_3=$4
column_4=$5
echo "working file : $file"
echo "Processing..."
#i=1
#while [ $i -le $permutations ]
#do

#4! = 24 permutations
calc_compress $file $column_1 $column_2 $column_3 $column_4
calc_compress $file $column_1 $column_3 $column_2 $column_4
calc_compress $file $column_1 $column_3 $column_4 $column_2
calc_compress $file $column_1 $column_4 $column_3 $column_2
calc_compress $file $column_1 $column_4 $column_2 $column_3
calc_compress $file $column_1 $column_2 $column_4 $column_3

calc_compress $file $column_2 $column_1 $column_3 $column_4
calc_compress $file $column_2 $column_3 $column_1 $column_4
#calc_compress $file $column_2 $column_3 $column_4 $column_1
#calc_compress $file $column_2 $column_4 $column_3 $column_1
#calc_compress $file $column_2 $column_4 $column_1 $column_3
#calc_compress $file $column_2 $column_1 $column_4 $column_3

#calc_compress $file $column_3 $column_1 $column_2 $column_4
#calc_compress $file $column_3 $column_2 $column_1 $column_4
#calc_compress $file $column_3 $column_2 $column_4 $column_1
#calc_compress $file $column_3 $column_4 $column_2 $column_1
#calc_compress $file $column_3 $column_4 $column_1 $column_2
#calc_compress $file $column_3 $column_1 $column_4 $column_2

#calc_compress $file $column_4 $column_1 $column_2 $column_3
#calc_compress $file $column_4 $column_2 $column_1 $column_3
#calc_compress $file $column_4 $column_2 $column_3 $column_1
#calc_compress $file $column_4 $column_3 $column_2 $column_1
#calc_compress $file $column_4 $column_3 $column_1 $column_2
#calc_compress $file $column_4 $column_1 $column_3 $column_2
#    i=$(($i + 1 ))
#done