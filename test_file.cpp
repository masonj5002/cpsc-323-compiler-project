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


void lex_scan(const std::string& line)
{
    std::vector<std::vector<int>> state_table; // define this in the future

    std::string column_string = ".''{}(*)$:=<> ";

    std::string alph = "_ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::string digits = "#0123456789";

    int start_pointer, current_char_pointer, token_class, column;

    char current_char;
    
    std::string token;

    // lex_state - global static integer var. (initially 1 before first call to lex_scan)

    current_char_pointer = 1;
    start_pointer        = 1;

    while ( current_char_pointer <= line.size() )
    {
        current_char = line[current_char_pointer];

        column = column_string.find(current_char) + 2;

        
        if (column < 3)
        {
            if (alph.find(std::toupper(current_char)) > 0)
            {
                column = 1;
            }
            else if (digits.find(current_char) > 0)
            {
                column = 2;
            }
            else
            {
                column = 12;
            }
        }

        // need to fix for this to actually work
        lex_state = state_table[lex_state][column]; // lex_staet = state_table[current_state][input]
        
        switch(lex_state)
        {
            case 3:
                token = line.substr(start_pointer, current_char_pointer - start_pointer);

                token_class = keywordSearch(token); // check for possible keyword

                // im guessing that if we find a keyword, then token_class will be either < 1 or > 1
                if (token_class < 1) token_class = 1;

                enter(token, token_class); // put in symbol table

                lex_state = 1; // go back to starting state
                
                current_char_pointer = current_char_pointer - 1;

                break;
            
            /* supposed to do the rest for the other states apparently 3 and so on*/

            default:
                std::cout << "default\n";
        }

        current_char_pointer += 1;

        // actual textbook line: if lex_state=1 then sp := cp; 
        if (lex_state == 1) start_pointer = current_char_pointer;
    }
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
