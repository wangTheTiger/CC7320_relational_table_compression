#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <sdsl/vectors.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
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
    std::vector<uint64_t> v;
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
            }
            entries += 1;
            v.push_back(m[record]);
        }
        counter += 1;
    }
    //Save the data as binary file.
    std::ofstream sof( file_name + "_mapping.dat" );
    boost::archive::binary_oarchive oarch2(sof);
    oarch2 &v;

    std::cout << " rows processed : " << counter << " |alphabet| : " << m.size() << " |entries| : "<< entries << " vector size in bytes " << v.size() * 8 <<std::endl;

    auto stop = timer::now();
    sdsl::memory_monitor::stop();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(stop-start).count() << " seconds." << std::endl;
    std::cout << sdsl::memory_monitor::peak() << " bytes." << std::endl;
    exit(EXIT_SUCCESS);
}
