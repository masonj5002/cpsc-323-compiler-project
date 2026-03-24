#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "lexer.h"

// void lex_scan(const std::string& line)
// {
//     /* */
// }

// std::pair<std::string, std::string> lexer(const std::string& token)
// {

//     // 1. Pass it into the separator FSM

//     // 2. Pass it into the keyword FSM

//     // 3. Pass it  into the operator FSM

//     // 4. Pass it into the identifier FSM

//     // 5. Pass it into the real FSM

//     // 6. Pass it into the integer FSM

//     return {};
// }

// TODO: Make a class for the cursor
// TODO: Make a class for each FSM (or all of them in the same class)

int main() {

    printHello();

    // TODO: Finish state table
    // TODO: Move state table to separate class file
    std::vector<std::pair<std::string, std::vector<int>>> state_table = {
        {"l", {2, 2, 1}}, {"d", {1}}, {"{", {2}},        {"}", {0}},
        {"(", {0}},       {"*", {0}}, {")", {0}},        {":", {0}},
        {"=", {0}},       {"<", {0}}, {"sp", {1, 3, 1}},  // space
        {"p", {0}},                                       // punctuation
    };

    std::vector<char> separators = {'@', '(', ')', ',', '{', '}', ';'};
    std::vector<std::string> keywords = {
        "function", "integer", "boolean", "real",  "if",   "fi",   "otherwise",
        "return",   "write",   "read",    "while", "true", "false"};
    std::vector<std::string> operators = {
        "=", "+", "-", "*", "/", "==", "!=", ">", "<", "<=", "=>"};

    const std::string SOURCE_FILE = "source_code.txt";
    std::ifstream my_file(SOURCE_FILE);

    if (!my_file) {
        std::cerr << "Error opening test.txt\n";
        return 1;
    }

    // Read the file
    std::string line;
    while (std::getline(my_file, line)) {
        std::cout << line << "\n";
    }

    my_file.close();

    // OUTPUT

    // TODO: Output needs to be saved to an output text file. Each type of token
    // should be labeled but the prof is flexible on how the file is output

    return 0;
}
