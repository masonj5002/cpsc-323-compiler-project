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
#include <iomanip>

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
  Record&             m_current_record = m_records[m_current_token_index];

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

    void output_current_token()
    {
        m_output_file_stream << std::setw(20) << std::left << m_current_record.token << ' ' << m_current_record.lexeme << '\n';
        // ++m_current_token_index;
    }

    void consume_token()
    {
        ++m_current_token_index;
    }
    
    void output_error()
    {
        std::cerr << "ERROR!!!!! RAT26S\n";
    }

    bool Rat26S()
    {

        if (m_print_productions)
            m_output_file_stream << "<Rat26S> ::= @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @\n";
        
        if (m_current_record.lexeme == "@")
        {
            output_current_token();
            consume_token();

            Opt_Function_Definitions();

            if (m_current_record.lexeme == "@")
            {
                output_current_token();
                consume_token();

                Opt_Declaration_List();

                if (m_current_record.lexeme == "@")
                {
                    output_current_token();
                    consume_token();

                    Statement_List();

                    if (m_current_record.lexeme == "@")
                    {
                        output_current_token();
                        consume_token();
                    }
                    else output_error();
                }
                else output_error();
            }
            else output_error();
        }
        else output_error();

        if (m_current_record.token != "eof")
        {
            output_error();
        }
    }
    
    void Opt_Function_Definitions()
    {

        if (m_print_productions)
            m_output_file_stream << "<Opt Function Definitions> ::= <Function Definitions> | <Empty>\n";

        if (m_current_record.lexeme == "function")
        {
            Function_Definitions();
        }
        // epsilon allowed here
    }

    void Function_Definitions()
    {
        if (m_print_productions)
            m_output_file_stream << "<Function Definitions> ::= <Function> <Function Definitions Prime>\n";
        
        Function();
        Function_Definitions_Prime();
    }

    void Function_Definitions_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Function Definitions Prime> ::= <Empty> | <Function Definitions>\n";
        
        if (m_current_record.lexeme == "function")
        {
            // output_current_token();
            // consume_token();
            
            Function_Definitions();
        }
        // epsilon is allowed here
    }

    void Function()
    {
        if (m_print_productions)
            m_output_file_stream << "<Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n";
        
        if (m_current_record.lexeme == "function")
        {
            output_current_token();
            consume_token();
            
            Identifier();
            
            if (m_current_record.lexeme == "(")
            {
                output_current_token();
                consume_token();

                Opt_Parameter_List();

                if (m_current_record.lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    Opt_Declaration_List();
                    Body();
                }
                else output_error();
            }
            else output_error();
        }
        else output_error();
    }

    bool Opt_Parameter_List()
    {
        if (m_print_productions)
            m_output_file_stream << "<Opt Parameter List> ::= <Parameter List> | <Empty>\n";

        
    }

    void Parameter_List()
    {
        if (m_print_productions)
            m_output_file_stream << "<Parameter List> ::= <Parameter> <Parameter List Prime>\n";
        
        Parameter();
        Parameter_List_Prime();

        
    }

    void Parameter_List_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Parameter List Prime> ::= <Empty> | , <Parameter List>\n";

        if (m_current_record.lexeme == ",")
        {
            output_current_token();
            consume_token();
            Parameter_List();
        }
        // else epsilon which is allowed here
    }

    void Parameter()
    {
        if (m_print_productions)
            m_output_file_stream << "<Parameter> ::= <IDs> <Qualifier>\n";
        
        IDs();
        Qualifier();
    }

    void Qualifier()
    {
        if (m_print_productions)
            m_output_file_stream << "<Qualifier> ::= integer | boolean | real\n";
        
        if (m_current_record.lexeme == "integer" || m_current_record.lexeme == "boolean" || m_current_record.lexeme == "real")
        {
            output_current_token();
            consume_token();
        }
        else output_error();
    }

    void Body()
    {
        if (m_print_productions)
            m_output_file_stream << "<Body> ::= { < Statement List> }\n";
        
        if (m_current_record.lexeme == "{")
        {
            output_current_token();
            consume_token();

            Statement_List();

            if (m_current_record.lexeme == "}")
            {
                output_current_token();
                consume_token();
            }
            else output_error();
        } 
        else output_error();
        
    }

    void Opt_Declaration_List()
    {
        if (m_print_productions)
            m_output_file_stream << "<Opt Declaration List> ::= <Declaration List> | <Empty>\n";

        std::vector<std::string> first_set_of_opt_declaration_list = {"integer", "boolean", "real"}; // technically epsilon as well but we don't count that
        
        for (const auto& terminal : first_set_of_opt_declaration_list)
        {
            if (m_current_record.lexeme == terminal)
            {
                output_current_token();
                Declaration_List();
            } // epsilon is allowed here, aka nothing happens
        }
        
    }

    void Declaration_List()
    {
        if (m_print_productions)
            m_output_file_stream << "<Declaration List> ::= <Declaration> ; <Declaration List Prime>\n";
            
        Declaration();

        if (m_current_record.lexeme == ";")
        {
            output_current_token();
            consume_token();
            Declaration_List_Prime();
        }
        else
        {
            output_error();
        }
    }

    void Declaration_List_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Declaration List Prime> ::= <Empty> | <Declaration List>\n";
        
        if (m_current_record.lexeme == "integer" || m_current_record.lexeme == "boolean" || m_current_record.lexeme == "real")
        {
            // output_current_token();

            // WE DON'T CONSUME BECAUSE THE NONTERMINAL IS NOT IN THE CURRENT PRODUCTION WE ARE LOOKING AT
            
            Declaration_List();
        }
    }

    void Declaration()
    {
        if (m_print_productions)
            m_output_file_stream << "<Declaration> ::= <Qualifier> <IDs>\n";

        Qualifier();
        IDs();
    }

    void IDs()
    {
        if (m_print_productions)
            m_output_file_stream << "<IDs> ::= <Identifier> <IDs Prime>\n";
        
        Identifier();
        IDs_Prime();
    }

    void IDs_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<IDs Prime> ::= <Empty> | , <IDs>\n";

        if (m_current_record.lexeme == ",")
        {
            output_current_token();
            consume_token();
            IDs();
        }
        // else epsilon which is allowed here
    }

    void Statement_List()
    {
        if (m_print_productions)
            m_output_file_stream << "<Statement List> ::= <Statement> <Statement List Prime>\n";
        
        Statement();
        Statement_List_Prime();
    }

    void Statement_List_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Statement List Prime> ::= <Empty> | <Statement List>\n";

        std::vector<std::string> first_set_of_statement_list_prime = {"{", "identifier", "if", "return", "write", "read", "while"};

        for (const auto& terminal : first_set_of_statement_list_prime)
        {
            if (m_current_record.lexeme == terminal || m_current_record.token == "identifier")
            {
                output_current_token();
                consume_token();
                Statement_List();
            }
        }
        // epsilon is okay here
    }

    bool Statement()
    {
        if (m_print_productions)
             m_output_file_stream << "<Statement> -> <Assign>\n";

             
        if (m_current_record.lexeme == "{")
        {
            Compound();
        }
        else if (m_current_record.token == "identifier")
        {
            Assign();
        }
        else if (m_current_record.lexeme == "if")
        {
            If();
        }
        else if (m_current_record.lexeme == "return")
        {
            Return();
        }
        else if (m_current_record.lexeme == "write")
        {
            Print();
        }
        else if (m_current_record.lexeme == "read")
        {
            Scan();
        }
        else if (m_current_record.lexeme == "while")
        {
            While();
        }
        else
        {
            output_error();
        }

        return false;
    }

    void Compound()
    {
        if (m_print_productions)
            m_output_file_stream << "<Compound> ::= { <Statement List> }\n";

        if (m_current_record.lexeme == "{")
        {
            output_current_token();
            consume_token();

            Statement_List();

            if (m_current_record.lexeme == "}")
            {
                output_current_token();
                consume_token();
            }
            else output_error();
        }
        else output_error();
    }

    void Assign()
    {
        if (m_print_productions)
            m_output_file_stream << "<Assign> ::= <Identifier> = <Expression>;\n";

        Identifier();

        if (m_current_record.lexeme == "=")
        {
            output_current_token();
            Expression();
        }
        else
        {
            output_error();
        }
        
    }

    void If()
    {
        if (m_print_productions)
            m_output_file_stream << "<If> ::= if ( <Condition> ) <Statement> <If Prime>\n";
        
        if (m_current_record.lexeme == "if")
        {
            output_current_token();
            consume_token();

            if (m_current_record.lexeme == "(")
            {
                output_current_token();
                consume_token();

                Condition();

                if (m_current_record.lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    Statement();
                    If_Prime();
                }
                else output_error();
            }
            else output_error();
        }
        else output_error();
    }

    void If_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<If Prime> ::= fi | otherwise <Statement> fi\n";
        
        if (m_current_record.lexeme == "fi")
        {
            output_current_token();
            consume_token();
        }
        else if (m_current_record.lexeme == "otherwise")
        {
            output_current_token();
            consume_token();

            Statement();

            if (m_current_record.lexeme == "fi")
            {
                output_current_token();
                consume_token();
            }
            else output_error();
        }
        else output_error();
    }

    void Return()
    {
        if (m_print_productions)
            m_output_file_stream << "<Return> ::= return <Return Prime>\n";

        if (m_current_record.lexeme == "return")
        {
            output_current_token();
            consume_token();

            Return_Prime();
        }
        else output_error();
    }

    void Return_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Return Prime> ::= ; | <Expression>\n";


        std::vector<std::string> first_set_of_expression = {"-", "identifier", "integer", "(", "real", "true", "false"};

        bool token_is_in_first_set = false;
        for (const auto& terminal : first_set_of_expression)
        {
            if (m_current_record.lexeme == terminal || m_current_record.token == terminal)
            {
                token_is_in_first_set = true;
            }
        }

        if (m_current_record.lexeme == ";")
        {
            output_current_token();
            consume_token();
        }
        else if (token_is_in_first_set)
        {
            // output_current_token();
            Expression();
        }
    }

    void Print()
    {
        if (m_print_productions)
            m_output_file_stream << "<Print> ::= write ( <Expression> );\n";
        
        if (m_current_record.lexeme == "write")
        {
            output_current_token();
            consume_token();

            if (m_current_record.lexeme == "(")
            {
                output_current_token();
                consume_token();

                Expression();

                if (m_current_record.lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    if (m_current_record.lexeme == ";")
                    {
                        output_current_token();
                        consume_token();
                    } 
                    else output_error();
                }  
                else output_error();
            }
            else output_error();
        }
        else output_error();
    }

    void Scan()
    {
        if (m_print_productions)
            m_output_file_stream << "<Scan> ::= read ( <IDs> );\n";
        
        if (m_current_record.lexeme == "read")
        {
            output_current_token();
            consume_token();

            if (m_current_record.lexeme == "(")
            {
                output_current_token();
                consume_token();

                IDs();

                if (m_current_record.lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    if (m_current_record.lexeme == ";")
                    {
                        output_current_token();
                        consume_token();
                    } else output_error();
                } else output_error();
            } else output_error();
        } else output_error();
    }

    void While()
    {
        if (m_print_productions)
            m_output_file_stream << "<While> ::= while ( <Condition> ) <Statement>\n";
        
        if (m_current_record.lexeme == "while")
        {
            output_current_token();
            consume_token();

            if (m_current_record.lexeme == "(")
            {
                output_current_token();
                consume_token();
                Condition();

                if (m_current_record.lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    Statement();
                }
                else output_error();
            }
            else output_error();
        }
        else output_error();
    }

    void Condition()
    {
        if (m_print_productions)
            m_output_file_stream << "<Condition> ::= <Expression> <Relop> <Expression>\n";
        
        Expression();
        Relop();
        Expression();
    }

    void Relop()
    {
        if (m_print_productions)
            m_output_file_stream << "<Relop> ::= == | != | > | < | <= | =>\n";

        std::vector<std::string> terminals = {"==", "!=", ">", "<", "<=", "=>"};

        for (const auto& terminal : terminals)
        {
            if (m_current_record.lexeme == terminal)
            {
                output_current_token();
                consume_token();
                break;
            }
        }
    }

    void Expression()
    {
        if (m_print_productions)
            m_output_file_stream << "<Expression> ::= <Term> <Expression Prime>\n";

        Term();
        Expression_Prime();

    }

    void Expression_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Expression Prime> ::= + <Term> <Expression Prime> | - <Term> <Expression Prime> | <Empty>\n";
        
        if (m_current_record.lexeme == "+")
        {
            output_current_token();
            consume_token();
            Term();
            Expression_Prime();
        }
        else if (m_current_record.lexeme == "-")
        {
            output_current_token();
            consume_token();
            Term();
            Expression_Prime();
        }
    }

    void Term()
    {
        if (m_print_productions)
            m_output_file_stream << "<Term> ::= <Factor> <Term Prime>\n";
        
        Factor();
        Term_Prime();
    }

    void Term_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Term Prime> ::= * <Factor> <Term Prime> | / <Factor> <Term Prime> | <Empty>\n";
        
        if (m_current_record.lexeme == "*")
        {
            output_current_token();
            consume_token();
            Factor();
            Term_Prime();
        }
        else if (m_current_record.lexeme == "/")
        {
            output_current_token();
            consume_token();
            Factor();
            Term_Prime();
        }
        // else <Empty>
    }

    void Factor()
    {
        if (m_print_productions)
            m_output_file_stream << "<Factor> ::= - <Primary> | <Primary>\n";
        
        if (m_current_record.lexeme == "-")
        {
            output_current_token();
            consume_token();
            Primary();
        }
        else
        {
            Primary();
        }

    }

    void Primary()
    {
        if (m_print_productions)
            m_output_file_stream << "<Primary> ::= <Identifier> <Primary Prime> | <Integer> | ( <Expression> ) | <Real> | true | false\n";
        
        if (m_current_record.token == "identifier")
        {
            //maybe you print here, and the consumption is done later
            output_current_token();
            Identifier();
            Primary_Prime();
        }
        else if (m_current_record.token == "integer")
        {
            Integer();
        }
        else if (m_current_record.lexeme == "(")
        {
            output_current_token();
            consume_token();
            Expression();

            if (m_current_record.lexeme == ")")
            {
                output_current_token();
                consume_token();
            }
            else output_error();
        }
        else if (m_current_record.token == "real")
        {
            Real();
        }
        else if (m_current_record.lexeme == "true" || m_current_record.lexeme == "false")
        {
            output_current_token();
            consume_token();
        }
        else
        {
            output_error();
        }
    }

    void Primary_Prime()
    {
        if (m_print_productions)
            m_output_file_stream << "<Primary Prime> ::= ( IDs ) | <Empty>\n";

        if (m_current_record.lexeme == "(")
        {
            output_current_token();
            consume_token();
            IDs();

            if (m_current_record.lexeme == ")")
            {
                output_current_token();
                consume_token();
            }
            else output_error();
        }
    }

    bool Empty()
    {

        return false;
    }

    void Identifier()
    {

        if (m_current_record.token == "identifier")
        {
            consume_token();
        }
        else
        {
            output_error();
        }
    }

    void Integer()
    {

        if (m_current_record.token == "integer")
        {
            consume_token();
        }
        else
        {
            output_error();
        }
    }

    void Real()
    {

        if (m_current_record.token == "real")
        {
            consume_token();
        }
        else
        {
            output_error();
        }
    }
    
};
