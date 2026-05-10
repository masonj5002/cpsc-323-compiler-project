/*
    
    CPSC 323
    Assignment 3 : Syntax Analyzer with Symbol Table
    Description: A syntax analyzer that checks the grammar for a given Rat26S source file using
    a Predictive Recursive Descent Parser (PRDP). It then records identifiers in a symbol table.
    
    Members: Mason Jennings, Gabriel Apodaca, Anthony Jimenez
    
    Submission Date: 5/10/2026
    Due Date: 5/10/2026
    Chosen Language: C++

*/
#include <string>

struct SymbolTableEntry{
    SymbolTableEntry(std::string identifier, int memoryAddress, std::string type) :
        identifier_{identifier}, memoryAddress_{memoryAddress}, type_{type} {}

    std::string identifier_;
    int memoryAddress_;
    std::string type_;
};
