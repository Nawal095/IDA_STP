#include "GeneratePDB.h"
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

// Function to generate and save a PDB
void GenerateAndSavePDB(const std::unordered_set<int>& pattern, int variant, const std::string& filepath) {
    GeneratePDB pdb_gen(pattern, variant, filepath);
    pdb_gen.BuildPDB();
    pdb_gen.SaveToFile();
}

int main() {
    // Define patterns and filepaths
    std::vector<std::tuple<std::unordered_set<int>, int, std::string>> tasks = {
	// Test	// {{0,2,3,4,5},1,"DB/test_pdb_v1_0-25.bin"}
	{{0, 1, 2, 3, 4, 5, 6, 7}, 1, "DB/pdb_v1_0-7.vec.bin"},
    {{0, 8, 9, 10, 11, 12, 13, 14, 15}, 1, "DB/pdb_v1_0+8-15.vec.bin"},
    {{0, 1, 2, 3, 4, 5, 6, 7}, 2, "DB/pdb_v2_0-7.bin"},
    {{0, 8, 9, 10, 11, 12, 13, 14, 15}, 2, "DB/pdb_v2_0+8-15.bin"},
    };

    /**
    // Launch threads for each PDB generation task
    std::vector<std::thread> threads;
    for (const auto& [pattern, variant, filepath] : tasks) {
        threads.emplace_back(GenerateAndSavePDB, pattern, variant, filepath);
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    */

    // Process each task sequentially
    for (const auto& [pattern, variant, filepath] : tasks) {
	auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Generating PDB for pattern: ";
        for (int tile : pattern) {
            std::cout << tile << " ";
        }
        std::cout << "(Variant " << variant << ")...\n";
	
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	double minutes = static_cast<double>(duration.count());

    GenerateAndSavePDB(pattern, variant, filepath);

    std::cout << "Saved to: " << filepath << "\n\n";

	std::cout << "Time taken to generate the PDB: "
          << minutes << " microsecs" << std::endl;
    }

    std::cout << "All PDBs generated and saved successfully!\n";
    return 0;
}

// nohup ./ParallelPDBGenerator > /home/mohammad/logs/PDB_generation.log 2>&1 &

// OUTPUT
// =================================
// [0-7]
// Total Nodes expanded: 518918400
// PDB built with 57657600 entries

// [0,8-15]
// Total Nodes expanded: 
// PDB built with 518918400 entries