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
#include <unordered_set>
#include <stdexcept>

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

    // Safely get current record with bounds checking
    const Record& get_current_record() const 
    { 
        if (m_current_token_index >= static_cast<int>(m_records.size()))
        {
            throw std::out_of_range("Parser error: Token index out of bounds");
        }
        return m_records[m_current_token_index]; 
    }

    void write_productions_to_file()
    {
        Rat26S();
    }

    void write_production(const std::string& production)
    {
        if (m_print_productions)
        {
            m_output_file_stream << production;
        }
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

    // Helper function to check if current token is in a set
    bool is_in_first_set(const std::unordered_set<std::string>& first_set) const
    {
        return first_set.count(get_current_record().lexeme) || first_set.count(get_current_record().token);
    }

    void Rat26S()
    {
        write_production("<Rat26S> ::= @ <Opt Function Definitions> @ <Opt Declaration List> @ <Statement List> @\n");
        
        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        consume_token();

        Opt_Function_Definitions();

        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        consume_token();

        Opt_Declaration_List();

        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        consume_token();

        Statement_List();

        if (get_current_record().lexeme != "@")
        {
            output_error("@");
            return;
        }

        output_current_token();
        consume_token();

        // Check for proper end of file
        if (get_current_record().token != "eof")
        {
            output_error("end of file");
        }
    }
    
    void Opt_Function_Definitions()
    {
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
        write_production("<Function Definitions> ::= <Function> <Function Definitions Prime>\n");
        
        Function();
        Function_Definitions_Prime();
    }

    void Function_Definitions_Prime()
    {
        write_production("<Function Definitions Prime> ::= <Function Definitions> | <Empty>\n");
        
        if (get_current_record().lexeme == "function")
        {
            output_current_token();
            Function_Definitions();
        }
        // epsilon is allowed here
    }

    void Function()
    {
        write_production("<Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n");
        
        if (get_current_record().lexeme != "function")
        {
            output_error("function");
            return;
        }

        // output_current_token();
        consume_token();
        
        if (get_current_record().token == "identifier")
            output_current_token();

        Identifier();
        
        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        consume_token();

        Opt_Parameter_List();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        consume_token();

        Opt_Declaration_List();
        Body();
    }

    void Opt_Parameter_List()
    {
        write_production("<Opt Parameter List> ::= <Parameter List> | <Empty>\n");

        if (get_current_record().token == "identifier")
        {
            Parameter_List();
        }
        // epsilon because nothing can happen
    }

    void Parameter_List()
    {
        write_production("<Parameter List> ::= <Parameter> <Parameter List Prime>\n");
        
        Parameter();
        Parameter_List_Prime();
    }

    void Parameter_List_Prime()
    {
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
        write_production("<Parameter> ::= <IDs> <Qualifier>\n");

        IDs();
        Qualifier();
    }

    void Qualifier()
    {
        write_production("<Qualifier> ::= integer | boolean | real\n");
        
        static const std::unordered_set<std::string> qualifiers = {"integer", "boolean", "real"};
        
        if (!qualifiers.count(get_current_record().lexeme))
        {
            output_error("integer, boolean, or real");
            return;
        }

        output_current_token();
        consume_token();
    }

    void Body()
    {
        write_production("<Body> ::= { < Statement List> }\n");
        
        if (get_current_record().lexeme != "{")
        {
            output_error("{");
            return;
        }

        output_current_token();
        consume_token();

        Statement_List();

        if (get_current_record().lexeme != "}")
        {
            output_error("}");
            return;
        }

        output_current_token();
        consume_token();
    }

    void Opt_Declaration_List()
    {
        write_production("<Opt Declaration List> ::= <Declaration List> | <Empty>\n");

        static const std::unordered_set<std::string> first_set = {"integer", "boolean", "real"};
        
        if (is_in_first_set(first_set))
        {
            Declaration_List();
        }
        // epsilon is allowed here
    }

    void Declaration_List()
    {
        write_production("<Declaration List> ::= <Declaration> ; <Declaration List Prime>\n");
            
        Declaration();

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
            return;
        }

        output_current_token();
        consume_token();
        Declaration_List_Prime();
    }

    void Declaration_List_Prime()
    {
        write_production("<Declaration List Prime> ::= <Declaration List> | <Empty>\n");
        
        static const std::unordered_set<std::string> first_set = {"integer", "boolean", "real"};
        
        if (is_in_first_set(first_set))
        {
            Declaration_List();
        }
        // epsilon is allowed here
    }

    void Declaration()
    {
        write_production("<Declaration> ::= <Qualifier> <IDs>\n");

        Qualifier();
        IDs();
    }

    void IDs()
    {
        write_production("<IDs> ::= <Identifier> <IDs Prime>\n");
        
        if (get_current_record().token == "identifier")
            output_current_token();
        
        Identifier();
        IDs_Prime();
    }

    void IDs_Prime()
    {
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
        write_production("<Statement List> ::= <Statement> <Statement List Prime>\n");
        
        Statement();
        Statement_List_Prime();
    }

    void Statement_List_Prime()
    {
        write_production("<Statement List Prime> ::= <Statement List> | <Empty>\n");

        static const std::unordered_set<std::string> first_set = {"{", "identifier", "if", "return", "write", "read", "while"};

        if (is_in_first_set(first_set))
        {
            Statement_List();
        }
        // epsilon is okay here
    }

    void Statement()
    {
        // Determine which type of statement based on first token
        if (get_current_record().lexeme == "{")
        {
            output_current_token();
            write_production("<Statement> ::= <Compound>\n");
            Compound();
        }
        else if (get_current_record().token == "identifier")
        {
            output_current_token();
            write_production("<Statement> ::= <Assign>\n");
            Assign();
        }
        else if (get_current_record().lexeme == "if")
        {
            output_current_token();
            write_production("<Statement> ::= <If>\n");
            If();
        }
        else if (get_current_record().lexeme == "return")
        {
            output_current_token();
            write_production("<Statement> ::= <Return>\n");
            Return();
        }
        else if (get_current_record().lexeme == "write")
        {
            output_current_token();
            write_production("<Statement> ::= <Print>\n");
            Print();
        }
        else if (get_current_record().lexeme == "read")
        {
            output_current_token();
            write_production("<Statement> ::= <Scan>\n");
            Scan();
        }
        else if (get_current_record().lexeme == "while")
        {
            output_current_token();
            write_production("<Statement> ::= <While>\n");
            While();
        }
        else
        {
            output_error("statement that begins with {, an identifier, if, return, write, read, or while");
        }
    }

    void Compound()
    {
        write_production("<Compound> ::= { <Statement List> }\n");

        if (get_current_record().lexeme != "{")
        {
            output_error("{");
            return;
        }

        output_current_token();
        consume_token();

        Statement_List();

        if (get_current_record().lexeme != "}")
        {
            output_error("}");
            return;
        }

        output_current_token();
        consume_token();
    }

    void Assign()
    {
        write_production("<Assign> ::= <Identifier> = <Expression>;\n");

        Identifier();

        if (get_current_record().lexeme != "=")
        {
            output_error("=");
            return;
        }

        output_current_token();
        consume_token();
        
        Expression();
    }

    void If()
    {
        write_production("<If> ::= if ( <Condition> ) <Statement> <If Prime>\n");
        
        if (get_current_record().lexeme != "if")
        {
            output_error("if");
            return;
        }

        consume_token();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        consume_token();

        Condition();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        consume_token();

        Statement();
        If_Prime();
    }

    void If_Prime()
    {
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

            if (get_current_record().lexeme != "fi")
            {
                output_error("fi");
                return;
            }

            output_current_token();
            consume_token();
        }
        else
        {
            output_error("fi or otherwise");
        }
    }

    void Return()
    {
        write_production("<Return> ::= return <Return Prime>\n");

        if (get_current_record().lexeme != "return")
        {
            output_error("return");
            return;
        }

        consume_token();
        Return_Prime();
    }

    void Return_Prime()
    {
        write_production("<Return Prime> ::= ; | <Expression>\n");

        static const std::unordered_set<std::string> first_set_of_expression = {"-", "identifier", "integer", "(", "real", "true", "false"};

        if (get_current_record().lexeme == ";")
        {
            output_current_token();
            consume_token();
        }
        else if (is_in_first_set(first_set_of_expression))
        {
            Expression();

            if (get_current_record().lexeme != ";")
            {
                output_error(";");
                return;
            }

            output_current_token();
            consume_token();
        }
        else
        {
            output_error("; or expression that starts with -, identifier, integer, (, real, true, or false");
        }
    }

    void Print()
    {
        write_production("<Print> ::= write ( <Expression> );\n");
        
        if (get_current_record().lexeme != "write")
        {
            output_error("write");
            return;
        }

        consume_token();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        consume_token();

        Expression();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        consume_token();

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
            return;
        }

        output_current_token();
        consume_token();
    }

    void Scan()
    {
        write_production("<Scan> ::= read ( <IDs> );\n");
        
        if (get_current_record().lexeme != "read")
        {
            output_error("read");
            return;
        }

        consume_token();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        consume_token();

        IDs();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        consume_token();

        if (get_current_record().lexeme != ";")
        {
            output_error(";");
            return;
        }

        output_current_token();
        consume_token();
    }

    void While()
    {
        write_production("<While> ::= while ( <Condition> ) <Statement>\n");
        
        if (get_current_record().lexeme != "while")
        {
            output_error("while");
            return;
        }

        output_current_token();
        consume_token();

        if (get_current_record().lexeme != "(")
        {
            output_error("(");
            return;
        }

        output_current_token();
        consume_token();
        
        Condition();

        if (get_current_record().lexeme != ")")
        {
            output_error(")");
            return;
        }

        output_current_token();
        consume_token();

        Statement();
    }

    void Condition()
    {
        write_production("<Condition> ::= <Expression> <Relop> <Expression>\n");
        
        Expression();
        Relop();
        Expression();
    }

    void Relop()
    {
        write_production("<Relop> ::= == | != | > | < | <= | =>\n");

        static const std::unordered_set<std::string> operators = {"==", "!=", ">", "<", "<=", "=>"};
        
        if (!operators.count(get_current_record().lexeme))
        {
            output_error("==, !=, >, <, <=, or =>");
            return;
        }

        output_current_token();
        consume_token();
    }

    void Expression()
    {
        write_production("<Expression> ::= <Term> <Expression Prime>\n");

        Term();
        Expression_Prime();
    }

    void Expression_Prime()
    {
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
        // else epsilon
    }

    void Term()
    {
        write_production("<Term> ::= <Factor> <Term Prime>\n");
        
        Factor();
        Term_Prime();
    }

    void Term_Prime()
    {
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
        // else epsilon
    }

    void Factor()
    {
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
        write_production("<Primary> ::= <Identifier> <Primary Prime> | <Integer> | ( <Expression> ) | <Real> | true | false\n");
        
        if (get_current_record().token == "identifier")
        {
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

            if (get_current_record().lexeme != ")")
            {
                output_error(")");
                return;
            }

            output_current_token();
            consume_token();
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
            output_error("primary that starts with identifier, integer, (, real, true, or false");
        }
    }

    void Primary_Prime()
    {
        write_production("<Primary Prime> ::= ( IDs ) | <Empty>\n");

        if (get_current_record().lexeme == "(")
        {
            output_current_token();
            consume_token();

            IDs();

            if (get_current_record().lexeme != ")")
            {
                output_error(")");
                return;
            }

            output_current_token();
            consume_token();
        }
        // else epsilon
    }

    void Identifier()
    {
        if (get_current_record().token != "identifier")
        {
            output_error("identifier");
            return;
        }

        consume_token();
    }

    void Integer()
    {
        if (get_current_record().token != "integer")
        {
            output_error("integer");
            return;
        }

        consume_token();
    }

    void Real()
    {
        if (get_current_record().token != "real")
        {
            output_error("real");
            return;
        }

        consume_token();
    }
};
