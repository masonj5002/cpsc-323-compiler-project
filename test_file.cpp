#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

// ASSUMING THAT THIS IS USED FOR A LIST OF LETTERS IN ALPHABETICAL ORDER
int char_to_col(char c)
{
    return (c - 'a');
}

// Finite State Machine for bit strings with letter states
bool fsm(const std::string& w) // w is a bit string basically
{

    std::vector<std::vector<char>> state_table
    {       /*  inputs  */
            /*  0    1  */
        /*A*/ {'a', 'b'},
        /*B*/ {'c', 'c'},
        /*C*/ {'d', 'b'},
        /*D*/ {'a', 'd'}
    };

    std::vector<char> accepting_states = {'b', 'd'};

    // Process the the input string w (similar to doing a trace diagram)
    char current_state = 'a';
    for (int i = 0; i < w.size(); ++i)
    {
        // std::cout << current_state << ' ' << w[i] << '\n';
        current_state = state_table[char_to_col(current_state)][w[i] - '0']; // made a mistake and didn't convert input char to integer
    }

    // std::cout << "final state: " << current_state << '\n';

    // Search through list of accepting states to see if our current state is an acccepting state.
    for (const char& c : accepting_states)
    {
        if (current_state == c)
        {
            return true;
        }
    }

    return false;
}

int main()
{
    
    // std::vector<int> tests(27, 0);
    
    // tests[char_to_col('a')] = 100;
    
    // tests[char_to_col('c')] = 200;
    
    // for (const auto& t : tests) std::cout << t << ' ';
    
    std::cout << std::boolalpha << std::fixed << "result: " << (fsm("011100") ? "accepted" : "not accepted") << '\n';
    std::cout << std::boolalpha << std::fixed << "result: " << (fsm("110011") ? "accepted" : "not accepted") << '\n';

    // std::cout << char_to_col('b');
    return 0;
}


/*
    things to do
    
        - figure out how to seperate line of code into meaningful units
        - lexer() -> seperators fsm -> operators fsm -> keywords fsm -> identifier fsm -> integer fsm -> real fsm

*/
