#include "IDAstar_variant_one.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>

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

void WriteResult(std::ofstream& outfile, int puzzle_num, const std::tuple<std::vector<Action>, int, double, int, int>& result) {
    outfile << "Puzzle " << puzzle_num << ":\n";
    outfile << "Moves: " << std::get<1>(result) << "\n";
    outfile << "Time: " << std::get<2>(result) << " seconds\n";
    outfile << "Nodes Expanded: " << std::get<3>(result) << "\n";
    // outfile << "Nodes Generated: " << std::get<4>(result) << "\n";
    outfile << "Path: \n";
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
    outfile << "\n";
}

int main() {
    std::vector<Puzzle> puzzles;
    ReadPuzzles("../Data/korf100.txt", puzzles);

    std::ofstream outfile("../Data/result.txt");
    if (!outfile.is_open()) {
        std::cerr << "Failed to open result.txt for writing\n";
        return 1;
    }

    IDAstar solver;

    for (size_t i = 0; i < puzzles.size(); ++i) {
        std::cout << "Solving Puzzle " << i + 1 << " with initial state:\n";
        puzzles[i].PrintState(); // Print initial state for debugging

        auto result = solver.Solve(puzzles[i]);
        
        // Write results to file after solving each puzzle
        WriteResult(outfile, i + 1, result);

        if (puzzles[i].GoalTest()) {
            std::cout << "\nPuzzle " << i + 1 << ": Solved!\n";
        } else {
            std::cout << "\nPuzzle " << i + 1 << ": Not Solved!\n";
        }

        std::cout << "Moves: " << std::get<1>(result) << "\n";
        std::cout << "Time: " << std::get<2>(result) << " seconds\n";
        std::cout << "Nodes Expanded: " << std::get<3>(result) << "\n";
        // std::cout << "Nodes Generated: " << std::get<4>(result) << "\n";
        std::cout << "-------------------------------------------\n";
    }

    outfile.close();
    std::cout << "Results written to result.txt\n";
    return 0;
}