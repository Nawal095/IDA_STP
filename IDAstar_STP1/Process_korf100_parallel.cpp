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

std::mutex cout_mutex; // Mutex for protecting std::cout

// Callback function to be used within IDAstar::Solve
void print_iteration_info(int core_num, int bound, long long expanded, long long generated, std::ofstream& outfile) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::string info = "Core_" + std::to_string(core_num) + ":\tStarting iteration with bound " + std::to_string(bound) + 
                       "; " + std::to_string(expanded) + " expanded, " + std::to_string(generated) + " generated\r";

    std::cout << info;

    outfile << info << "\n";
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

void SolvePuzzleAndWrite(int puzzle_num, Puzzle puzzle, int core_num) {
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
    auto result = solver.Solve(puzzle, 
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

    outfile << "\nPath: \n";
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

void ReadPuzzles(const std::string& filename, std::vector<Puzzle>& puzzles) {
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
        puzzles.emplace_back(initial_state, 1); // Using variant 1
    }
    std::cout << "Read " << puzzles.size() << " puzzles from " << filename << "\n"; // Debugging output
}

int main() {
    std::vector<Puzzle> puzzles;
    ReadPuzzles("../Data/korf100.txt", puzzles);

    int num_cores = 6; // Number of cores to use
    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < puzzles.size(); ++i) {
        int core_num = i % num_cores; // Assign puzzles to cores in round-robin fashion
        futures.push_back(std::async(std::launch::async, SolvePuzzleAndWrite, i + 1, puzzles[i], core_num));
    }

    // Wait for all threads to finish
    for (auto& future : futures) {
        future.get(); 
    }

    std::cout << "All puzzles processed.\n";
    return 0;
}