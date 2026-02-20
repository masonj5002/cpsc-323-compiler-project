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

    std::vector<std::pair<std::string, int>> state_table =
    {
        { "l", {} },
        { "d", 1},
        { "{", 2},
        { "}", 0},
        { "(", 0},
        { "*", 0},
        { ")", 0},
        { ":", 0},
        { "=", 0},
        { "<"}
    };

    std::map<std::string, std::vector<int>> state_table =
    {
      { , }  
    };

    for (const auto& p : state_table) std::cout << p.first

    while (std::cin >> variable)
    std::vector<std::vector<int>> state_table;

    for (const auto& p : state_table)
    {
        std::cout << '{' << p.first << ',' << p.second << "}\n";
    }

    std::vector<char>        seperators = { '@', '(', ')', ',', '{', '}', ';'};
    std::vector<std::string> keywords   = { "function", "integer", "boolean", "real", "if", "fi", "otherwise", "return", "write", "read", "while", "true", "false"};
    std::vector<std::string> operators  = {"=", "+", "-", "*", "/", "==", "!=", ">", "<", "<=", "=>"};
    
    std::string line;

    std::ifstream my_file("test.txt");

    if (!my_file.is_open())
    {
        std::cerr << "Error opening test.txt\n";
        return -1;
    }

    
    while (!my_file.eof())
    {
        std::getline(my_file, line, '\n');
        std::cout << line << '\n';
    }

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
