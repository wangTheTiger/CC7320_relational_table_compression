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
typedef sdsl::select_support_mcl<> C_select_type;

typedef sdsl::bit_vector C_type;
typedef sdsl::rank_support_v<> C_rank_type;
typedef sdsl::select_support_mcl<> C_select_type;
typedef sdsl::select_support_mcl<0> C_select0_type;

C_type *C_bv;
C_rank_type C_rank;
C_select_type C_select;
C_select0_type C_select0;
/**
 * get_last_column_as_vector retrieves a std::vector<uint64_t> corresponding to the last column of table.
 * It also calculates C vector used for LP mapping.
 * */
std::vector<uint64_t> get_last_column_as_vector(matrix &table, std::string filename){//TODO: Make it a class so C_bv is a member and not an output parameter
    std::vector<uint64_t> l;//TODO: how to set the size here. should I use an std::array instead?
    int i;
    uint64_t num_rows = table.size();
    //calculate alphabet. It also calculates L in first loop
    std::set<uint64_t> alphabet;
    int last_column_id = table[0].size() - 1;
    for (i = 0; i < num_rows; i++){
        uint64_t number = table[i][last_column_id];
        alphabet.insert(number);
        l.push_back(number);
    }
    uint64_t max_alphabet = *alphabet.rbegin();
    std::cout << " max_alphabet: " << max_alphabet << " alphabet size : " << alphabet.size() << " num_rows : " << num_rows << " last_column_id: "<< last_column_id << std::endl;
    std::vector<uint64_t> v_aux(max_alphabet+1);
    for (i = 0; i <= max_alphabet; i++){
        //std::cout << " v_aux attempting to store 0 in pos: " << i << std::endl;
        v_aux[i] = 0;
    }
    for (i = 0; i < num_rows; i++){
        //std::cout << " table[i][last_column_id] : " << table[i][last_column_id] << std::endl;
        v_aux[table[i][last_column_id]]++;
    }
    //Calculate C array
    uint64_t cur_pos = 1;
    std::vector<uint64_t> C_;
    C_.push_back(0); // Dummy value
    C_.push_back(cur_pos);
    for (int c = 2; c <= max_alphabet; c++){
        cur_pos += v_aux[c - 1];
        C_.push_back(cur_pos);
    }
    C_.push_back(num_rows + 1);
    C_.shrink_to_fit();

    alphabet.clear();
    v_aux.clear();
    //Create C array as bit vector ( saves space )
    sdsl::bit_vector C_aux = sdsl::bit_vector(C_[C_.size()-1]+1+C_.size(), 0);

    for (uint64_t i=0; i < C_.size(); i++) {
        //std::cout << " en pos C_["<<i<<"] : " << C_[i] << "+i = " << C_[i]+i << " se asigna un 1"<< std::endl;
        C_aux[C_[i]+i] = 1;
    }
    C_bv = new C_type(C_aux);
    sdsl::util::init_support(C_rank,C_bv);
    sdsl::util::init_support(C_select,C_bv);
    sdsl::util::init_support(C_select0,C_bv);
    std::cout << "C_bv : " << *C_bv << " C_bv->size() : " << C_bv->size() << std::endl;

    sdsl::store_to_file(*C_bv, filename);
    //3. return the last column as vector
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
    //std::vector<std::map<uint64_t, uint64_t>> C;
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

        sdsl::bit_vector c_bitvector;
        L.push_back(get_last_column_as_vector(table,file + "_0.C"));
        L[L_last_pos].shrink_to_fit();
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
            move_last_column_to_front(table);
            std::sort(table_begin, triple_end);
            //pop_first_column(table);
            L.push_back(get_last_column_as_vector(table, file + "_"+std::to_string(i)+".C"));
            L_last_pos = L.size() - 1;
            L[L_last_pos].shrink_to_fit();
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
    auto stop = timer::now();
    sdsl::memory_monitor::stop();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    return 0;
}
