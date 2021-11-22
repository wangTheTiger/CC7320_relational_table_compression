#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <sdsl/vectors.hpp>
#include <boost/serialization/unordered_map.hpp>
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

    int counter = 0, entries = 0;
    uint64_t current_id = 1;
    //Hash mapping O(1)
    std::unordered_map<std::string, uint64_t> m;
    //bypassing first line cause in CSV contains column names
    std::getline(sstream, record);
    while (std::getline(sstream, record)) {
        std::istringstream line(record);
        while (std::getline(line, record, delimiter)) {
            //check if entry is in map, if is not then define it.
            if (!m[record]){
                m[record] = current_id++;
                //std::cout << "assigning " << current_id << " to m[" << record << "]"<<std::endl;
            }
            entries += 1;
        }
        counter += 1;
    }
    //now we know the number of entries, we loop again.
    int i=0;
    int in_if = 0;
    std::istringstream sstream2(file_contents);
    //Int vector size, # of entries.
    sdsl::int_vector<32> v(entries);//para alfabeto de largo 18.256.946, se necesitan 25 bits, es decir 5 bytes.
    while (std::getline(sstream2, record)) {
        std::istringstream line(record);
        while (std::getline(line, record, delimiter)) {
            if (m[record]){
                v[i++] = m[record];
                in_if++;
            }
        }
    }
    //Save the data as binary file.
    sdsl::store_to_file(v, file_name + "_mapping.dat");
    //sdsl::store_to_file(v, file_name + "_mapping.dat");
    std::cout << " rows processed : " << counter << " |alphabet| : " << m.size() << " |entries| : "<< entries << " bytes per entry :"<< sdsl::size_in_bytes(v) / entries <<" in_if: " << in_if << std::endl;
    //std::cout << v << std::endl;
    std::cout << "int vector size in bytes : " << sdsl::size_in_bytes(v) << std::endl;
    sdsl::util::bit_compress(v);
    std::cout << "int vector size in bytes (after compression): " << sdsl::size_in_bytes(v) << std::endl;
    auto stop = timer::now();
    sdsl::memory_monitor::stop();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    exit(EXIT_SUCCESS);
}
