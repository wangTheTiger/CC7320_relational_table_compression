#include <iostream>
#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#include <fstream>

typedef std::tuple<uint64_t, uint64_t, uint64_t> spo_triple;

using timer = std::chrono::high_resolution_clock;
//stable_sort with dinamic content is slow for the test data set.
using row = std::deque<uint64_t>;
using matrix = std::vector<row>;
using wm = sdsl::wm_int<sdsl::rrr_vector<15>>;
/*
void apply_front_coding_and_vlc(std::vector<spo_triple> &D, std::string file){
    uint64_t i;
    std::vector<spo_triple>::iterator it, triple_begin = D.begin(), triple_second = std::next(D.begin()), triple_end = D.end();
    //Optional step to calculate the input size with two flavors, first as a int_vector and then as a compressed int_vector.
    { 
        sdsl::int_vector<> v_aux(D.size() * 3);
        std::vector<spo_triple>::iterator n_it, n_end = D.end();

        for (i = 0, n_it = D.begin(); n_it != n_end; n_it++, i+=3)
        {
            v_aux[i] = std::get<0>(*n_it);
            v_aux[i+1] = std::get<1>(*n_it);
            v_aux[i+2] = std::get<2>(*n_it);
        }
        //std::cout << "input size in megabytes                           : "<< sdsl::size_in_mega_bytes(v_aux) << std::endl;
        sdsl::util::bit_compress(v_aux);
        std::cout << "compressed input size in megabytes                : "<< sdsl::size_in_mega_bytes(v_aux) << std::endl;
    }
    //<< optional code ends here
    spo_triple t_dash = spo_triple(std::get<0>(*triple_begin), std::get<1>(*triple_begin), std::get<2>(*triple_begin));
    std::vector<uint64_t> numbers_to_encode;
    //initial triple
    spo_triple t = spo_triple(std::get<0>(*triple_begin), std::get<1>(*triple_begin), std::get<2>(*triple_begin));
    //std::cout << std::get<0>(t_dash) << " " << std::get<1>(t_dash) << " " << std::get<2>(t_dash) << std::endl;
    numbers_to_encode.push_back(std::get<0>(t_dash) + 1);
    numbers_to_encode.push_back(std::get<1>(t_dash) + 1);
    numbers_to_encode.push_back(std::get<2>(t_dash) + 1);
    for (i = 0, it = triple_second; it != triple_end; it++, i++)
    {
        spo_triple t_aux = spo_triple(std::get<0>(*it) - std::get<0>(t), std::get<1>(*it) - std::get<1>(t), std::get<2>(*it) - std::get<2>(t));
        t_dash = spo_triple(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it));
        if(std::get<0>(t_aux) == 0){

            if(std::get<1>(t_aux) == 0){
                //std::cout << std::get<2>(t_aux) << std::endl;
                numbers_to_encode.push_back(std::get<2>(t_aux) + 1);
            }else{
                //std::cout<< "(ESC)" << std::get<1>(t_aux) << " " << std::get<2>(t_dash) << std::endl;
                numbers_to_encode.push_back(0);
                numbers_to_encode.push_back(std::get<1>(t_aux) + 1);
                numbers_to_encode.push_back(std::get<2>(t_dash) + 1);
            }
        }else{
            //std::cout<< "(ESC)(ESC)" << std::get<0>(t_aux) << " " << std::get<1>(t_dash) << " " << std::get<2>(t_dash) << std::endl;
            numbers_to_encode.push_back(0);
            numbers_to_encode.push_back(0);
            numbers_to_encode.push_back(std::get<0>(t_aux) + 1);
            numbers_to_encode.push_back(std::get<1>(t_dash) + 1);
            numbers_to_encode.push_back(std::get<2>(t_dash) + 1);
        }
        t = spo_triple(std::get<0>(t_dash), std::get<1>(t_dash), std::get<2>(t_dash));
    }
    //2. copy vector to an int_vector
    sdsl::int_vector<> v(numbers_to_encode.size());
    std::vector<uint64_t>::iterator n_it, n_end = numbers_to_encode.end();
    for (i = 0, n_it = numbers_to_encode.begin(); n_it != n_end; n_it++, i++)
    {
        v[i] = *n_it;
    }
    //std::cout << "numbers_to_encode: " << numbers_to_encode << std::endl;
    //3. encode it
    //std::cout << "vector<spo_triple> # of elements                  : " << D.size() * 3 << std::endl;
    //std::cout << "front coded int vector # of elements              : " << v.size() << std::endl;
    //std::cout << "vector<spo_triple> size in megabytes              : "<< D.size() * 3 * 8 / pow(10,6) << std::endl;
    //std::cout << "sdsl int_vector size in megabytes                 : "<< sdsl::size_in_mega_bytes(v) << std::endl;
    sdsl::util::bit_compress(v);
    //std::cout << "sdsl compressed int_vector size in megabytes      : "<<  sdsl::size_in_mega_bytes(v) << std::endl;
    {
        sdsl::vlc_vector<sdsl::coder::elias_delta> ev_d(v);
        //std::cout << "ev_d: " << ev_d << std::endl;
        sdsl::store_to_file(ev_d, file + ".elias_delta");
        //std::cout << "encoded vector size : " << ev_d.size() << std::endl;
        std::cout << "Elias delta encoded vector size in megabytes      : "<< sdsl::size_in_mega_bytes(ev_d) << std::endl;
    }
    {
        sdsl::vlc_vector<sdsl::coder::elias_gamma> ev_g(v);
        //std::cout << "ev_g: " << ev_g << std::endl;
        sdsl::store_to_file(ev_g, file + ".elias_gamma");
        //std::cout << "encoded vector size : " << ev_g.size() << std::endl;
        std::cout << "Elias gamma encoded vector size in megabytes      : "<< sdsl::size_in_mega_bytes(ev_g) << std::endl;
    }
    {
        sdsl::vlc_vector<sdsl::coder::fibonacci> ev_f(v);
        //std::cout << "ev_f: " << ev_f << std::endl;
        sdsl::store_to_file(ev_f, file + ".fibonacci");
        //std::cout << "encoded vector size : " << ev_f.size() << std::endl;
        std::cout << "Fibonacci encoded vector size in megabytes        : "<< sdsl::size_in_mega_bytes(ev_f) << std::endl;
    }
}*/
/**
 * get_last_column_as_vector retrieves a std::vector<uint64_t> corresponding to the last column of table.
 * It also calculates C vector used for LP mapping.
 * */
std::vector<uint64_t> get_last_column_as_vector(matrix &table, std::map<uint64_t, uint64_t> &C){//TODO: future work, do C as bit_vector (larger alphabets)
    std::vector<uint64_t> l;//TODO: how to set the size here. should I use an std::array instead?
    int i;
    //Map used to count instances of an element
    for (i = 0; i < table.size(); i++){
        uint64_t number = table[i][table[i].size() - 1];
        C[number] = C[number] + 1;
        l.push_back(number);
    }

    uint64_t count = 0, key = 0, aux = 0;
    std::map<uint64_t, uint64_t>::iterator it, end = C.end();
    std::cout << " C: ";
    for (i = 0, it = C.begin(); it != end; it++, i++){
        //gets the value of the map.
        key = it->first;
        aux = it->second;
        C[key] = count;
        std::cout << count << ' ';
        count += aux;
    }
    std::cout << std::endl;
    l.shrink_to_fit();
    return l;
}

void move_last_column_to_front(matrix &table){
    matrix::iterator it, end = table.end();
    int i;
    for (i = 0, it = table.begin(); it != end; it++, i++){
        row &r = table[i];
        r.push_front(r[r.size() - 1]);
        r.pop_back();
    }
}
void pop_first_column(matrix &table){
    matrix::iterator it, end = table.end();
    int i;
    for (i = 0, it = table.begin(); it != end; it++, i++){
        table[i].pop_front();
    }
}
void print_L(std::vector<uint64_t> &L){
    std::vector<uint64_t>::iterator it, end = L.end();
    int i;
    for (i = 0, it = L.begin(); it != end; it++, i++){
        std::cout << L[i] << ' ';
    }
    std::cout << std::endl;
}
uint64_t LF(wm &wm, std::map<uint64_t, uint64_t> &C, int search_span, uint64_t symbol){
    //std::cout << " rank : " << wm.rank(search_span + 1,symbol) << " C: " << C[symbol] << " Symbol : " << symbol << " search_span : " << search_span + 1 << std::endl;
    return C[symbol] + wm.rank(search_span + 1,symbol);
}
int main(int argc, char **argv){
    uint64_t i;
    matrix table;
    std::string file= argv[1];
    char delim = ' ';

    sdsl::memory_monitor::start();
    auto start = timer::now();
    /*********************** PART 1 : Read input file ***********************/
    std::ifstream ifs(file);
    std::string str_line;
    while (std::getline(ifs, str_line)){
        //TODO: only ints for initial version, to be improved.
        std::deque<uint64_t> aux;
        // construct a stream from the string
        std::stringstream ss(str_line);
        std::string s;
        while (std::getline(ss, s, delim)) {
            int i = std::stoi(s);
            aux.push_back((uint64_t) i);
        }
        table.push_back(aux);
    }
    ifs.close();
    table.shrink_to_fit();
    std::cout << "--Loaded " << table.size() << " rows" << " with " << table[0].size() << " columns each. " << std::endl;
    /*********************** PART 2 : Process last column (k) ***********************/
    //lexicographic table sorting.
    matrix::iterator it, table_begin = table.begin(), triple_end = table.end();
    std::sort(table_begin, triple_end);//TODO: try with stable_sort. Is it necessary? How slow is it in comparison with the former?

    std::vector<std::vector<uint64_t>> L;
    int L_last_pos = 0, num_of_rows = table.size(), num_of_columns = table[0].size();
    std::vector<std::map<uint64_t, uint64_t>> C;  // TODO: C is part of a class Table I have to use which contains C, WT and also LF Mapping.
    //adding the last column of the table as L_j, with j in {1,..,k} backwardly.
    std::map<uint64_t, uint64_t> c_aux;
    L.push_back(get_last_column_as_vector(table, c_aux));
    L[L_last_pos].shrink_to_fit();
    C.push_back(c_aux);
    print_L(L[L_last_pos]);

    //building Wavelet tree of L_i.
    std::vector<wm> wavelet_matrices;//WE actually dont need to store the L_i! TODO: define rrr_vector block param
    sdsl::int_vector<> v(num_of_rows);

    wm wm_aux;
    for(int j = 0; j < num_of_rows; j++){
        v[j] = L[L_last_pos][j];
    }
    construct_im(wm_aux, v);
    wavelet_matrices.push_back(wm_aux);
    /*std::cout << "number of lines  : " << wm_test.rank(wm_test.size(), 2) << std::endl;
    std::cout << "first '2' in line: " << wm_test.rank(wm_test.select(1, '2'),'\n')+1 << std::endl;
    std::cout << "wt.sigma : " << wm_test.sigma << std::endl;
    std::cout << wm_test << std::endl;
    std::cout << "WM size in bytes : " << sdsl::size_in_bytes(wm_test) << std::endl;    */

    /*********************** PART 2 : Process k-1 ..+ 1 column ***********************/
    for(i = 1; i < num_of_columns; i++){
        std::map<uint64_t, uint64_t> c_aux;
        move_last_column_to_front(table);
        std::sort(table_begin, triple_end);
        pop_first_column(table);
        L.push_back(get_last_column_as_vector(table, c_aux));
        L_last_pos = L.size() - 1;
        L[L_last_pos].shrink_to_fit();
        C.push_back(c_aux);
        print_L(L[L_last_pos]);

        sdsl::int_vector<> v(num_of_rows);
        wm wm_aux;
        for(int j = 0; j < num_of_rows; j++){
            v[j] = L[L_last_pos][j];
        }
        construct_im(wm_aux, v);
        wavelet_matrices.push_back(wm_aux);
    }

    //RETRIEVAL
    uint64_t current_value = 0;
    for (int j = 0 ; j < table.size(); j++){
        int row_num=j;
        int current_column_id = row_num;
        std::string tmp_str = "";
        for(int i = 0 ; i < L.size(); i++){
            current_value = L[i][current_column_id];
            tmp_str = std::to_string(current_value) + " " + tmp_str;
            current_column_id = LF(wavelet_matrices[i], C[i], current_column_id, current_value);
            current_column_id -= 1;
        }
        std::cout << "Retrieving row # "<< row_num + 1 << " : " << tmp_str << std::endl; //FALLA EL 1ro y el ultimo. Mayor refactoring.
    }
    //apply_front_coding_and_vlc(D, file);
    auto stop = timer::now();
    sdsl::memory_monitor::stop();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    return 0;
}

//we need to make it work properly. Create a class that stores everything. then retrieve data.
