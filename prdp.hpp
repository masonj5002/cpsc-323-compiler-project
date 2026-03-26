/*
    
    CPSC 323
    Assignment 2 : Syntax Analyzer
    Description: 

    Members: Mason Jennings, Gabriel Apodaca, Anthony Jimenez
    
    Submission Date: 4/5/2026
    Due Date: 4/5/2026
    Chosen Language: C++

*/

#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "lexical_analyzer.hpp"

using namespace lexical_analysis;

// Note: There are like 30 functions in this file... bruh

// The naming convention is that the function's names are the nonterminals in
// the assignment documentation for the compiler project 

class Rat26SParser
{
 private:
  std::vector<Record> m_records;
  std::ofstream&      m_output_file_stream;
  bool                m_print_productions;
  int                 m_current_token_index;

 public:
    Rat26SParser(const std::vector<Record>& records, std::ofstream& output_file_stream, bool print_productions=true)
     : m_records(records),
       m_output_file_stream(output_file_stream),
       m_print_productions(print_productions),
       m_current_token_index(0)
     {}

    
    void parse_to_file()
    {
        Rat26S();
    }

    const std::string& get_next_token() 
    {
        ++m_current_token_index;

        if (m_current_token_index < m_records.size())
        {
            return m_records[m_current_token_index].token;
        }

        return ""; // <-- maybe throw an exception here or something idk
    }

    void output_current_token()
    {
        
    }

    bool Rat26S()
    {
        
        if (m_print_productions)
            m_output_file_stream << "<Rat26S> ::= @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @\n";

        Opt_Function_Definitions();

        Opt_Declaration_List();

        Statement_List();
        
    }
    
    bool Opt_Function_Definitions()
    {

        return false;
    }

    bool Function_Definitions()
    {

        return false;
    }

    bool Function()
    {

        return false;
    }

    bool Opt_Parameter_List()
    {
        
        return false;
    }

    bool Parameter_List()
    {

        return false;
    }

    bool Parameter()
    {

        return false;
    }

    bool Qualifier()
    {

        return false;
    }

    bool Body()
    {

        return false;
    }

    bool Opt_Declaration_List()
    {

        return false;
    }

    bool Declaration()
    {

        return false;
    }

    bool IDs()
    {

        return false;
    }

    bool Statement_List()
    {

        return false;
    }

    bool Statement()
    {

        return false;
    }

    bool Compound()
    {
        
        return false;
    }

    bool Assign()
    {

        return false;
    }

    bool If()
    {

        return false;
    }

    bool Return()
    {

        return false;
    }

    bool Print()
    {

        return false;
    }

    bool Scan()
    {

        return false;
    }

    bool While()
    {

        return false;
    }

    bool Condition()
    {
        
        return false;
    }

    bool Relop()
    {

        return false;
    }

    bool Expression()
    {

        return false;
    }

    bool Term()
    {

        return false;
    }

    bool Factor()
    {

        return false;
    }

    bool Primary()
    {

        return false;
    }

    bool Empty()
    {

        return false;
    }
    
};
