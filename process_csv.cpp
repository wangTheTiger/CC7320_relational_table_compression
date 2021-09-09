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


/*
//saving additional mapping structures.
        std::ofstream ext_to_int_ofs( filename + "_ext_to_int.dat" );
        for(auto &item : ext_to_int){
            ext_to_int_ofs << item.first << ' ' << item.second << endl;
        }
        ext_to_int_ofs.close();
        //load additional mapping structures
        std::ifstream ext_to_int_ifs( filename + "_ext_to_int.dat" );
        u_int64_t key, value;
        while (ext_to_int_ifs >> key >> value){
            if(!ext_to_int[key]){
                ext_to_int[key] = value;
            }
        }
        ext_to_int_ifs.close();
*/
int main(int argc, char **argv){
    std::string file_name= argv[1];

    sdsl::memory_monitor::start();
    auto start = timer::now();
    //CSV input processing
    std::string file_contents;
    char delimiter = ',';

    file_contents = readFileIntoString(file_name);

    std::istringstream sstream(file_contents);
    std::string record;

    int counter = 0;
    int current_id = 1;
    //output file.
    std::ofstream output_file( file_name + "_mapping.dat" );
    //Hash mapping O(1)
    std::unordered_map<std::string, int> m;
    //bypassing first line cause in CSV contains column names
    std::getline(sstream, record);
    while (std::getline(sstream, record)) {
        std::istringstream line(record);
        while (std::getline(line, record, delimiter)) {
            //save it to an output file
            if (!m[record]){
                m[record] = current_id++;
            }
            output_file << m[record]  << " ";
            //output_file << record  << " ";
        }
        output_file << '\n' << std::flush;
        counter += 1;
    }
    std::cout << counter << " rows processed " << std::endl;

    auto stop = timer::now();
    sdsl::memory_monitor::stop();
    
    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    exit(EXIT_SUCCESS);
}
