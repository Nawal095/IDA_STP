#include "Puzzle.h"
#include <fstream>
#include <sstream>
#include <initializer_list>
#include <random>

void RunTests(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Cannot open test file!" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        int test_num;
        iss >> test_num;
        
        // Use std::array<int, 16> instead of std::vector<int>
        std::array<int, 16> tiles;
        for (int i = 0; i < 16; ++i) {
            int val;
            iss >> val;
            tiles[i] = val; // Assign to array
        }

        for (int variant : {1, 2}) {
            Puzzle puzzle(tiles, variant);
            cout << "Test #" << test_num << " Variant " << variant << endl;
            cout << "Initial state:" << endl;
            puzzle.PrintState();

            cout << "Is goal state? " << (puzzle.GoalTest() ? "Yes" : "No") << endl;

            auto actions = puzzle.GetPossibleActions();
            cout << "Possible actions (" << actions.size() << "):" << endl;
            for (const auto& a : actions) {
                string dir;
                switch (a.dir) {
                    case Left:  dir = "Left";   break;
                    case Right: dir = "Right";  break;
                    case Up:    dir = "Up";     break;
                    case Down:  dir = "Down";   break;
                }
                cout << " - " << dir << " " << a.steps << " step(s)" << endl;
            }

            if (!actions.empty()) {
                // Randomly select an action
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distr(0, actions.size() - 1);
                Action random_action = actions[distr(gen)];

                cout << "\nApplying random action: " << random_action.steps << " step(s) ";
                switch (random_action.dir) {
                    case Left:  cout << "Left";  break;
                    case Right: cout << "Right"; break;
                    case Up:    cout << "Up";    break;
                    case Down:  cout << "Down";  break;
                }
                cout << endl;

                // Store the original state as an array
                std::array<int, 16> original = puzzle.GetTiles();
                puzzle.ApplyAction(random_action);
                cout << "State after action:" << endl;
                puzzle.PrintState();

                puzzle.UndoAction(random_action);
                cout << "State after undo:" << endl;
                puzzle.PrintState();

                // Compare arrays directly
                if (original != puzzle.GetTiles()) {
                    cerr << "Error: Undo action failed!" << endl;
                }
            }

            // Add a test case to ensure invalid vertical moves are caught
            vector<Action> invalid_actions = {
                {Up, 2}, {Down, 2}, {Up, 3}, {Down, 3}
            };

            for (const auto& a : invalid_actions) {
                cout << "\nTesting invalid action: " << a.steps << " step(s) ";
                switch (a.dir) {
                    case Up:    cout << "Up";    break;
                    case Down:  cout << "Down";  break;
                    default:    continue; // Skip non-vertical actions
                }
                cout << endl;

                // Store the original state as an array
                std::array<int, 16> original = puzzle.GetTiles();
                puzzle.ApplyAction(a);
                if (original != puzzle.GetTiles()) {
                    cerr << "Error: Invalid vertical action was incorrectly applied!" << endl;
                } else {
                    cout << "Correctly identified and prevented invalid vertical action." << endl;
                }
            }
            
            cout << "----------------------------------------" << endl;
        }
    }
}

int main() {
    RunTests("../Data/korf100.txt");
    return 0;
}