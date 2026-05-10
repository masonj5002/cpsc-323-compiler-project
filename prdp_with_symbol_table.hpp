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

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <unordered_set>
#include <stdexcept>

#include "lexical_analyzer.hpp"
#include "symbol_table_entry.cpp"

using namespace lexical_analysis;

/**
 * @class Rat26SParser
 * 
 * @brief Object which encapsulates the data and behavior required for a Predictive Recursive Descent Parser (PRDP)
 * 
 */
class Rat26SParser
{
 public:
    
    // Parameterized Constructor
    Rat26SParser(const std::vector<Record>& records, const std::string& input_file_name, std::ofstream& output_file_stream, bool print_productions=true)
     : m_records(records),
       m_output_file_stream(output_file_stream),
       m_print_productions(print_productions),
       m_current_token_index(0),
       m_current_production(""),
       m_input_file_name(input_file_name),
       current_memory_address(1000) // starting address value (Assignment 3)
       {}
    
    void write_productions_to_file()
    {
        Rat26S();
    }

 private:

    // ----------------------------------
    // State of the Parser
    // ----------------------------------

    std::vector<Record> m_records;
    std::ofstream&      m_output_file_stream;
    bool                m_print_productions;
    int                 m_current_token_index;
    std::string         m_current_production;
    std::string         m_input_file_name;
    int current_memory_address; // Assignment 3
    std::vector<SymbolTableEntry> symbol_table; // Assignment 3
    

    // ----------------------------------
    // Helper functions
    // ----------------------------------

    // Safely get current record with bounds checking
    const Record& get_current_record() const 
    { 
        if (m_current_token_index >= static_cast<int>(m_records.size()))
        {
            throw std::out_of_range("Parser error: Token index out of bounds");
        }
        return m_records[m_current_token_index]; 
    }

    // Writes the current token and lexeme to the output file
    void output_current_token()
    {
        m_output_file_stream << "\nToken: " << std::setw(20) << std::left << get_current_record().token << "Lexeme: " << get_current_record().lexeme << '\n';
        
        // Assignment 3
        if (get_current_record().token == "identifier") {
            // create entry and add to table
            // 
            // must check to make sure entry exists
            // cannot add an undeclared identifier
            // cannot add an identifier that's been declared twice
            symbol_table.push_back(SymbolTableEntry(get_current_record().lexeme, use_address(), ""/*type goes here*/)); 
        }
    }

    // Assignment 3
    // returns the current memory address and increments it by one
    int use_address() {
        current_memory_address++;
        return current_memory_address - 1;
    }

    // Moves to the next token in the list of tokens and lexemes (list of Record objects)
    void lexer()
    {
        ++m_current_token_index;
    }
    
    // Writes the given production to the output file if writing is enabled for productions. 
    // Also sets the current production to the passed production string.
    void write_production(const std::string& production)
    {
        if (m_print_productions)
        {
            m_output_file_stream << production;
        }
        m_current_production = production;
    }
    
    // Writes to the output file and console output a formatted syntax error message that includes the token, lexeme,
    // expected message, and the current production that was violated.
    void output_error(const std::string& expected_message)
    {
        m_output_file_stream << "\nRat26S Syntax Error in " << m_input_file_name << " on line " << get_current_record().line << " with\n"
                             << "\tToken       : " << get_current_record().token  << '\n'
                             << "\tLexeme      : " << get_current_record().lexeme << '\n'
                             << "\tExpected    : " << expected_message            << '\n'
                             << "\tViolation of: " << m_current_production        << '\n';
        
        std::cout << "\nRat26S Syntax Error in " << m_input_file_name << " on line " << get_current_record().line << " with\n"
                  << "\tToken       : " << get_current_record().token  << '\n'
                  << "\tLexeme      : " << get_current_record().lexeme << '\n'
                  << "\tExpected    : " << expected_message            << '\n'
                  << "\tViolation of: " << m_current_production        << '\n';
    }

    // Helper function to check if current token is in a set
    bool is_in_first_set(const std::unordered_set<std::string>& first_set) const
    {
        return first_set.count(get_current_record().lexeme) || first_set.count(get_current_record().token);
    }

    
    // ----------------------------------
    // Functions that simulate productions
    // ----------------------------------

    // Simulates <Rat26S> ::= @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @
    void Rat26S()
    {
        write_production("<Rat26S> -> @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @\n");
        
        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        lexer();

        Opt_Function_Definitions();

        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        lexer();

        Opt_Declaration_List();

        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        lexer();

        Statement_List();

        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        lexer();

        // Check for proper end of file
        if (get_current_record().token != "eof")
        {
            output_error("end of file");
        }

        // Assignment 3 test:
        std::cout << "Symbol Table\n";
        std::cout << "Identifier    MemoryLocation    Type\n";
        for (const SymbolTableEntry& entry : symbol_table ) {
            std::cout << entry.identifier_ << "    " << entry.memoryAddress_ << "    " << entry.type_ << "\n";
        }
    }
    
    // Simulates <Opt Function Definitions> ::= <Function Definitions> | <Empty>
    void Opt_Function_Definitions()
    {
        if (get_current_record().lexeme == "function")
        {
            write_production("<Opt Function Definitions> -> <Function Definitions>\n");

            output_current_token();

            Function_Definitions();
        }
        else Empty("<Opt Function Definitions>");
    }

    // Simulates <Function Definitions> ::= <Function> <Function Definitions Prime> 
    void Function_Definitions()
    {
        write_production("<Function Definitions> -> <Function> <Function Definitions Prime>\n");
        
        Function();
        Function_Definitions_Prime();
    }

    // Simulates <Function Definitions Prime> ::= <Function Definitions> | <Empty>
    void Function_Definitions_Prime()
    {
        if (get_current_record().lexeme == "function")
        {
            write_production("<Function Definitions Prime> -> <Function Definitions>\n");
         
            output_current_token();
         
            Function_Definitions();
        }
        else Empty("<Function Definitions Prime>");
    }

    // Simulates <Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
    void Function()
    {
        write_production("<Function> -> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n");
        
        if (get_current_record().lexeme != "function")
        {
            output_error("function");
            return;
        }

        lexer();
        
        if (get_current_record().token == "identifier")
            output_current_token();

        Identifier();
        
        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        lexer();

        Opt_Parameter_List();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        lexer();

        Opt_Declaration_List();
        Body();
    }

    // Simulates <Opt Parameter List> ::= <Parameter List> | <Empty>
    void Opt_Parameter_List()
    {
        if (get_current_record().token == "identifier")
        {
            write_production("<Opt Parameter List> -> <Parameter List>\n");

            Parameter_List();
        }
        else Empty("<Opt Parameter List>");
    }

    // Simulates <Parameter List> ::= <Parameter> <Parameter List Prime>
    void Parameter_List()
    {
        write_production("<Parameter List> -> <Parameter> <Parameter List Prime>\n");
        
        Parameter();
        Parameter_List_Prime();
    }

    // Simulates <Parameter List Prime> ::= , <Parameter List> | <Empty> 
    void Parameter_List_Prime()
    {
        if (get_current_record().lexeme == ",")
        {
            write_production("<Parameter List Prime> -> , <Parameter List>\n");

            output_current_token();
            lexer();

            Parameter_List();
        }
        else Empty("<Parameter List Prime>");
    }

    // Simulates <Parameter> ::= <IDs > <Qualifier>
    void Parameter()
    {
        write_production("<Parameter> -> <IDs> <Qualifier>\n");

        IDs();
        Qualifier();
    }

    // Simulates <Qualifier> ::= integer | boolean | real
    void Qualifier()
    {
        static const std::unordered_set<std::string> qualifiers = {"integer", "boolean", "real"};
        
        if (!qualifiers.count(get_current_record().lexeme))
        {
            output_error("integer, boolean, or real");
            return;
        }

        write_production("<Qualifier> -> " + get_current_record().lexeme + '\n');

        output_current_token();
        lexer();
    }

    // Simulates <Body> ::= { < Statement List> }
    void Body()
    {
        write_production("<Body> -> { < Statement List> }\n");
        
        if (get_current_record().lexeme != "{")
        {
            output_error("{");
            return;
        }

        output_current_token();
        lexer();

        Statement_List();

        if (get_current_record().lexeme != "}")
        {
            output_error("}");
            return;
        }

        output_current_token();
        lexer();
    }

    // Simulates <Opt Declaration List> ::= <Declaration List> | <Empty>
    void Opt_Declaration_List()
    {
        static const std::unordered_set<std::string> first_set = {"integer", "boolean", "real"};
        
        if (is_in_first_set(first_set))
        {
            write_production("<Opt Declaration List> -> <Declaration List>\n");
            
            Declaration_List();
        }
        else Empty("<Opt Declaration List>");
    }

    // Simulates <Declaration List> ::= <Declaration> ; <Declaration List Prime>
    void Declaration_List()
    {
        write_production("<Declaration List> -> <Declaration> ; <Declaration List Prime>\n");
            
        Declaration();

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
            return;
        }

        output_current_token();
        lexer();

        Declaration_List_Prime();
    }

    // Simulates <Declaration List Prime> ::= <Declaration List> | <Empty> 
    void Declaration_List_Prime()
    {
        static const std::unordered_set<std::string> first_set = {"integer", "boolean", "real"};
        
        if (is_in_first_set(first_set))
        {
            write_production("<Declaration List Prime> -> <Declaration List>\n");

            Declaration_List();
        }
        else Empty("<Declaration List Prime>");
    }

    // Simulates <Declaration> ::= <Qualifier > <IDs>
    void Declaration()
    {
        write_production("<Declaration> -> <Qualifier> <IDs>\n");

        Qualifier();
        IDs();
    }

    // Simulates <IDs> ::= <Identifier> <IDs Prime>
    void IDs()
    {
        write_production("<IDs> -> <Identifier> <IDs Prime>\n");
        
        if (get_current_record().token == "identifier")
            output_current_token();
        
        Identifier();
        IDs_Prime();
    }

    // Simulates <IDs Prime> ::= , <IDs> | <Empty>
    void IDs_Prime()
    {
        if (get_current_record().lexeme == ",")
        {
            write_production("<IDs Prime> -> , <IDs>\n");

            output_current_token();
            lexer();
            
            IDs();
        }
        else Empty("<IDs Prime>");
    }

    // Simulates <Statement List> ::= <Statement> <Statement List Prime>
    void Statement_List()
    {
        write_production("<Statement List> -> <Statement> <Statement List Prime>\n");
        
        Statement();
        Statement_List_Prime();
    }

    // Simulates <Statement List Prime> ::= <Statement List> | <Empty>
    void Statement_List_Prime()
    {
        static const std::unordered_set<std::string> first_set = {"{", "identifier", "if", "return", "write", "read", "while"};

        if (is_in_first_set(first_set))
        {
            write_production("<Statement List Prime> -> <Statement List>\n");

            Statement_List();
        }
        else Empty("<Statement List Prime>");
    }

    // Simulates <Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>
    void Statement()
    {
        // Determine which type of statement based on first token
        if (get_current_record().lexeme == "{")
        {
            output_current_token();
            write_production("<Statement> -> <Compound>\n");
            Compound();
        }
        else if (get_current_record().token == "identifier")
        {
            output_current_token();
            write_production("<Statement> -> <Assign>\n");
            Assign();
        }
        else if (get_current_record().lexeme == "if")
        {
            output_current_token();
            write_production("<Statement> -> <If>\n");
            If();
        }
        else if (get_current_record().lexeme == "return")
        {
            output_current_token();
            write_production("<Statement> -> <Return>\n");
            Return();
        }
        else if (get_current_record().lexeme == "write")
        {
            output_current_token();
            write_production("<Statement> -> <Print>\n");
            Print();
        }
        else if (get_current_record().lexeme == "read")
        {
            output_current_token();
            write_production("<Statement> -> <Scan>\n");
            Scan();
        }
        else if (get_current_record().lexeme == "while")
        {
            output_current_token();
            write_production("<Statement> -> <While>\n");
            While();
        }
        else
        {
            output_error("statement that begins with {, an identifier, if, return, write, read, or while");
        }
    }

    // Simulates <Compound> ::= { <Statement List> }
    void Compound()
    {
        write_production("<Compound> -> { <Statement List> }\n");

        if (get_current_record().lexeme != "{")
        {
            output_error("{");
            return;
        }

        output_current_token();
        lexer();

        Statement_List();

        if (get_current_record().lexeme != "}")
        {
            output_error("}");
            return;
        }

        output_current_token();
        lexer();
    }

    // Simulates <Assign> ::= <Identifier> = <Expression> ;
    void Assign()
    {
        write_production("<Assign> -> <Identifier> = <Expression> ;\n");

        Identifier();

        if (get_current_record().lexeme != "=")
        {
            output_error("=");
            return;
        }

        output_current_token();
        lexer();
        
        Expression();

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
        }

        output_current_token(); // <------------- DOUBLE CHECK
        lexer();
    }   

    // Simulates <If> ::= if ( <Condition> ) <Statement> <If Prime>
    void If()
    {
        write_production("<If> -> if ( <Condition> ) <Statement> <If Prime>\n");
        
        if (get_current_record().lexeme != "if")
        {
            output_error("if");
            return;
        }

        lexer();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        lexer();

        Condition();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        lexer();

        Statement();
        If_Prime();
    }

    // Simulates <If Prime> ::= fi | otherwise <Statement> fi
    void If_Prime()
    {
        if (get_current_record().lexeme == "fi")
        {
            write_production("<If Prime> -> fi\n");

            output_current_token();
            lexer();
        }
        else if (get_current_record().lexeme == "otherwise")
        {
            write_production("<If Prime> -> otherwise <Statement> fi\n");

            output_current_token();
            lexer();

            Statement();

            if (get_current_record().lexeme != "fi")
            {
                output_error("fi");
                return;
            }

            output_current_token();
            lexer();
        }
        else
        {
            output_error("fi or otherwise");
        }
    }

    // Simulates <Return> ::= return <Return Prime>
    void Return()
    {
        write_production("<Return> -> return <Return Prime>\n");

        if (get_current_record().lexeme != "return")
        {
            output_error("return");
            return;
        }

        lexer();
        Return_Prime();
    }

    // Simulates <Return Prime> ::= ; | <Expression> ;
    void Return_Prime()
    {
        static const std::unordered_set<std::string> first_set_of_expression = {"-", "identifier", "integer", "(", "real", "true", "false"};

        if (get_current_record().lexeme == ";")
        {
            write_production("<Return Prime> -> ;\n");
            
            output_current_token();
            lexer();
        }
        else if (is_in_first_set(first_set_of_expression))
        {
            write_production("<Return Prime> -> <Expression> ;\n");

            Expression();

            if (get_current_record().lexeme != ";")
            {
                output_error(";");
                return;
            }

            output_current_token();
            lexer();
        }
        else
        {
            output_error("; or expression that starts with -, identifier, integer, (, real, true, or false");
        }
    }

    // Simulates <Print> ::= write ( <Expression>) ;
    void Print()
    {
        write_production("<Print> -> write ( <Expression> ) ;\n");
        
        if (get_current_record().lexeme != "write")
        {
            output_error("write");
            return;
        }

        lexer();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        lexer();

        Expression();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        lexer();

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
            return;
        }

        output_current_token();
        lexer();
    }

    // Simulates <Scan> ::= read ( <IDs> ) ;
    void Scan()
    {
        write_production("<Scan> -> read ( <IDs> );\n");
        
        if (get_current_record().lexeme != "read")
        {
            output_error("read");
            return;
        }

        lexer();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        lexer();

        IDs();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        lexer();

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
            return;
        }

        output_current_token();
        lexer();
    }

    // Simulates <While> ::= while ( <Condition> ) <Statement>
    void While()
    {
        write_production("<While> -> while ( <Condition> ) <Statement>\n");
        
        if (get_current_record().lexeme != "while")
        {
            output_error("while");
            return;
        }

        output_current_token();
        lexer();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        lexer();
        
        Condition();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        lexer();

        Statement();
    }

    // Simulates <Condition> ::= <Expression> <Relop> <Expression>
    void Condition()
    {
        write_production("<Condition> -> <Expression> <Relop> <Expression>\n");
        
        Expression();
        Relop();
        Expression();
    }

    // Simulates <Relop> ::= == | != | > | < | <= | =>
    void Relop()
    {
        static const std::unordered_set<std::string> operators = {"==", "!=", ">", "<", "<=", "=>"};
        
        if (!operators.count(get_current_record().lexeme))
        {
            output_error("==, !=, >, <, <=, or =>");
            return;
        }

        write_production("<Relop> -> " + get_current_record().lexeme + '\n');

        output_current_token();
        lexer();
    }

    // Simulates <Expression> ::= <Term> <Expression Prime>
    void Expression()
    {
        write_production("<Expression> -> <Term> <Expression Prime>\n");

        Term();
        Expression_Prime();
    }

    // Simulates <Expression Prime> ::= + <Term> <Expression Prime> | - <Term> <Expression Prime> | <Empty>
    void Expression_Prime()
    {
        if (get_current_record().lexeme == "+")
        {
            write_production("<Expression Prime> -> + <Term> <Expression Prime>\n");
            
            output_current_token();
            lexer();

            Term();
            Expression_Prime();
        }
        else if (get_current_record().lexeme == "-")
        {
            write_production("<Expression Prime> -> - <Term> <Expression Prime>\n");

            output_current_token();
            lexer();

            Term();
            Expression_Prime();
        }
        else Empty("<Expression Prime>");
    }

    // Simulates <Term> ::= <Factor> <Term Prime>
    void Term()
    {
        write_production("<Term> -> <Factor> <Term Prime>\n");
        
        Factor();
        Term_Prime();
    }

    // Simulates <Term Prime> ::= * <Factor> <Term Prime> | / <Factor> <Term Prime> | <Empty>
    void Term_Prime()
    {
        if (get_current_record().lexeme == "*")
        {
            write_production("<Term Prime> -> * <Factor> <Term Prime>\n");

            output_current_token();
            lexer();
            
            Factor();
            Term_Prime();
        }
        else if (get_current_record().lexeme == "/")
        {
            write_production("<Term Prime> -> / <Factor> <Term Prime>\n");
            output_current_token();
            lexer();
            
            Factor();
            Term_Prime();
        }
        else Empty("<Term Prime>");
    }

    // Simulates <Factor> ::= - <Primary> | <Primary>
    void Factor()
    {
        if (get_current_record().lexeme == "-")
        {
            write_production("<Factor> -> - <Primary>\n");

            output_current_token();
            lexer();
            Primary();
        }
        else
        {
            write_production("<Factor> -> <Primary>\n");

            Primary();
        }
    }

    // Simulates <Primary> ::= <Identifier> <Primary Prime> | <Integer> | ( <Expression> ) | <Real> | true | false
    void Primary()
    {
        if (get_current_record().token == "identifier")
        {
            write_production("<Primary> -> <Identifier> <Primary Prime>\n");
            
            output_current_token();
            
            Identifier();
            Primary_Prime();
        }
        else if (get_current_record().token == "integer")
        {
            write_production("<Primary> -> <Integer>\n");
            output_current_token();
            Integer();
        }
        else if (get_current_record().lexeme == "(")
        {
            write_production("<Primary> -> ( <Expression> )\n");

            output_current_token();
            lexer();

            Expression();

            if (get_current_record().lexeme != ")")
            {
                output_error(")");
                return;
            }

            output_current_token();
            lexer();
        }
        else if (get_current_record().token == "real")
        {
            write_production("<Primary> -> <Real>\n");
            output_current_token();
            Real();
        }
        else if (get_current_record().lexeme == "true")
        {
            write_production("<Primary> -> true\n");
            output_current_token();
            lexer();
        }
        else if (get_current_record().lexeme == "false")
        {
            write_production("<Primary> -> false\n");
            output_current_token();
            lexer();
        }
        else
        {
            output_error("primary that starts with identifier, integer, (, real, true, or false");
        }
    }

    // Simulates <Primary Prime> ::= ( IDs ) | <Empty>
    void Primary_Prime()
    {
        if (get_current_record().lexeme == "(")
        {
            write_production("<Primary Prime> -> ( <IDs> )\n");
            
            output_current_token();
            lexer();

            IDs();

            if (get_current_record().lexeme != ")")
            {
                output_error(")");
                return;
            }

            output_current_token();
            lexer();
        }
        else Empty("<Primary Prime>");
    }

    // Recognizes whether the current token is an identifier
    void Identifier()
    {
        if (get_current_record().token != "identifier")
        {
            output_error("identifier");
            return;
        }

        lexer();
    }

    // Recognizes whether the current token is an integer
    void Integer()
    {
        if (get_current_record().token != "integer")
        {
            output_error("integer");
            return;
        }

        lexer();
    }

    // Recognizes whether the current token is a real
    void Real()
    {
        if (get_current_record().token != "real")
        {
            output_error("real");
            return;
        }

        lexer();
    }

    // Simulates <Empty> ::= ε
    void Empty(const std::string& nonterminal)
    {
        write_production(nonterminal + " -> ");
        write_production("\u03B5");
        write_production("\n");
    }
};
