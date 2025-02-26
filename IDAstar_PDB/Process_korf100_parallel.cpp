#include "IDAstar.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <iomanip> // For std::setprecision
#include <atomic>
#include <stdexcept> // For std::runtime_error

std::mutex cout_mutex; // Mutex for protecting std::cout

std::vector<uint8_t> ReadPDB(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open PDB file: " + filename);
    }
    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

std::unordered_set<int> ParsePattern(const std::string& pattern_str) {
    std::unordered_set<int> pattern;
    std::string cleaned = pattern_str.substr(1, pattern_str.size() - 2); // Remove '{' and '}'
    // std::cout << "Cleaned Pattern Read: " << cleaned << std::endl;
    std::istringstream iss(cleaned);
    std::string token;
    while (std::getline(iss, token, ',')) {
        pattern.insert(std::stoi(token));
    }
    // Print the Pattern
    std::cout << "Final Pattern: " << std::endl;
    for (int p : pattern)
        std::cout << p << "\t";
    std::cout << std::endl;

    return pattern;
}

// Callback function to be used within IDAstar::Solve
void print_iteration_info(int core_num, int bound, long long expanded, long long generated, std::ofstream& outfile) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::string info = "Core_" + std::to_string(core_num) + ":\tStarting iteration with bound " + std::to_string(bound) + 
                       "; " + std::to_string(expanded) + " expanded, " + std::to_string(generated) + " generated\r";

    std::cout << info;

    // outfile << info << "\n";
}


// Local PrintState function (copy of the printing logic)
void PrintStateToFile(const Puzzle& puzzle, std::ostream& os) {
    const auto& state = puzzle.GetTiles(); // Use the getter!
    for (int i = 0; i < 16; ++i) {
        os << state[i] << " ";
        if ((i + 1) % 4 == 0) {
            os << "\n";
        }
    }
}

void SolvePuzzleAndWrite(
    int puzzle_num, 
    Puzzle puzzle, 
    int core_num,
    const std::vector<uint8_t>& pdb1,
    const std::vector<uint8_t>& pdb2,
    const std::unordered_set<int>& pattern1,
    const std::unordered_set<int>& pattern2) {

    IDAstar solver;
    std::ofstream outfile("../Data/result_Puzzle_" + std::to_string(puzzle_num) + ".txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open result_Puzzle_" << puzzle_num << ".txt for writing\n";
        return;
    }

    outfile << "IDA* working to solve Puzzle " << puzzle_num << ":\n";
    outfile << "################################";
    outfile << "\nInitial State:\n===============\n";
    PrintStateToFile(puzzle, outfile); // Use the local function
    outfile << "\n";

    // Pass the callback to the Solve function
    auto result = solver.Solve(puzzle, pdb1, pdb2, pattern1, pattern2,
        [](int core_num, int bound, long long expanded, long long generated, std::ofstream &outfile) {
            print_iteration_info(core_num, bound, expanded, generated, outfile);
        }, 
        core_num, outfile);
    
    outfile << "IDA*: " << std::fixed << std::setprecision(2) << std::get<2>(result) << "s elapsed; " // Time (double)
        << std::get<3>(result) << " expanded; "                                      // Expanded nodes (long long)
        << std::get<4>(result) << " generated; "                                     // Generated nodes (long long)
        << "solution length " << std::get<1>(result) << "\n";    

    outfile << "\nGoal State:\n===============\n";
    PrintStateToFile(std::get<5>(result), outfile); // Use the local function
    outfile << "\n";                    // Moves (int)

    outfile << "\nPath: \n===============\n";
    for (const auto& action : std::get<0>(result)) {
        std::string dir_str;
        switch (action.dir) {
            case Left: dir_str = "Left"; break;
            case Right: dir_str = "Right"; break;
            case Up: dir_str = "Up"; break;
            case Down: dir_str = "Down"; break;
        }
        outfile << "Direction: " << dir_str << ", Steps: " << action.steps << "\n";
    }
    outfile.close();

    { // Protect std::cout with a mutex
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Core_" << core_num << ": FINISHED EXECUTION\n";
        std::cout << "###################################\n";
    }
}

void ReadPuzzles(const std::string& filename, std::vector<Puzzle>& puzzles, int variant) {
    std::ifstream infile(filename);
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::array<int, 16> initial_state; // Use std::array<int, 16> instead of std::vector<int>
        int var;
        iss >> var; // Read puzzle number (ignored for now)
        for (int& tile : initial_state) {
            iss >> tile;
        }
        puzzles.emplace_back(initial_state, variant);
    }
    std::cout << "Read " << puzzles.size() << " puzzles from " << filename << "\n"; // Debugging output
}

int main(int argc, char* argv[]) {
    std::cout << "argc = " << argc << std::endl;

    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] 
                  << " <pdb1_file> <pattern1> <pdb2_file> <pattern2> <variant>\n"
                  << "Example: " << argv[0] << "\t" << argv[1] << "\t" << argv[2] << "\t" << argv[3] << "\t" << argv[4] << "\t" << argv[5] << "\n";
                  // << " ../PDB/DB/pdb_v1_0-7.vec.bin {1,2,3,4,5,6,7} ../PDB/DB/pdb_v1_0+8-15.vec.bin {8,9,10,11,12,13,14,15} 1\n";
        return 1;
    }

    // Read PDB files
    std::vector<uint8_t> pdb1, pdb2;
    try {
        pdb1 = ReadPDB(argv[1]);
        pdb2 = ReadPDB(argv[3]);
        std::cout << "PDBs have been read into memory." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error reading PDB files: " << e.what() << "\n";
        return 1;
    }

    // Parse patterns
    std::unordered_set<int> pattern1 = ParsePattern(argv[2]);
    std::unordered_set<int> pattern2 = ParsePattern(argv[4]);
    std::cout << "Patterns have been read into memory." << std::endl;

    int variant = std::stoi(argv[5]);

    std::vector<Puzzle> puzzles;
    ReadPuzzles("../Data/korf100.txt", puzzles, variant);

    int num_cores = 50; // Number of cores to use
    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < puzzles.size(); ++i) {
        int core_num = i % num_cores; // Assign puzzles to cores in round-robin fashion
        futures.push_back(std::async(std::launch::async, SolvePuzzleAndWrite, i + 1, puzzles[i], core_num, std::cref(pdb1), std::cref(pdb2), std::cref(pattern1), std::cref(pattern2)));
    }

    // Wait for all threads to finish
    for (auto& future : futures) {
        future.get(); 
    }

    std::cout << "All puzzles processed.\n";
    return 0;
}