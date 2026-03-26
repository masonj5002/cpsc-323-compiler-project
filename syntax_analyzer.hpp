#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#include "lexical_analyzer.hpp"
// #include "prdp.hpp"

using namespace lexical_analysis;
// using namespace Rat26SGrammar;

namespace syntax_analysis
{
    
    // records is a list of Record objects that hold tokens and corresponding lexemes
    void parse(const std::vector<Record>& records, std::ofstream& output_file_stream)
    {

        /*
        
        if starting symbol is correct and the last token is an end statement
            print yay we did it, everything is correct
        else
            print an error i think idk
        
        */
        // if (Rat26S(records))
        // {

        // }
    
    }

}
