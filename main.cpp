/*
    
    CPSC 323
    Assignment 1 : Lexical Analyzer
    Description: A lexer that divides up source code, written in Rat26S, into meaningful units and writes
                 these generated tokens into a desired output text file.

    Members: Mason Jennings, Gabriel Apodaca, Anthony Jimenez
    
    Submission Date: 3/1/2026
    Due Date: 3/1/2026
    Chosen Language: C++

*/

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "fsm.hpp"
#include "lexical_analyzer.hpp"
#include "syntax_analyzer.hpp"

using namespace lexical_analysis;
using namespace syntax_analysis;

int main(int argc, char* argv[])
{

    // Collect command line arguments
    std::vector<std::string> arguments{argv, argv + argc};
    if (arguments.size() != 3)
    {
        std::cout << "Error: usage: ./executable \"input_file_name.txt\" \"output_file_name.txt\"";
        return -1;
    }

    // Set up file names from list of command line arguments
    const std::string INPUT_FILE_NAME  = arguments[1];
    const std::string OUTPUT_FILE_NAME = arguments[2];
    

    // Create file input and output streams
    std::ifstream input_file_stream {INPUT_FILE_NAME };
    std::ofstream output_file_stream{OUTPUT_FILE_NAME};
    
    if (!input_file_stream.is_open() || !output_file_stream.is_open())
    {
        std::cout << "Error while creating file streams for " << INPUT_FILE_NAME << " and " << OUTPUT_FILE_NAME << '\n';
        return -1;
    }
    
    // // Header for the output file
    // output_file_stream << std::setw(20) << std::left << "token" << "lexeme\n"
    //                    << "---------------------------\n"; 



    // List that holds the results of the lexical analyzer
    std::vector<Record> records = lexical_analyzer(input_file_stream);
    
    // Write everything to the output file
    // for (const Record& record : records)
    // {
    //     std::cout << std::setw(20) << std::left << record.token << ' ' << record.lexeme << '\n';
    // }

    for (const Record& record : records)
    {
        if (record.token == "unknown")
        {
            std::cout << "Lexical Error: Unknown token classification on line " << record.line << " with a lexeme of: " << record.lexeme << '\n';
        }
    }
    
    Rat26SParser parser(records, INPUT_FILE_NAME, output_file_stream, true);
    parser.write_productions_to_file();

    // Close the file streams
    input_file_stream.close();
    output_file_stream.close();

    return 0;
}
