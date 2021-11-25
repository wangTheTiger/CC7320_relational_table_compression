g++  -std=c++17 -I ~/include -L ~/lib compress_table.cpp -o compress_table -lsdsl -ldivsufsort -ldivsufsort64 -lboost_serialization
g++  -std=c++17 -I ~/include -L ~/lib retrieve_table.cpp -o retrieve_table -lsdsl -ldivsufsort -ldivsufsort64 -lboost_serialization
g++  -std=c++17 -I ~/include -L ~/lib process_csv_text.cpp -o process_csv_text -lsdsl -ldivsufsort -ldivsufsort64
g++  -std=c++17 -I ~/include -L ~/lib process_csv_binary.cpp -o process_csv_binary -lsdsl -ldivsufsort -ldivsufsort64 -lboost_serialization
g++  -std=c++17 -I ~/include -L ~/lib process_csv_int_vector_sdsl.cpp -o process_csv_int_vector_sdsl -lsdsl -ldivsufsort -ldivsufsort64
g++  -std=c++17 -I ~/include -L ~/lib retrieve_columns_bwt.cpp -o retrieve_columns_bwt -lsdsl -ldivsufsort -ldivsufsort64
g++  -std=c++17 -I ~/include -L ~/lib retrieve_columns_plain.cpp -o retrieve_columns_plain -lsdsl -ldivsufsort -ldivsufsort64