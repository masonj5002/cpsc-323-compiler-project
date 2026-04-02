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


// The naming convention is that the function's names are the nonterminals in
// the assignment documentation for the compiler project 

class Rat26SParser
{
 private:
  std::vector<Record> m_records;
  std::ofstream&      m_output_file_stream;
  bool                m_print_productions;
  int                 m_current_token_index;
  std::string         m_current_production;
  std::string         m_input_file_name;

 public:
 
    Rat26SParser(const std::vector<Record>& records, const std::string& input_file_name, std::ofstream& output_file_stream, bool print_productions=true)
     : m_records(records),
       m_output_file_stream(output_file_stream),
       m_print_productions(print_productions),
       m_current_token_index(0),
       m_current_production(""),
       m_input_file_name(input_file_name)
     {}

    const Record& get_current_record() const { return m_records[m_current_token_index]; }

    void write_productions_to_file()
    {
        Rat26S();
    }

    void write_production(const std::string& production)
    {
            m_output_file_stream << production;
            m_current_production = production;
    }
    
    void output_current_token()
    {
        m_output_file_stream << "\nToken: " << std::setw(20) << std::left << get_current_record().token << "Lexeme: " << get_current_record().lexeme << '\n';
    }

    void consume_token()
    {
        ++m_current_token_index;
    }
    
    void output_error(const std::string& expected_message)
    {
        m_output_file_stream << "\nSyntax Error in " << m_input_file_name << " on line " << get_current_record().line << " with\n"
                             << "\tToken       : " << get_current_record().token  << '\n'
                             << "\tLexeme      : " << get_current_record().lexeme << '\n'
                             << "\tExpected    : " << expected_message            << '\n'
                             << "\tViolation of: " << m_current_production        << '\n';
    }

    void Rat26S()
    {
        if (m_print_productions)
            write_production("<Rat26S> ::= @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @\n");
        
        // This is to prevent two syntax errors being written to the output file because there is 
        // another check at the bottom of this nonterminal function
        bool first_check = false;

        if (get_current_record().lexeme == "@")
        {
            first_check = true;

            output_current_token();
            consume_token();

            Opt_Function_Definitions();

            if (get_current_record().lexeme == "@")
            {
                output_current_token();
                consume_token();

                Opt_Declaration_List();

                if (get_current_record().lexeme == "@")
                {
                    output_current_token();
                    consume_token();

                    Statement_List();

                    if (get_current_record().lexeme == "@")
                    {
                        output_current_token();
                        consume_token();
                    }
                    else output_error("@");
                }
                else output_error("@");
            }
            else output_error("@");
        }
        else output_error("@");

        // If the lexeme is not @, then that means we got passed the very first check and were
        // originally looking through the right hand side of the production for <Rat@26S>. 
        // If it was @, then that means the very first check of the grammar failed and
        // there is a syntax error that was already written to the output file.
        if (first_check && get_current_record().token != "eof")
        {
            output_error("eof");
        }
    }
    
    void Opt_Function_Definitions()
    {

        if (m_print_productions)
            write_production("<Opt Function Definitions> ::= <Function Definitions> | <Empty>\n");

        if (get_current_record().lexeme == "function")
        {
            output_current_token();
            Function_Definitions();
        }
        // epsilon allowed here
    }

    void Function_Definitions()
    {
        if (m_print_productions)
            write_production("<Function Definitions> ::= <Function> <Function Definitions Prime>\n");
        
        Function();
        Function_Definitions_Prime();
    }

    void Function_Definitions_Prime()
    {
        if (m_print_productions)
            write_production("<Function Definitions Prime> ::= <Function Definitions> | <Empty>\n");
        
        if (get_current_record().lexeme == "function")
        {
            Function_Definitions();
        }
        // epsilon is allowed here
    }

    void Function()
    {
        if (m_print_productions)
            write_production("<Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n");
        
        if (get_current_record().lexeme == "function")
        {
            consume_token();
            
            if (get_current_record().token == "identifier")
                output_current_token();

            Identifier();
            
            if (get_current_record().lexeme == "(")
            {
                output_current_token();
                consume_token();

                Opt_Parameter_List();

                if (get_current_record().lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    Opt_Declaration_List();
                    Body();
                }
                else output_error(")");
            }
            else output_error("(");
        }
        else output_error("function");
    }

    void Opt_Parameter_List()
    {
        if (m_print_productions)
            write_production("<Opt Parameter List> ::= <Parameter List> | <Empty>\n");

        if (get_current_record().token == "identifier")
        {
            Parameter_List();
        }
        // epsilon because nothing can happen
    }

    void Parameter_List()
    {
        if (m_print_productions)
            write_production("<Parameter List> ::= <Parameter> <Parameter List Prime>\n");
        
        Parameter();
        Parameter_List_Prime();
    }

    void Parameter_List_Prime()
    {
        if (m_print_productions)
            write_production("<Parameter List Prime> ::= , <Parameter List> | <Empty>\n");

        if (get_current_record().lexeme == ",")
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
            write_production("<Parameter> ::= <IDs> <Qualifier>\n");

        IDs();
        Qualifier();
    }

    void Qualifier()
    {
        if (m_print_productions)
            write_production("<Qualifier> ::= integer | boolean | real\n");
        
        if (get_current_record().lexeme == "integer" || get_current_record().lexeme == "boolean" || get_current_record().lexeme == "real")
        {
            output_current_token();
            consume_token();
        }
        else output_error("integer, boolean, or real");
    }

    void Body()
    {
        if (m_print_productions)
            write_production("<Body> ::= { < Statement List> }\n");
        
        if (get_current_record().lexeme == "{")
        {
            output_current_token();
            consume_token();

            Statement_List();

            if (get_current_record().lexeme == "}")
            {
                output_current_token();
                consume_token();
            }
            else output_error("}");
        } 
        else output_error("{");
        
    }

    void Opt_Declaration_List()
    {
        if (m_print_productions)
            write_production("<Opt Declaration List> ::= <Declaration List> | <Empty>\n");

        std::vector<std::string> first_set_of_opt_declaration_list = {"integer", "boolean", "real"}; // technically epsilon as well but we don't count that
        
        for (const auto& terminal : first_set_of_opt_declaration_list)
        {
            if (get_current_record().lexeme == terminal)
            {
                Declaration_List();
            } // epsilon is allowed here, aka nothing happens
        }
        
    }

    void Declaration_List()
    {
        if (m_print_productions)
            write_production("<Declaration List> ::= <Declaration> ; <Declaration List Prime>\n");
            
        Declaration();

        if (get_current_record().lexeme == ";")
        {
            output_current_token();
            consume_token();
            Declaration_List_Prime();
        }
        else
        {
            output_error(";");
        }
    }

    void Declaration_List_Prime()
    {
        if (m_print_productions)
            write_production("<Declaration List Prime> ::= <Declaration List> | <Empty>\n");
        
        if (get_current_record().lexeme == "integer" || get_current_record().lexeme == "boolean" || get_current_record().lexeme == "real")
        {
            // output_current_token();

            // WE DON'T CONSUME BECAUSE THE NONTERMINAL IS NOT IN THE CURRENT PRODUCTION WE ARE LOOKING AT
            
            Declaration_List();
        }
    }

    void Declaration()
    {
        if (m_print_productions)
            write_production("<Declaration> ::= <Qualifier> <IDs>\n");

        Qualifier();
        IDs();
    }

    void IDs()
    {
        if (m_print_productions)
            write_production("<IDs> ::= <Identifier> <IDs Prime>\n");
        
        if (get_current_record().token == "identifier")
            output_current_token();
        
        Identifier();
        IDs_Prime();
    }

    void IDs_Prime()
    {
        if (m_print_productions)
            write_production("<IDs Prime> ::= , <IDs> | <Empty>\n");

        if (get_current_record().lexeme == ",")
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
            write_production("<Statement List> ::= <Statement> <Statement List Prime>\n");
        
        Statement();
        Statement_List_Prime();
    }

    void Statement_List_Prime()
    {
        if (m_print_productions)
            write_production("<Statement List Prime> ::= <Statement List> | <Empty>\n");

        std::vector<std::string> first_set_of_statement_list_prime = {"{", "identifier", "if", "return", "write", "read", "while"};

        for (const auto& terminal : first_set_of_statement_list_prime)
        {
            if (get_current_record().lexeme == terminal || get_current_record().token == "identifier")
            {
                Statement_List();
                break;
            }
        }
        // epsilon is okay here
    }

    void Statement()
    {
        // if (m_print_productions)
        //      write_production("<Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>\n");

        if (get_current_record().lexeme == "{")
        {
            output_current_token();
            if (m_print_productions) write_production("<Statement> ::= <Compound>\n");
            Compound();
        }
        else if (get_current_record().token == "identifier")
        {
            output_current_token();
            if (m_print_productions) write_production("<Statement> ::= <Assign>\n");
            Assign();
        }
        else if (get_current_record().lexeme == "if")
        {
            output_current_token();
            if (m_print_productions) write_production("<Statement> ::= <If>\n");
            If();
        }
        else if (get_current_record().lexeme == "return")
        {
            output_current_token();
            if (m_print_productions) write_production("<Statement> ::= <Return>\n");
            Return();
        }
        else if (get_current_record().lexeme == "write")
        {
            output_current_token();
            if (m_print_productions) write_production("<Statement> ::= <Print>\n");
            Print();
        }
        else if (get_current_record().lexeme == "read")
        {
            output_current_token();
            if (m_print_productions) write_production("<Statement> ::= <Scan>\n");
            Scan();
        }
        else if (get_current_record().lexeme == "while")
        {
            output_current_token();
            if (m_print_productions) write_production("<Statement> ::= <While>\n");
            While();
        }
        else
        {
            output_error("statement that begins with {, an identifier, if, return, write, read, or while");
        }

    }

    void Compound()
    {
        if (m_print_productions)
            write_production("<Compound> ::= { <Statement List> }\n");

        if (get_current_record().lexeme == "{")
        {
            output_current_token();
            consume_token();

            Statement_List();

            if (get_current_record().lexeme == "}")
            {
                output_current_token();
                consume_token();
            }
            else output_error("}");
        }
        else output_error("{");
    }

    void Assign()
    {
        if (m_print_productions)
            write_production("<Assign> ::= <Identifier> = <Expression>;\n");

        Identifier();

        if (get_current_record().lexeme == "=")
        {
            output_current_token();
            consume_token();
            
            Expression();
        }
        else output_error("=");
    }

    void If()
    {
        if (m_print_productions)
            write_production("<If> ::= if ( <Condition> ) <Statement> <If Prime>\n");
        
        if (get_current_record().lexeme == "if")
        {
            consume_token();

            if (get_current_record().lexeme == "(")
            {
                output_current_token();
                consume_token();

                Condition();

                if (get_current_record().lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    Statement();
                    If_Prime();
                }
                else output_error(")");
            }
            else output_error("(");
        }
        else output_error("if");
    }

    void If_Prime()
    {
        if (m_print_productions)
            write_production("<If Prime> ::= fi | otherwise <Statement> fi\n");
        
        if (get_current_record().lexeme == "fi")
        {
            output_current_token();
            consume_token();
        }
        else if (get_current_record().lexeme == "otherwise")
        {
            output_current_token();
            consume_token();

            Statement();

            if (get_current_record().lexeme == "fi")
            {
                output_current_token();
                consume_token();
            }
            else output_error("fi");
        }
        else output_error("fi or otherwise");
    }

    void Return()
    {
        if (m_print_productions)
            write_production("<Return> ::= return <Return Prime>\n");

        if (get_current_record().lexeme == "return")
        {
            consume_token();

            Return_Prime();
        }
        else output_error("return");
    }

    void Return_Prime()
    {
        if (m_print_productions)
            write_production("<Return Prime> ::= ; | <Expression>\n");


        std::vector<std::string> first_set_of_expression = {"-", "identifier", "integer", "(", "real", "true", "false"};

        bool token_is_in_first_set = false;
        for (const auto& terminal : first_set_of_expression)
        {
            if (get_current_record().lexeme == terminal || get_current_record().token == terminal)
            {
                token_is_in_first_set = true;
                break;
            }
        }

        if (get_current_record().lexeme == ";")
        {
            output_current_token();
            consume_token();
        }
        else if (token_is_in_first_set)
        {
            // output_current_token();
            Expression();

            if (get_current_record().lexeme == ";")
            {
                output_current_token();
                consume_token();
            }
            else output_error(";");
        }
        else output_error("expression that starts with -, identifier, integer, (, real, true, or false");
    }

    void Print()
    {
        if (m_print_productions)
            write_production("<Print> ::= write ( <Expression> );\n");
        
        if (get_current_record().lexeme == "write")
        {
            consume_token();

            if (get_current_record().lexeme == "(")
            {
                output_current_token();
                consume_token();

                Expression();

                if (get_current_record().lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    if (get_current_record().lexeme == ";")
                    {
                        output_current_token();
                        consume_token();
                    } 
                    else output_error(";");
                }  
                else output_error(")");
            }
            else output_error("(");
        }
        else output_error("write");
    }

    void Scan()
    {
        if (m_print_productions)
            write_production("<Scan> ::= read ( <IDs> );\n");
        
        if (get_current_record().lexeme == "read")
        {
            consume_token();

            if (get_current_record().lexeme == "(")
            {
                output_current_token();
                consume_token();

                IDs();

                if (get_current_record().lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    if (get_current_record().lexeme == ";")
                    {
                        output_current_token();
                        consume_token();
                    } else output_error(";");
                } else output_error(")");
            } else output_error("(");
        } else output_error("read");
    }

    void While()
    {
        if (m_print_productions)
            write_production("<While> ::= while ( <Condition> ) <Statement>\n");
        
        if (get_current_record().lexeme == "while")
        {
            output_current_token();
            consume_token();

            if (get_current_record().lexeme == "(")
            {
                output_current_token();
                consume_token();
                Condition();

                if (get_current_record().lexeme == ")")
                {
                    output_current_token();
                    consume_token();

                    Statement();
                }
                else output_error(")");
            }
            else output_error("(");
        }
        else output_error("while");
    }

    void Condition()
    {
        if (m_print_productions)
            write_production("<Condition> ::= <Expression> <Relop> <Expression>\n");
        
        Expression();
        Relop();
        Expression();
    }

    void Relop()
    {
        if (m_print_productions)
            write_production("<Relop> ::= == | != | > | < | <= | =>\n");

        std::vector<std::string> terminals = {"==", "!=", ">", "<", "<=", "=>"};
        
        bool terminal_found = false;
        for (const auto& terminal : terminals)
        {
            if (get_current_record().lexeme == terminal)
            {
                output_current_token();
                consume_token();
                terminal_found = true;
                break;
            }
        }

        if (!terminal_found)
        {
            output_error("==, !=, >, <, <=, or =>");
        }
    }

    void Expression()
    {
        if (m_print_productions)
            write_production("<Expression> ::= <Term> <Expression Prime>\n");

        Term();
        Expression_Prime();
    }

    void Expression_Prime()
    {
        if (m_print_productions)
            write_production("<Expression Prime> ::= + <Term> <Expression Prime> | - <Term> <Expression Prime> | <Empty>\n");
        
        if (get_current_record().lexeme == "+")
        {
            output_current_token();
            consume_token();
            Term();
            Expression_Prime();
        }
        else if (get_current_record().lexeme == "-")
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
            write_production("<Term> ::= <Factor> <Term Prime>\n");
        
        Factor();
        Term_Prime();
    }

    void Term_Prime()
    {
        if (m_print_productions)
            write_production("<Term Prime> ::= * <Factor> <Term Prime> | / <Factor> <Term Prime> | <Empty>\n");
        
        if (get_current_record().lexeme == "*")
        {
            output_current_token();
            consume_token();
            Factor();
            Term_Prime();
        }
        else if (get_current_record().lexeme == "/")
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
            write_production("<Factor> ::= - <Primary> | <Primary>\n");
        
        if (get_current_record().lexeme == "-")
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
            write_production("<Primary> ::= <Identifier> <Primary Prime> | <Integer> | ( <Expression> ) | <Real> | true | false\n");
        
        if (get_current_record().token == "identifier")
        {
            //maybe you print here, and the consumption is done later
            output_current_token();
            Identifier();
            Primary_Prime();
        }
        else if (get_current_record().token == "integer")
        {
            output_current_token();
            Integer();
        }
        else if (get_current_record().lexeme == "(")
        {
            output_current_token();
            consume_token();

            Expression();

            if (get_current_record().lexeme == ")")
            {
                output_current_token();
                consume_token();
            }
            else output_error(")");
        }
        else if (get_current_record().token == "real")
        {
            output_current_token();
            Real();
        }
        else if (get_current_record().lexeme == "true" || get_current_record().lexeme == "false")
        {
            output_current_token();
            consume_token();
        }
        else
        {
            output_error("primary that starts with identifier, integer, (, ), real, true, or false");
        }
    }

    void Primary_Prime()
    {
        if (m_print_productions)
            write_production("<Primary Prime> ::= ( IDs ) | <Empty>\n");

        if (get_current_record().lexeme == "(")
        {
            output_current_token();
            consume_token();

            IDs();

            if (get_current_record().lexeme == ")")
            {
                output_current_token();
                consume_token();
            }
            else output_error(")");
        }
    }

    // void Empty()
    // {
    // 
    //     write_production("\u03B5\n") // Empty ::= epsilon
    //     
    // }

    void Identifier()
    {

        if (get_current_record().token == "identifier")
        {
            consume_token();
        }
        else
        {
            output_error("identifier");
        }
    }

    void Integer()
    {

        if (get_current_record().token == "integer")
        {
            consume_token();
        }
        else
        {
            output_error("integer");
        }
    }

    void Real()
    {

        if (get_current_record().token == "real")
        {
            consume_token();
        }
        else
        {
            output_error("real");
        }
    }
    
};
