#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cstring> // for strcmp

void printHelp() {
    std::cout << "Usage: ./print_range <filename> <start> <end>\n"
              << "Options:\n"
              << "  --h       Show this help message\n"
              << "Arguments:\n"
              << "  filename  Path to the binary file\n"
              << "  start     Start index of the range (inclusive)\n"
              << "  end       End index of the range (inclusive)\n"
              << "Example:\n"
              << "  ./print_range pdb.bin 10 15\n";
}

void printRangeFromFile(const std::string& filename, int start, int end) {
    // Open the binary file
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    // Read the entire file into a vector
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Check if the range is valid
    if (start < 0 || end >= data.size() || start > end) {
        std::cerr << "Error: Invalid range [" << start << ", " << end << "]" << std::endl;
        return;
    }

    // Print the indices and values in the range
    
    std::cout << "Vector Size:" << data.size() << std::endl;
    std::cout << "Values in range [" << start << ", " << end << "]:" << std::endl;
    for (long long i = start; i <= end; ++i) {
        std::cout << i << ": " << static_cast<int>(data[i]) << "\t";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Check for help option
    if (argc == 2 && strcmp(argv[1], "--h") == 0) {
        printHelp();
        return 0;
    }

    // Validate number of arguments
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments.\n";
        printHelp();
        return 1;
    }

    // Parse arguments
    std::string filename = argv[1];
    long long start = std::stoi(argv[2]);
    long long end = std::stoi(argv[3]);

    // Call the function to print the range
    printRangeFromFile(filename, start, end);

    return 0;
}

// g++ print_range.cpp -o print_range
// ./print_range ../DB/test_pdb_v1_0-25.bin 43670 43679