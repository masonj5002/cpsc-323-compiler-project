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
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <stack>
#include <tuple>

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
       current_memory_address(10000), // starting address value (Assignment 3)
       current_type("Unknown"),
       m_instruction_address(1)
       {}
    
    void write_productions_to_file()
    {
        Rat26S();
    }

 private:

    // ----------------------------------------------------------------------------------------------------------------------------------------
    // State of the Parser
    // ----------------------------------------------------------------------------------------------------------------------------------------

    std::vector<Record> m_records;
    std::ofstream&      m_output_file_stream;
    bool                m_print_productions;
    int                 m_current_token_index;
    std::string         m_current_production;
    std::string         m_input_file_name;
    int current_memory_address; // Assignment 3
    std::vector<SymbolTableEntry> symbol_table; // Assignment 3
    std::string current_type;
    std::size_t m_instruction_address;
    std::stack<std::size_t> jmpz_stack;
    std::stack<std::string> id_stack;
    std::vector<std::tuple<std::string, std::string, std::string>> m_instruction_table;
    std::string current_qualifier; // not sure if this will work...
    

    // ----------------------------------------------------------------------------------------------------------------------------------------
    // Object Code Generation Functions
    // ----------------------------------------------------------------------------------------------------------------------------------------

    // Returns the current memory address and increments it by one
    int use_address() {
        current_memory_address++;
        return current_memory_address - 1;
    }


    // Insert the current lexeme into the symbol table
    void insertSymbol()
    {   
        symbol_table.push_back(SymbolTableEntry(get_current_record().lexeme, use_address(), current_type));
    }

    
    // Check if the symbol is already in the symbol table
    bool check_symbol_existence()
    {
        for (const auto& symbol_entry : symbol_table)
        {
            if (symbol_entry.identifier_ == get_current_record().lexeme)
            {
                return true;
            }
        }

        return false;
    }
    
    // Get address of a symbol
    int get_address(const std::string& id)
    {
        for (const auto& symbol_entry : symbol_table)
        {
            if (symbol_entry.identifier_ == id)
            {
                return symbol_entry.memoryAddress_;
            }
        }

        return -1; // -1 indicating that the identifier does not exist in the symbol table
    }
    
    // Output the entire symbol table to standard output and to the specified output file stream
    void print_symbol_table(bool output_to_console=false)
    {
        m_output_file_stream << '\n';
        
        m_output_file_stream << std::setw(30) << std::right << "Symbol Table\n";
        m_output_file_stream << std::setw(20) << std::left << "Identifier" << std::setw(20) << std::left << "MemoryLocation" << std::setw(20) << std::left << "Type" << '\n';;
        
        if (output_to_console)
        {
            std::cout            << '\n';
            std::cout << std::setw(30) << std::right << "Symbol Table\n";
            std::cout << std::setw(20) << std::left << "Identifier" << std::setw(20) << std::left << "MemoryLocation" << std::setw(20) << std::left << "Type" << '\n';
        }

        for (const auto& symbol_entry : symbol_table ) {
            m_output_file_stream << std::setw(20) << std::left << symbol_entry.identifier_ << std::setw(20) << std::left << symbol_entry.memoryAddress_ << std::setw(20) << std::left << symbol_entry.type_ << '\n';
            
            if (output_to_console)
                std::cout << std::setw(20) << std::left << symbol_entry.identifier_ << std::setw(20) << std::left << symbol_entry.memoryAddress_ << std::setw(20) << std::left << symbol_entry.type_ << '\n';
        }
    }

    
    // Output a semantic error
    void output_semantic_error(const std::string& error_description)
    {
        m_output_file_stream << "\nRat26S Semantic Error in " << m_input_file_name << " on line " << get_current_record().line << " with\n"
                             << "\tToken       : " << get_current_record().token   << '\n'
                             << "\tLexeme      : " << get_current_record().lexeme  << '\n'
                             << '\t'               << error_description            << '\n';
        
        std::cout << "\nRat26S Semantic Error in " << m_input_file_name << " on line " << get_current_record().line << " with\n"
                  << "\tToken       : " << get_current_record().token   << '\n'
                  << "\tLexeme      : " << get_current_record().lexeme  << '\n'
                  << '\t'               << error_description            << '\n';
    }

    // Generate an assembly instruction
    void generate_instruction(const std::string& instruction_operation, const std::string& operand)
    {
        m_instruction_table.push_back(std::make_tuple(std::to_string(m_instruction_address), instruction_operation, operand));

        ++m_instruction_address;
    }

    // Takes in the instruction address of a JMP instruction and 
    // replaces the operand with the new current instruction address
    void back_patch(const std::size_t& jmp_instruction_address)
    {
        std::size_t jmpz_address = jmpz_stack.top();
        jmpz_stack.pop();

        std::get<2>(m_instruction_table[jmpz_address - 1]) = std::to_string(jmp_instruction_address);

    }

    void print_instruction_table(bool output_to_console=false)
    {
        for (const auto& t : m_instruction_table)
        {
            m_output_file_stream << std::setw(10) << std::left << std::get<0>(t) << std::setw(10) << std::left << std::get<1>(t) << std::setw(10) << std::left << (std::get<2>(t) == "nil" ? "" : std::get<2>(t)) << '\n';
            
            if (output_to_console)
                std::cout << std::setw(10) << std::left << std::get<0>(t) << std::setw(10) << std::left << std::get<1>(t) << std::setw(10) << std::left << (std::get<2>(t) == "nil" ? "" : std::get<2>(t)) << '\n';
        }
    }

    // ----------------------------------------------------------------------------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------------------------------------------------------------------------

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
        if (m_print_productions)
        {
            m_output_file_stream << "\nToken: " << std::setw(20) << std::left << get_current_record().token << "Lexeme: " << get_current_record().lexeme << '\n';
        }
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

    
    // ----------------------------------------------------------------------------------------------------------------------------------------
    // Functions that simulate productions
    // ----------------------------------------------------------------------------------------------------------------------------------------

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

        // Display the complete instruction table and symbol table once
        // syntax analysis and object code generation is finished
        print_instruction_table(false);
        print_symbol_table(false);
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

    // Simulates <Parameter> ::= <IDs> <Qualifier>
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
        
        // Save the current type for type checking
        current_type = get_current_record().lexeme; 

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

    // Simulates <Declaration> ::= <Qualifier> <IDs>
    void Declaration()
    {
        write_production("<Declaration> -> <Qualifier> <IDs>\n");

        Qualifier(); // This function will also update the current type when it finishes

        IDs(true);
    }

    // Simulates <IDs> ::= <Identifier> <IDs Prime>
    void IDs(bool is_declaration=false)
    {
        write_production("<IDs> -> <Identifier> <IDs Prime>\n");
        
        if (get_current_record().token == "identifier")
        {
            output_current_token();

            if (!is_declaration)
            {   
                if (check_symbol_existence())
                {
                    id_stack.push(get_current_record().lexeme);
                }
                else
                {
                    output_semantic_error("Undefined identifier");
                }
            }
            else
            {
                if(!check_symbol_existence())
                {
                    insertSymbol();
                }
                else
                {
                    output_semantic_error("Multiple declarations of identifier");
                }
            }
            
        }

        Identifier();
        IDs_Prime(is_declaration);
    }

    // Simulates <IDs Prime> ::= , <IDs> | <Empty>
    void IDs_Prime(bool is_declaration=false)
    {
        if (get_current_record().lexeme == ",")
        {
            write_production("<IDs Prime> -> , <IDs>\n");

            output_current_token();
            lexer();
            
            IDs(is_declaration);
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

        std::string save = get_current_record().lexeme;

        Identifier();

        if (get_current_record().lexeme != "=")
        {
            output_error("=");
            return;
        }

        output_current_token();
        lexer();
        

        // TODO (DELETE THIS WHEN FINISHED): We need to be able to determine the overall type of the <Expression> ================================================================ Assignment 3
        // so that we can compare the identifier's type with the expression's type for type checking.
        Expression();

        generate_instruction("POPM", std::to_string(get_address(save)));

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
        }

        output_current_token(); 
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

        Condition(); // Note: There is a jmpz created by calling Condition() which is back patched later on when If_Prime() is called

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
            
            back_patch(m_instruction_address);
            generate_instruction("LABEL", "nil");
            
            output_current_token();
            lexer();
        }
        else if (get_current_record().lexeme == "otherwise")
        {
            write_production("<If Prime> -> otherwise <Statement> fi\n");

            // Back patching for if-otherwise-fi statement
            std::size_t saved_jmp_address = m_instruction_address;
            generate_instruction("JMP", "nil");

            back_patch(m_instruction_address); 
            generate_instruction("LABEL", "nil");
            
            // This is done separately to avoid the back patching that
            // will be done for the jmpz instruction that is handled above
            jmpz_stack.push(saved_jmp_address);


            output_current_token();
            lexer();

            Statement();

            if (get_current_record().lexeme != "fi")
            {
                output_error("fi");
                return;
            }

            back_patch(m_instruction_address);
            generate_instruction("LABEL", "nil");

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

    // Simulates <Print> ::= write ( <Expression> ) ;
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

        generate_instruction("SOUT", "nil");
        
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

        for (int i = 0; i < id_stack.size(); ++i)
        {
            generate_instruction("SIN", "nil");
        }

        while (!id_stack.empty())
        {
            std::string identifier_argument = id_stack.top();
            id_stack.pop();

            generate_instruction("POPM", std::to_string(get_address(identifier_argument)));
        }

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

        int top_of_loop_instruction_address = m_instruction_address;
        generate_instruction("LABEL", "nil");

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

        generate_instruction("JMP", std::to_string(top_of_loop_instruction_address));
        back_patch(m_instruction_address);
    }

    // Simulates <Condition> ::= <Expression> <Relop> <Expression>
    void Condition()
    {
        write_production("<Condition> -> <Expression> <Relop> <Expression>\n");
        
        Expression();

        std::string current_operator = get_current_record().lexeme;

        Relop();
        Expression();

        if (current_operator == "<")
        {
            generate_instruction("LES", "nil");
            jmpz_stack.push(m_instruction_address);
            generate_instruction("JMPZ", "nil");
        }
        else if (current_operator == ">")
        {
            generate_instruction("GRT", "nil");
            jmpz_stack.push(m_instruction_address);
            generate_instruction("JMPZ", "nil");
        }
        else if (current_operator == "==")
        {
            generate_instruction("EQU", "nil");
            jmpz_stack.push(m_instruction_address);
            generate_instruction("JMPZ", "nil");
        }
        else if (current_operator == "<=")
        {
            generate_instruction("LEQ", "nil");
            jmpz_stack.push(m_instruction_address);
            generate_instruction("JMPZ", "nil");
        }
        else if (current_operator == "=>")
        {
            generate_instruction("GEQ", "nil");
            jmpz_stack.push(m_instruction_address);
            generate_instruction("JMPZ", "nil");
        }
        else if (current_operator == "!=")
        {
            generate_instruction("NEQ", "nil");
            jmpz_stack.push(m_instruction_address);
            generate_instruction("JMPZ", "nil");
        }
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

            generate_instruction("A", "nil");

            Expression_Prime();
        }
        else if (get_current_record().lexeme == "-")
        {
            write_production("<Expression Prime> -> - <Term> <Expression Prime>\n");

            output_current_token();
            lexer();

            Term();
            generate_instruction("S", "nil");

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

            generate_instruction("M", "nil");

            Term_Prime();
        }
        else if (get_current_record().lexeme == "/")
        {
            write_production("<Term Prime> -> / <Factor> <Term Prime>\n");
            output_current_token();
            lexer();
            
            Factor();

            generate_instruction("D", "nil");

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
            
            generate_instruction("PUSHM", std::to_string(get_address(get_current_record().lexeme)));

            Identifier();
            
            Primary_Prime();
        }
        else if (get_current_record().token == "integer")
        {
            write_production("<Primary> -> <Integer>\n");
            output_current_token();

            generate_instruction("PUSHI", get_current_record().lexeme);

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

            generate_instruction("PUSHI", "1");

            output_current_token();                         
            lexer();
        }
        else if (get_current_record().lexeme == "false")
        {
            write_production("<Primary> -> false\n");

            generate_instruction("PUSHI", "0");

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
