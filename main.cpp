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

#ifdef _WIN32
#include <windows.h>
#endif

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "fsm.hpp"
#include "lexical_analyzer.hpp"
#include "prdp_with_symbol_table.hpp"

using namespace lexical_analysis;

int main(int argc, char* argv[])
{

    // Switches Window Command Prompt to UTF-8 in order to display epsilon correctly.
    // This is only for Windows and should not be an issue for Linux.
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif

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
    

    // List that holds the results of the lexical analyzer
    std::vector<Record> records = lexical_analyzer(input_file_stream);

    // Check if something in the source code was unable to be identified
    for (const Record& record : records)
    {
        if (record.token == "unknown")
        {
            std::cout          << "Rat26S Compiler Lexical Error: Unknown token classification in " << INPUT_FILE_NAME << " on line " << record.line << " with a lexeme of: " << record.lexeme << '\n';
            output_file_stream << "Rat26S Compiler Lexical Error: Unknown token classification in " << INPUT_FILE_NAME << " on line " << record.line << " with a lexeme of: " << record.lexeme << '\n';
        }
    }
    output_file_stream << '\n';

    // Begin syntax analysis
    Rat26SParser parser(records, INPUT_FILE_NAME, output_file_stream, true);
    parser.write_productions_to_file();

    // Close the file streams
    input_file_stream.close();
    output_file_stream.close();

    return 0;
}
