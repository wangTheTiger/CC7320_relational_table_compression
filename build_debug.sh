g++  -std=c++17 -g -I ~/include -L ~/lib main.cpp -o main -lsdsl -ldivsufsort -ldivsufsort64
g++  -std=c++17 -g -I ~/include -L ~/lib process_csv.cpp -o process_csv -lsdsl -ldivsufsort -ldivsufsort64