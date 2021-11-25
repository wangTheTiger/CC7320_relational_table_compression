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

typedef sdsl::bit_vector C_type;
typedef sdsl::rank_support_v<> C_rank_type;
typedef sdsl::select_support_mcl<> C_select_type;
typedef sdsl::select_support_mcl<0> C_select0_type;

int main(int argc, char **argv){
    uint64_t i;
    std::string file= argv[1];
    int start_row = atoi(argv[2]);
    int row_interval = atoi(argv[3]);
    int end_row = atoi(argv[4]);
    int num_of_rows = 0, num_of_columns = 0;
    //std::vector<C_select_type> C;
    sdsl::memory_monitor::start();
    auto start = timer::now();
    {
        int num_of_rows = 0, num_of_columns = 0, current_row = start_row - 1;
        //TEST LOADING FROM FILE AND RETRIEVING.
        std::ifstream ifs(file + ".metadata");
        ifs >> num_of_rows;
        ifs >> num_of_columns;
        std::cout << "num_of_rows : " << num_of_rows << " num_of_columns : " << num_of_columns << std::endl;
        sdsl::int_vector<32> v(num_of_rows * num_of_columns);
        sdsl::load_from_file(v, file + "_INT_VECTOR");
        std::cout << "Int vector size: " << v.size() << std::endl;
        //retrieval
        std::cout << "start_row : " << start_row << " end_row : " << end_row << " interval : " << row_interval << std::endl;
        for (int j = ( start_row -1 )* num_of_columns ; j < (end_row - 1 ) * num_of_columns; j= j + row_interval * num_of_columns){
            std::string tmp_str = "";
            for ( int x = 0; x < num_of_columns; x++){
                //std::cout << v[j+x] << " ";
                tmp_str += std::to_string(v[j+x])+ " ";
            }
            current_row = current_row + row_interval;
            std::cout << "Retrieving row # "<< current_row << " : " << tmp_str << std::endl;
        }
    }
    auto stop = timer::now();
    sdsl::memory_monitor::stop();
    std::chrono::duration<double> diff = stop - start;
    std::cout << diff.count() << " Milliseconds , " << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    return 0;
}
