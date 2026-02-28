#pragma once

#include <string>
#include <vector>

namespace fsm
{   
    // Contains implementations for deterministic finite state machines that recognize
    // identifiers, integers, and real numbers.


    /**
     * @brief Accepts or rejects input strings if they match the identifier regular expression l(l|d|_)* 
     * 
     * @param w The input string to be processed.
     * @return A boolean indicating whether the string was accepted or rejected.
     */
    bool identifier_fsm(const std::string& w)
    {

        // Set up our next-state transition table and accepting states
        std::vector<std::vector<int>> state_table = {
            //     l  d  _
            /*0*/ {1, 5, 5},
            /*1*/ {2, 3, 4},
            /*2*/ {2, 3, 4},                // Note: For implementation purposes, the state label values of our DFSM
            /*3*/ {2, 3, 4},                //       were decremented by one in order to match the 0-indexing of C++.
            /*4*/ {2, 3, 4},
            /*5*/ {5, 5, 5}
        };

        std::vector<int> accepting_states = {1, 2, 3, 4};


        // Begin processing the input string.
        int current_state = 0; // Starting state is 0
        for (char c : w)
        {   
            int input = -1; 
            if      (std::tolower(c) >= 'a' && std::tolower(c) <= 'z') input = 0; // If the character is a letter --> column 1
            else if (c >= '0' && c <= '9'                            ) input = 1; // If the character is a digit  --> column 2
            else if (c == '_'                                        ) input = 2; // If the character is an _     --> column 3
            else return false; // return false if there is an unknown character
            
            current_state = state_table[current_state][input];
        }

        // Check if current state of our machine was left in one of the accepting states
        for (int i = 0; i < accepting_states.size(); ++i)
        {
            if (current_state == accepting_states[i]) return true;
        }

        return false;
    }


    /**
     * @brief Accepts or rejects input strings if they match the integer regular expression d+ 
     * 
     * @param w The input string to be processed.
     * @return A boolean indicating whether the string was accepted or rejected.
     */
    bool integer_fsm(const std::string& w)
    {
        
        // Set up our next-state transition table and accepting states
        std::vector<std::vector<int>> state_table = {
                  /*d*/
            /*0*/  {1},         // Note: For implementation purposes, the state label values of our DFSM
            /*1*/  {1},         //       were decremented by one in order to match the 0-indexing of C++.
        };

        int accepting_state = 1;
        

        // Begin processing the input string.
        int current_state = 0; // 0 is the starting state
        for (char c : w)
        {
            
            // There is only one column in the state table so we can just 
            // access the elements at index 0 of the 2D-vector state_table
            if (c >= '0' && c <= '9') current_state = state_table[current_state][0];
            else return false; // return false if there is an unknown character

        }

        // If the last state that the machine was in is an accepting state, then return true.
        if (current_state == accepting_state) return true;

        return false;
    }


    /**
     * @brief Accepts or rejects input strings if they match the real regular expression d+.d+ 
     * 
     * @param w The input string to be processed.
     * @return A boolean indicating whether the string was accepted or rejected.
     */
    bool real_fsm(const std::string& w)
    {

        // Set up our next-state transition table and accepting states
        std::vector<std::vector<int>> state_table = {
                // d  .
            /*0*/ {2, 1},
            /*1*/ {1, 1},       // Note: For implementation purposes, the state label values of our DFSM
            /*2*/ {2, 3},       //       were decremented by one in order to match the 0-indexing of C++.
            /*3*/ {4, 1},
            /*4*/ {4, 1}
        };

        int accepting_state = 4;


        // Begin processing the input string.
        int current_state = 0;
        for (char c : w)
        {
            int input = -1;
            if      (c >= '0' && c <= '9') input = 0;
            else if (c == '.'            ) input = 1;
            else return false; // return false if there is an unknown character

            current_state = state_table[current_state][input];
        }

        // If the last state that the machine was in is an accepting state, then return true.
        if (current_state == accepting_state) return true;

        return false;
    }

} // namespace fsm