#include <iostream>
#include <sdsl/vectors.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#include <fstream>

using timer = std::chrono::high_resolution_clock;
//stable_sort with dinamic content is slow for the test data set.
using row = std::deque<uint64_t>;
using matrix = std::vector<row>;
using wm = sdsl::wm_int<sdsl::rrr_vector<15>>;

typedef sdsl::bit_vector C_type;
typedef sdsl::rank_support_v<> C_rank_type;
typedef sdsl::select_support_mcl<> C_select_type;
typedef sdsl::select_support_mcl<0> C_select0_type;

uint64_t LF(wm &wm, C_select_type &C_select, int search_span, uint64_t symbol){
    //std::cout << " rank : " << wm.rank(search_span + 1,symbol) << " C: " << C[symbol] << " Symbol : " << symbol << " search_span : " << search_span + 1 << std::endl;
    std::cout << C_select(1) << " " << C_select(1 +1) << std::endl;
    return C_select(symbol +1)-symbol + wm.rank(search_span + 1,symbol);
}

int main(int argc, char **argv){
    uint64_t i;
    std::string file= argv[1];
    int num_of_rows = 0, num_of_columns = 0;
    std::vector<wm> wavelet_matrices;
    std::vector<C_select_type> C;
    sdsl::memory_monitor::start();
    auto start = timer::now();
    {
        //LOAD STORED WM & C arrays
        std::ifstream ifs(file + ".metadata");
        ifs >> num_of_rows;
        ifs >> num_of_columns;

        for ( int i = 0 ; i < num_of_columns; i++){
            wm wm_aux;
            sdsl::load_from_file(wm_aux, file+"_"+std::to_string(i)+".WM");
            wavelet_matrices.push_back(wm_aux);
            C_type C_bv;
            C_select_type C_select;
            sdsl::load_from_file(C_bv, file + "_"+std::to_string(i)+".C");
            sdsl::util::init_support(C_select,&C_bv);
            C.push_back(C_select);
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
                std::cout << wavelet_matrices[i][current_column_id]<<std::endl;
                current_column_id = LF(wavelet_matrices[i], C[i], current_column_id, current_value);
                current_column_id -= 1;
            }
            std::cout << "Retrieving row # "<< row_num + 1 << " : " << tmp_str << std::endl;
        }
    }
    //apply_front_coding_and_vlc(D, file);
    auto stop = timer::now();
    sdsl::memory_monitor::stop();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    return 0;
}
