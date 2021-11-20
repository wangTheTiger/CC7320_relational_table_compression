#include <iostream>
#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#include <fstream>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using timer = std::chrono::high_resolution_clock;
//stable_sort with dinamic content is slow for the test data set.
using row = std::deque<uint64_t>;
using matrix = std::vector<row>;
using wm = sdsl::wm_int<sdsl::rrr_vector<15>>;

/**
 * get_last_column_as_vector retrieves a std::vector<uint64_t> corresponding to the last column of table.
 * It also calculates C vector used for LP mapping.
 * */
std::vector<uint64_t> get_last_column_as_vector(matrix &table, std::map<uint64_t, uint64_t> &C){//TODO: future work, do C as bit_vector (larger alphabets)
    std::vector<uint64_t> l;//TODO: how to set the size here. should I use an std::array instead?
    int i;
    
    for (i = 0; i < table.size(); i++){
        uint64_t number = table[i][table[i].size() - 1];
        C[number] = C[number] + 1;
        l.push_back(number);
    }
    //C as std::map<uint64_t, uint64_t> &C
    uint64_t count = 0, key = 0, aux = 0;
    std::map<uint64_t, uint64_t>::iterator it, end = C.end();
    //std::cout << "C: ";
    for (i = 0, it = C.begin(); it != end; it++, i++){
        //gets the value of the map.
        key = it->first;
        aux = it->second;
        C[key] = count;
        //std::cout << key << ":" << count << ' ';
        count += aux;
    }
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
    //std::cout << "L: ";
    for (i = 0, it = L.begin(); it != end; it++, i++){
        std::cout << L[i] << ' ';
    }
    std::cout << std::endl;
}

uint64_t LF(wm &wm, std::map<uint64_t, uint64_t> &C, int search_span, uint64_t symbol){
    //std::cout << "result: " << C[symbol] + wm.rank(search_span + 1,symbol) << " rank : " << wm.rank(search_span + 1,symbol) << " C: " << C[symbol] << " Symbol : " << symbol << " search_span : " << search_span + 1 << std::endl;
    return C[symbol] + wm.rank(search_span + 1,symbol);
}
int main(int argc, char **argv){
    uint64_t i;
    std::string file= argv[1];
    char delim = ' ';
    int num_of_rows = 0;
    //std::vector<wm> wavelet_matrices;//WE actually dont need to store the L_i! TODO: define rrr_vector block param
    //Custom C representation - TODO: think about migrating it entirely to SDSL way of doing C array.
    std::vector<std::map<uint64_t, uint64_t>> C;
    sdsl::memory_monitor::start();
    auto start = timer::now();
    /*********************** PART 1 : Read input file ***********************/
    {
        std::ifstream ifs(file);
        std::string str_line;
        matrix table;
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
        num_of_rows = table.size();
        std::cout << "--Loaded " << num_of_rows << " rows" << " with " << table[0].size() << " columns each. " << std::endl;
        /*********************** PART 2 : Process last column (k) ***********************/
        //lexicographic table sorting.
        matrix::iterator it, table_begin = table.begin(), triple_end = table.end();
        std::sort(table_begin, triple_end);//TODO: try with stable_sort. Is it necessary? How slow is it in comparison with the former?

        std::vector<std::vector<uint64_t>> L;
        int L_last_pos = 0, num_of_columns = table[0].size();
        //adding the last column of the table as L_j, with j in {1,..,k} backwardly.

        std::map<uint64_t, uint64_t> c_aux;
        L.push_back(get_last_column_as_vector(table, c_aux));
        {
            //Save c_aux map.
            std::ofstream sof( file + "_0.C");
            boost::archive::binary_oarchive oarch2(sof);
            //oarch2 << map;
            // Save the data
            oarch2 &c_aux;
        }
        L[L_last_pos].shrink_to_fit();
        //C.push_back(c_aux); // to be commented
        //print_L(L[L_last_pos]);

        //building Wavelet tree of L_i.
        sdsl::int_vector<> v(num_of_rows);

        wm wm_aux;
        for(int j = 0; j < num_of_rows; j++){
            v[j] = L[L_last_pos][j];
        }

        std::cout << "creating Wavelet trees"  << std::endl;
        construct_im(wm_aux, v);
        //wavelet_matrices.push_back(wm_aux);//--to be commented
        sdsl::store_to_file(wm_aux, file + "_0.WM");
        std::cout << " > wavelet tree #1 ready" << std::endl;
        /*********************** PART 2 : Process k-1 ..+ 1 column ***********************/
        for(i = 1; i < num_of_columns; i++){
            std::map<uint64_t, uint64_t> c_aux;
            move_last_column_to_front(table);
            std::sort(table_begin, triple_end);
            //pop_first_column(table);
            L.push_back(get_last_column_as_vector(table, c_aux));
            {
                //Save c_aux map.
                std::ofstream sof( file + "_"+std::to_string(i)+".C");
                boost::archive::binary_oarchive oarch2(sof);
                //oarch2 << map;
                // Save the data
                oarch2 &c_aux;
            }
            L_last_pos = L.size() - 1;
            L[L_last_pos].shrink_to_fit();
            //C.push_back(c_aux);//to be commented
            //print_L(L[L_last_pos]);

            sdsl::int_vector<> v(num_of_rows);
            wm wm_aux;
            for(int j = 0; j < num_of_rows; j++){
                v[j] = L[L_last_pos][j];
            }
            construct_im(wm_aux, v);
            //wavelet_matrices.push_back(wm_aux); // to be commented
            sdsl::store_to_file(wm_aux, file + "_"+std::to_string(i)+".WM");
            std::cout << " > wavelet tree #" <<  i+1 << " ready" << std::endl;
        }
        std::ofstream ofs(file + ".metadata");
        ofs << num_of_rows << std::endl;
        ofs << num_of_columns << std::endl;
        ofs.close();
    }
    /*{
        //RETRIEVAL - old method if we want to do it in one step.
        uint64_t current_value = 0;
        for (int j = 0 ; j < num_of_rows; j++){
            int row_num=j;
            int current_column_id = row_num;
            std::string tmp_str = "";
            for(int i = 0 ; i < wavelet_matrices.size(); i++){ // all of the WM have the same size.
                current_value = wavelet_matrices[i][current_column_id];//TODO> why do I need L to get current value? can't I use the wavelet matrix instead?
                tmp_str = std::to_string(current_value) + " " + tmp_str;
                //std::cout << wavelet_matrices[i][current_column_id]<<std::endl;
                current_column_id = LF(wavelet_matrices[i], C[i], current_column_id, current_value);
                current_column_id -= 1;
            }
            std::cout << "Retrieving row # "<< row_num + 1 << " : " << tmp_str << std::endl; 
        }
    }*/


    /*
    {
        int num_of_rows = 0, num_of_columns = 0;
        std::vector<wm> wavelet_matrices;
        std::vector<std::map<uint64_t, uint64_t>> C;
        //TEST LOADING FROM FILE AND RETRIEVING.
        //LOAD STORED WM & C arrays
        std::ifstream ifs(file + ".metadata");
        ifs >> num_of_rows;
        ifs >> num_of_columns;

        for ( int i = 0 ; i < num_of_columns; i++){
            wm wm_aux;
            sdsl::load_from_file(wm_aux, file+"_"+std::to_string(i)+".WM");
            wavelet_matrices.push_back(wm_aux);
            //loading C's
            {
                std::map<uint64_t, uint64_t> C_aux;
                std::ifstream sif( file+"_"+std::to_string(i)+".C");
                boost::archive::binary_iarchive iarch2(sif);
                iarch2 &C_aux;
                C.push_back(C_aux);
            }
        }
        //RETRIEVAL
        uint64_t current_value = 0;
        for (int j = 0 ; j < num_of_rows; j++){
            int row_num=j;
            int current_column_id = row_num;
            std::string tmp_str = "";
            for(int i = 0 ; i < wavelet_matrices.size(); i++){ // all of the WM have the same size.
                current_value = wavelet_matrices[i][current_column_id];//TODO> why do I need L to get current value? can't I use the wavelet matrix instead?
                tmp_str = std::to_string(current_value) + " " + tmp_str;
                //std::cout << wavelet_matrices[i][current_column_id]<<std::endl;
                current_column_id = LF(wavelet_matrices[i], C[i], current_column_id, current_value);
                current_column_id -= 1;
            }
            std::cout << "Retrieving row # "<< row_num + 1 << " : " << tmp_str << std::endl;
        }
    }
*/
    //apply_front_coding_and_vlc(D, file);
    auto stop = timer::now();
    sdsl::memory_monitor::stop();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    return 0;
}
