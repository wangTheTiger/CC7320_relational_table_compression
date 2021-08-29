#include <iostream>
#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>
#include <fstream>

typedef std::tuple<uint64_t, uint64_t, uint64_t> spo_triple;

using timer = std::chrono::high_resolution_clock;
//stable_sort with dinamic content is slow for the test data set.
//using matrix = std::vector<std::vector<uint64_t>>;
using matrix = std::vector<spo_triple>;

void apply_front_coding_and_vlc(std::vector<spo_triple> &D, std::string filename){
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
        sdsl::store_to_file(ev_d, filename + ".elias_delta");
        //std::cout << "encoded vector size : " << ev_d.size() << std::endl;
        std::cout << "Elias delta encoded vector size in megabytes      : "<< sdsl::size_in_mega_bytes(ev_d) << std::endl;
    }
    {
        sdsl::vlc_vector<sdsl::coder::elias_gamma> ev_g(v);
        //std::cout << "ev_g: " << ev_g << std::endl;
        sdsl::store_to_file(ev_g, filename + ".elias_gamma");
        //std::cout << "encoded vector size : " << ev_g.size() << std::endl;
        std::cout << "Elias gamma encoded vector size in megabytes      : "<< sdsl::size_in_mega_bytes(ev_g) << std::endl;
    }
    {
        sdsl::vlc_vector<sdsl::coder::fibonacci> ev_f(v);
        //std::cout << "ev_f: " << ev_f << std::endl;
        sdsl::store_to_file(ev_f, filename + ".fibonacci");
        //std::cout << "encoded vector size : " << ev_f.size() << std::endl;
        std::cout << "Fibonacci encoded vector size in megabytes        : "<< sdsl::size_in_mega_bytes(ev_f) << std::endl;
    }
}

std::vector<uint64_t> get_column_as_vector(int column_id, matrix &sorted_table){
    std::vector<uint64_t> l;//TODO: how to set the size here. should I use an std::array instead?
    int i;
    for (i = 0; i < sorted_table.size(); i++){
        //l.push_back(sorted_table[i][column_id]);
        l.push_back(std::get<2>(sorted_table[i]));
    }
    l.shrink_to_fit();
    return l;

}
/*
void swap_columns(matrix &sorted_table, int col1, int col2){
    int i;
    for (i = 0; i < sorted_table.size(); i++){
        sorted_table[i]
    }
}*/
int main(int argc, char **argv){
    uint64_t i;
    //int rows = 6, columns = 3;
    //matrix name(rows, std::vector<double>(columns));
    matrix table;
    std::string filename= argv[1];

    sdsl::memory_monitor::start();
    auto start = timer::now();
    std::ifstream ifs(filename);
    uint64_t s, p , o;
    do {
        //TODO: only ints for initial version, to be improved.
        ifs >> s >> p >> o;
        std::vector<uint64_t> aux;
        aux.push_back(s);
        aux.push_back(p);
        aux.push_back(o);
        //table.push_back(aux);
        table.push_back(spo_triple(s, p, o));
        
    } while (!ifs.eof());

    table.shrink_to_fit();
    //std::cout << "--Loaded " << table.size() << " rows" << " with " << table[0].size() << " columns each. " << std::endl;
    std::cout << "--Loaded " << table.size() << " rows" << " with 3 columns each. " << std::endl;
    //lexicographic table sorting. 
    matrix::iterator it, table_begin = table.begin(), triple_end = table.end();
    std::sort(table_begin, triple_end);//TODO: probar con stable_sort instead. es necesario? cuan lento es comparativamente?
    
    //std::vector<uint64_t> l_k= get_column_as_vector(table[0].size() - 1, table);
    std::vector<uint64_t> l_k= get_column_as_vector(2, table);
    /*
    swap_columns(table, 0, 2);
    std::sort(table_begin, triple_end);
    std::vector<uint64_t> l_k_1= get_column_as_vector(1, table);
    swap_columns(table, 0, 1);
    std::sort(table_begin, triple_end);
    std::vector<uint64_t> l_k_2= get_column_as_vector(0, table);
    */
    //apply_front_coding_and_vlc(D, filename);
    auto stop = timer::now();
    sdsl::memory_monitor::stop();
    
    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    return 0;
}

