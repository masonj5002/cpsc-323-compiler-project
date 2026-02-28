// Note: This C++ program was compiled using g++ 13.2.0

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "fsm.hpp"

using namespace fsm;


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


int main(int argc, char* argv[])
{

    // Collect command line arguments
    std::vector<std::string> arguments{argv, argv + argc};
    if (arguments.size() != 3)
    {
        std::cout << "Error: usage: ./executable \"input_file_name.txt\" \"output_file_name.txt\"";
        return -1;
    }

    std::string input_file_name  = arguments[1];
    std::string output_file_name = arguments[2];
    

    // Create file input and output streams
    std::ifstream input_file_stream {input_file_name };
    std::ofstream output_file_stream{output_file_name};
    
    if (!input_file_stream.is_open() || !output_file_stream.is_open())
    {
        std::cout << "Error while creating file streams for " << input_file_name << " and " << output_file_name << '\n';
        return -1;
    }


    // Set up lexicon for lexical analzyer
    std::vector<std::string> separators = { "@", "(", ")", ",", "{", "}", ";"};
    std::vector<std::string> keywords   = { "function", "integer", "boolean", "real", "if", "fi", "otherwise", "return", "write", "read", "while", "true", "false"};
    std::vector<std::string> operators  = { "=", "+", "-", "*", "/", "==", "!=", ">", "<", "<=", "=>"};
    
    std::vector<std::string> compound_operators = {"==", "!=", "<=", "=>"};
    std::vector<std::string> special_operators  = {"=",  "!",  "<", ">"  };


    // Header for the output file
    output_file_stream << std::setw(20) << std::left << "token" << "lexeme\n"
                       << "---------------------------\n"; 


    
    // Begin reading the input file and writing to the output file.
    while (!input_file_stream.eof())
    {
        // Read a line from the input file
        std::string line;
        std::getline(input_file_stream, line, '\n');

        int current_char = 0;
        int count = 1;        // How many characters to read from line starting from current_char.

        // Begin processing the obtained line from the input file.
        while (current_char < line.size() && count <= line.size())
        {
            
            // Get current substring
            std::string current_substr = line.substr(current_char, count);

            // std::cout << '[' << current_char << ',' << count << "]: " << current_substr << '\n';

            // Skip comments 
            if (current_substr == "/" && count <= line.size())
            {
                std::string next_possible = line.substr(current_char, count + 1);
                if (next_possible == "/*")
                {
                    // std::cout << "HELLO\n";
                    while (next_possible != "*/")
                    {
                        if (current_char >= line.size() || count >= line.size())
                        {
                            
                            std::getline(input_file_stream, line, '\n');
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

            // std::cout << "RESULT: " << result.token << ' ' << result.lexeme << '\n';

            // Determine which possible token that is about to show up and construct it.


            if (result.token == "identifier")
            {
                if (count <= line.size())
                {
                    int i = 1;
                    std::string potential_identifier = line.substr(current_char, count + 1);
                    while ((lexer(potential_identifier).token == "identifier" || lexer(potential_identifier).token == "keyword") && count + i <= line.size())
                    {
                        i += 1;
                        potential_identifier = line.substr(current_char, count + i);
                    }
                    potential_identifier = line.substr(current_char, count + i - 1);

                    Record potential_identifier_record = lexer(potential_identifier);

                    result.token  = potential_identifier_record.token;
                    result.lexeme = potential_identifier_record.lexeme;

                    current_char += i - 1;
                }
                current_char += count;
                count = 1;
                output_file_stream << std::setw(20) << std::left << result.token << ' ' << result.lexeme << '\n';
                continue;
            }
            else if (result.token == "integer")
            {
                // take into accountability of reals. this can possibly be a real b/c the regular expression of real starts with an integer
                // if the next character is a '.'
                while (lexer(current_substr).token == "integer" && count <= line.size())
                {
                    count += 1;
                    current_substr = line.substr(current_char, count);
                }
                count -= 1; // Go back one to the last accepting input string for an integer
                current_substr = line.substr(current_char, count);

                // std::cout << "INTEGER PORTION: " << current_substr << '\n';

                // std::cout << "CURRENT: " << current_substr << '\n';
                bool real_number_found = false;
                if (count <= line.size())
                {
                    std::string next_substr = line.substr(current_char, count + 1);
                    // std::cout << "size - 1: " << next_substr[next_substr.size() - 1] << '\n';
                    // std::cout << "count: " << next_substr[count] << '\n';
                    
                    if (next_substr[next_substr.size() - 1] == '.')
                    {
                        std::string fraction_part_of_real = line.substr(current_char + count + 1, 1);
                        // std::cout << "FRACTION PART: " << fraction_part_of_real << '\n';
                        int i = 1;
                        while (i <= line.size() && lexer(fraction_part_of_real).token == "integer")
                        {
                            i += 1;
                            fraction_part_of_real = line.substr(current_char + count + 1, i);
                            // std::cout << "EACH ITERATION: " << fraction_part_of_real << '\n';
                        }
                        fraction_part_of_real = line.substr(current_char + count + 1, i - 1);
                        // std::cout << fraction_part_of_real << '\n';
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
                // std::cout << "COUNT IN INTEGER: " << count << '\n';
                current_char += count;

                count = 1;

                if (!real_number_found) result = lexer(current_substr);

                output_file_stream << std::setw(20) << std::left << result.token << ' ' << result.lexeme << '\n';
                continue;
            }
            else if (result.token == "separator")
            {
                output_file_stream << std::setw(20) << std::left << result.token << ' ' << result.lexeme << '\n';
                current_char += 1;
                count = 1;
                continue;
            }
            else if (result.token == "operator")
            {
                // take into acountability of compound operators
                // if the lexeme is any one of the special operators
                // which have the potential to become a compound operator,
                // then check if the next character added onto to the current substring
                // if it could match any one of the compound operators.
                // if yes, then this is a compound operator
                // if no, then this is a single operator
                // std::cout << "operator found " << result.lexeme << "\n";
                bool special_operator_found = false;

                for (const std::string& special_operator : special_operators)
                {
                    if (current_substr == special_operator)
                    {
                        special_operator_found = true;
                        break;
                    }
                }

                bool compound_operator_found = false;
                if (special_operator_found && count <= line.size()) // note: maybe add count <= line.size() here
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
                
                output_file_stream << std::setw(20) << std::left << result.token << ' ' << result.lexeme << '\n';
                
                if (!compound_operator_found) current_char += count;
                else                          current_char += count + 1;
                count = 1;
                continue;
            }
            
            count += 1;
        }
    }   


    // Close the file streams
    input_file_stream.close();
    output_file_stream.close();

    return 0;
}
