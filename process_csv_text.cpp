#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <sdsl/vectors.hpp>

using timer = std::chrono::high_resolution_clock;

std::string readFileIntoString(const std::string& path) {
    auto ss = std::ostringstream{};
    std::ifstream input_file(path);
    if (!input_file.is_open()) {
        std::cerr << "Could not open the file - '"
             << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    ss << input_file.rdbuf();
    return ss.str();
}

int main(int argc, char **argv){
    //TODO: save the content as binary file.
    std::string file_name= argv[1];

    sdsl::memory_monitor::start();
    auto start = timer::now();
    //CSV input processing
    std::string file_contents;
    char delimiter = ',';

    file_contents = readFileIntoString(file_name);

    std::istringstream sstream(file_contents);
    std::string record;

    int num_of_rows = 0, num_of_columns = 0, columns_processed = 0;
    int current_id = 1;
    //output file.
    std::ofstream output_file( file_name + "_mapping.dat" );
    //Hash mapping O(1)
    std::unordered_map<std::string, int> m;
    //bypassing first line cause in CSV contains column names
    std::getline(sstream, record);
    while (std::getline(sstream, record)) {
        std::istringstream line(record);
        columns_processed = 0;
        while (std::getline(line, record, delimiter)) {
            //Limiting to 14 columns max. cause DL_ATTEMPTS_EXPORT.CSV has an issue. some columns have 14 and some other 15.
            if(columns_processed >= 14){
                break;
            }
            //check if entry is in map, if is not then define it.
            if (!m[record]){
                m[record] = current_id++;
            }
            //save it to an output file
            output_file << m[record]  << " ";
            //output_file << record  << " ";
            if(num_of_rows == 0){
                num_of_columns ++;
            }
            columns_processed++;
        }
        output_file << '\n' << std::flush;
        num_of_rows += 1;
        //std::cout << " columns_processed : " << columns_processed << std::endl;
    }
    std::cout << num_of_rows << " rows processed, with " << num_of_columns << " columns. File saved: " << file_name + "_mapping.dat" << std::endl;

    auto stop = timer::now();
    sdsl::memory_monitor::stop();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    exit(EXIT_SUCCESS);
}
