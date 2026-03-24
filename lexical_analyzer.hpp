#pragma once

#include <string>
#include <vector>

#include "fsm.hpp"

using namespace fsm;

namespace lexical_analysis
{
    // Contains definitions for the Record object and functions that are support
    // the implementation for the lexical analyzer

    /**
     * @brief Holds a token and a lexeme.
     * 
     * This structure is used to group related tokenization data. 
     * 
     */
    struct Record
    {
    Record() : token{""}, lexeme{""} {}
    Record(const std::string& token, const std::string& lexeme) : token{token}, lexeme{lexeme} {}

    std::string token;
    std::string lexeme;
    };

    
    /**
     * @brief Determines the token classification of a given input string.
     * 
     * @param input_string The input string to be processed and read.
     * 
     * @return A Record which holds the token that input_string belongs to, and the lexeme associated with the token.
     * 
     */
    Record lexer(const std::string& input_string)
    {
        Record result("unknown", input_string);

        std::vector<std::string> separators = { "@", "(", ")", ",", "{", "}", ";"};
        std::vector<std::string> keywords   = { "function", "integer", "boolean", "real", "if", "fi", "otherwise", "return", "write", "read", "while", "true", "false"};
        std::vector<std::string> operators  = { "=", "+", "-", "*", "/", "==", "!=", ">", "<", "<=", "=>"};
        
        std::vector<std::string> compound_operators = {"==", "!=", "<=", "=>"};
        std::vector<std::string> special_operators  = {"=",  "!",  "<", ">"};


        // 1. Check if the input_string is a separator
        for (const std::string& separator : separators)
        {
            if (input_string == separator)
            {
                result.token = "separator";
                return result;
            }
        }

        // 2. Check if the input_string is an operator
        for (const std::string& oper : operators)
        {
            if (input_string == oper)
            {
                result.token = "operator";
                return result;
            }
        }

        // 3. Check if the input_string is accepted by the identifier finite state machine.
        if (identifier_fsm(input_string))
        {
            // Check if this is actually just a keyword
            for (const std::string& keyword : keywords)
            {
                if (input_string == keyword)
                {
                    result.token = "keyword";
                    return result;
                }
            }

            result.token = "identifier";
            return result;
        }

        // 4. Check if the input_string is accepted by the integer finite state machine
        if (integer_fsm(input_string))
        {
            result.token = "integer";
            return result;
        }

        // 5. Check if the input_string is accepted by the real finite state machine
        if (real_fsm(input_string))
        {
            result.token = "real";
            return result;
        }

        return result;
    };
}
