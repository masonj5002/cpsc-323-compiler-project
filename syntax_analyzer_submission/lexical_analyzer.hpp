/*
    
    CPSC 323
    Assignment 2 : Syntax Analyzer
    Description: A syntax analyzer that checks the grammar for a given Rat26S source file using
    a Predictive Recursive Descent Parser (PRDP).
    
    Members: Mason Jennings, Gabriel Apodaca, Anthony Jimenez
    
    Submission Date: 4/5/2026
    Due Date: 4/5/2026
    Chosen Language: C++

*/

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
    Record() : token{""}, lexeme{""}, line{0} {}
    Record(const std::string& token, const std::string& lexeme, const unsigned long int & line) : token{token}, lexeme{lexeme}, line{line} {}

    std::string token;
    std::string lexeme;
    unsigned long int line;
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
        Record result("unknown", input_string, 0);

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
    
    std::vector<Record> lexical_analyzer(std::ifstream& input_file_stream)
    {

        std::vector<std::string> compound_operators = {"==", "!=", "<=", "=>"};
        std::vector<std::string> special_operators  = {"=",  "!",  "<", ">"  }; // Operators that could potentially be the first character of a compound operator

        std::vector<Record> records;
        unsigned long int current_line_number = 0;

        while (!input_file_stream.eof())
        {
            // Read a line from the input file
            std::string line;
            std::getline(input_file_stream, line, '\n');
            ++current_line_number;

            int current_char = 0;
            int count = 1;        // How many characters to read from line starting from current_char.

            // Begin processing the obtained line from the input file.
            while (current_char < line.size() && count <= line.size())
            {
                // Get current substring
                std::string current_substr = line.substr(current_char, count);

                // Skip comments 
                if (current_substr == "/" && count <= line.size())
                {
                    std::string next_possible = line.substr(current_char, count + 1);
                    if (next_possible == "/*")
                    {
                        // Keep reading pairs of characters until we find */
                        while (next_possible != "*/")
                        {
                            // If run out of characters and we did not find a closing comments symbol */,
                            // then we must move to the next line begin skipping characters again.
                            if (current_char >= line.size() || count >= line.size())
                            {
                                std::getline(input_file_stream, line, '\n');
                                ++current_line_number;
                                current_char = 0;
                                count = 2;

                                if (count <= line.size())
                                {
                                    next_possible = line.substr(current_char, count);
                                }

                                continue; // Skip an iteration because we still need to check the new string we have on the next line
                            }

                            // Read the next pair of characters
                            current_char += 1;
                            count = 2;
                            next_possible = line.substr(current_char, count);
                        }

                        // Move the current character pointer to the beginning of the next possible token
                        // and set reading count back to 1 character. 
                        current_char += count;
                        count = 1;
                        continue;
                    }
                }


                // Skip spaces
                if (current_substr == " ")
                {
                    current_char += 1;
                    continue;
                }
                

                // Send the current substring to the lexer 
                Record result = lexer(current_substr);

                if (result.token == "identifier")
                {
                    // Here we check what other characters are possibly part of the identifer
                    // or if this "identifier" is actually just a keyword
                    if (count <= line.size())
                    {
                        std::string potential_identifier = line.substr(current_char, count + 1);
                        
                        // Keep reading characters until the lexer cannot recognize an identifier or keyword
                        int i = 1;
                        while ((lexer(potential_identifier).token == "identifier" || lexer(potential_identifier).token == "keyword") && count + i <= line.size())
                        {
                            i += 1;
                            potential_identifier = line.substr(current_char, count + i);
                        }
                        potential_identifier = line.substr(current_char, count + i - 1); // Get last string that was accepted

                        
                        Record potential_identifier_record = lexer(potential_identifier);
                        result.token  = potential_identifier_record.token;
                        result.lexeme = potential_identifier_record.lexeme;

                        current_char += i - 1;
                    }

                    // Add the final result to the working list of tokens and lexemes
                    result.line = current_line_number;
                    records.push_back(result);

                    // Move the current_char to the next potential token and set reading count back to 1 
                    current_char += count;
                    count = 1;
                    continue;
                }
                else if (result.token == "integer")
                {
                    // Keep reading characters just in case there are more digits in the 
                    // integer portion of our number
                    while (lexer(current_substr).token == "integer" && count <= line.size())
                    {
                        count += 1;
                        current_substr = line.substr(current_char, count);
                    }
                    count -= 1; // Go back one to the last accepting input string for an integer
                    current_substr = line.substr(current_char, count);

                    // Check if this number is a real
                    bool real_number_found = false;
                    if (count <= line.size())
                    {
                        std::string next_substr = line.substr(current_char, count + 1);

                        // If there is a decimal '.', then this must be a real and we will
                        // keep checking after the decimal for digits in the fractional portion
                        // of our real number
                        if (next_substr[next_substr.size() - 1] == '.')
                        {
                            std::string fraction_part_of_real = line.substr(current_char + count + 1, 1);

                            // Keep reading characters after the '.' to obtain the digits in the 
                            // fractional portion of our real number
                            int i = 1;
                            while (i <= line.size() && lexer(fraction_part_of_real).token == "integer")
                            {
                                i += 1;
                                fraction_part_of_real = line.substr(current_char + count + 1, i);
                            }
                            fraction_part_of_real = line.substr(current_char + count + 1, i - 1); // Get last string that was accepted

                            // See if the integer portion, the character after the integer portion (which should be a '.'), and
                            // the fraction portion make up a real. If not, then this is just an integer.
                            Record potential_real_record = lexer(next_substr + fraction_part_of_real);
                            if (potential_real_record.token == "real")
                            {
                                result.token  = potential_real_record.token;
                                result.lexeme = potential_real_record.lexeme;
                                current_char += i;
                                real_number_found = true;
                            }
                        }
                    }

                    // Move the current_char to the next potential token and set reading count back to 1 
                    current_char += count;
                    count = 1;

                    // If no real number was found, then current_substr contains the constructed integer
                    // portion of what would be an a real number. We call the lexer to get an updated
                    // version of the integer
                    if (!real_number_found) result = lexer(current_substr);

                    // Add the final result to the working list of tokens and lexemes
                    result.line = current_line_number;
                    records.push_back(result);

                    continue;
                }
                else if (result.token == "separator")
                {
                    // Add the final result to the working list of tokens and lexemes
                    result.line = current_line_number;
                    records.push_back(result);

                    // Move the character pointer to the next character, set reading count to 1, and skip the
                    // increment statement at the bottom of the while loop so we do not read excess characters.
                    current_char += 1;
                    count = 1;
                    continue;
                }
                else if (result.token == "operator")
                {
                    
                    // Check if this operator is the first character of any one of the compound operators
                    bool special_operator_found = false;
                    for (const std::string& special_operator : special_operators)
                    {
                        if (current_substr == special_operator)
                        {
                            special_operator_found = true;
                            break;
                        }
                    }

                    
                    // If we found the starting character of a compound operator, we need to read one character extra
                    // and check whether the extra character makes this a compound operator or not.
                    bool compound_operator_found = false;
                    if (special_operator_found && count <= line.size())
                    {
                        // Find out if adding the next character onto the current substring
                        // will be a compound operator
                        std::string next_substr = line.substr(current_char, count + 1);
                        for (const std::string& compound_operator : compound_operators)
                        {
                            if (next_substr == compound_operator)
                            {
                                result.lexeme = next_substr;
                                compound_operator_found = true;
                                break;
                            }
                        }
                    }
                    
                    // Add the final result to the working list of tokens and lexemes
                    result.line = current_line_number;
                    records.push_back(result);
                    
                    // Move the current_char to the next potential token and set reading count back to 1 
                    if (!compound_operator_found) current_char += count;
                    else                          current_char += count + 1;
                    count = 1;
                    continue;
                } 
                else if (result.token == "unknown" && std::find(special_operators.begin(), special_operators.end(), result.lexeme) == special_operators.end())
                {
                    result.line = current_line_number;
                    records.push_back(result);
                    current_char += count;
                    continue;
                }

                count += 1;
            }
        } 

        // Add eof marker
        ++current_line_number;
        records.push_back(Record("eof", "$", current_line_number));
        
        return records;
    }
}
