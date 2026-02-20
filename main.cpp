#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>

void lex_scan(const std::string& line)
{
    /* */
}



std::pair<std::string, std::string> lexer(const std::string& token)
{

    // 1. Pass it into the seperator FSM

    // 2. Pass it into the keyword FSM

    // 3. Pass it  into the operator FSM

    // 4. Pass it into the identifier FSM

    // 5. Pass it into the real FSM

    // 6. Pass it into the integer FSM

    return {};
}

int main() {

    std::vector<std::pair<int, int>> test = {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}};
    std::vector<std::pair<std::string, std::vector<int>>> state_table = 
    {
        { "l", {2, 2, 1}},
        { "d", {1}},
        { "{", {2}},
        { "}", {0}},
        { "(", {0}},
        { "*", {0}},
        { ")", {0}},
        { ":", {0}},
        { "=", {0}},
        { "<", {0}},
        { "sp", {1, 3, 1}}, // space
        { "p", {0}},        // punctuation
    };

    // std::map<std::string, std::vector<int>> state_table =
    // {
    //   { , }  
    // };

    // for (const auto& p : state_table) std::cout << p.first

    // while (std::cin >> variable)
    // std::vector<std::vector<int>> state_table;

    // for (const auto& p : state_table)
    // {
    //     std::cout << '{' << p.first << ',' << p.second << "}\n";
    // }

    std::vector<char>        separators = { '@', '(', ')', ',', '{', '}', ';'};
    std::vector<std::string> keywords   = { "function", "integer", "boolean", "real", "if", "fi", "otherwise", "return", "write", "read", "while", "true", "false"};
    std::vector<std::string> operators  = {"=", "+", "-", "*", "/", "==", "!=", ">", "<", "<=", "=>"};
    
    std::string line;

    std::ifstream my_file("test.txt");

    if (!my_file.is_open())
    {
        std::cerr << "Error opening test.txt\n";
        return -1;
    }

    // prints text file
    while (!my_file.eof())
    {
        std::getline(my_file, line, '\n');
        // std::cout << line << '\n';
        // for each character on each line, determine what the character is
        for (char c : line) {
            // std::cout << c;
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                //std::cout << "this is a letter\n";
                std::cout << state_table[0].second[0] << std::endl;
            }
        }
        std::cout << "\n";
    }
    my_file.close();

    // if (my_file.is_open())
    // {
    //     while ()
    //     {
    //         std::cout << line << '\n';
    //     }
    //     my_file.close();
    // }

    return 0;
}
