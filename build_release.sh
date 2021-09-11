g++  -std=c++17 -I ~/include -L ~/lib compress_table.cpp -o compress_table -lsdsl -ldivsufsort -ldivsufsort64 -lboost_serialization
g++  -std=c++17 -I ~/include -L ~/lib retrieve_table.cpp -o retrieve_table -lsdsl -ldivsufsort -ldivsufsort64 -lboost_serialization
g++  -std=c++17 -I ~/include -L ~/lib process_csv.cpp -o process_csv -lsdsl -ldivsufsort -ldivsufsort64