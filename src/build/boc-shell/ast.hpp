#pragma once

#include "ast/node.hpp"
#include <string>

namespace gorc {

    enum class unary_operator {
        atom,
        car,
        cdr,
        logical_not,
        null
    };

    enum class infix_operator {
        cons,
        equal,
        not_equal
    };

    /* Expression */
    class argument_expression;
    class nil_expression;
    class call_expression;
    class unary_expression;
    class infix_expression;
    using expression = variant<argument_expression*,
                               nil_expression*,
                               call_expression*,
                               unary_expression*,
                               infix_expression*>;

    class argument;
    class argument_expression : public visitable_ast_node<argument_expression> {
    public:
        argument *value;

        argument_expression(diagnostic_context_location const &loc,
                            argument *value);
    };

    class nil_expression : public visitable_ast_node<nil_expression> {
    public:
        nil_expression(diagnostic_context_location const &loc);
    };

    class simple_word;
    class call_expression : public visitable_ast_node<call_expression> {
    public:
        simple_word *name;
        ast_list_node<expression*> *arguments;

        call_expression(diagnostic_context_location const &loc,
                        simple_word *name,
                        ast_list_node<expression*> *arguments);
    };

    class unary_expression : public visitable_ast_node<unary_expression> {
    public:
        expression *value;
        unary_operator op;

        unary_expression(diagnostic_context_location const &loc,
                         expression *value,
                         unary_operator op);
    };

    class infix_expression : public visitable_ast_node<infix_expression> {
    public:
        expression *left;
        expression *right;
        infix_operator op;

        infix_expression(diagnostic_context_location const &loc,
                         expression *left,
                         expression *right,
                         infix_operator op);
    };

    /* General */

    class simple_word;
    class expression_word;
    class variable_name;
    class environment_variable_name;

    using lvalue = variant<variable_name*,
                           environment_variable_name*>;

    using word = variant<simple_word*,
                         expression_word*,
                         variable_name*,
                         environment_variable_name*>;

    class simple_word : public visitable_ast_node<simple_word> {
    public:
        std::string value;

        simple_word(diagnostic_context_location const &loc,
                    std::string const &value);
    };

    class expression_word : public visitable_ast_node<expression_word> {
    public:
        expression *value;

        expression_word(diagnostic_context_location const &loc,
                        expression *value);
    };

    class variable_name : public visitable_ast_node<variable_name> {
    public:
        std::string name;

        variable_name(diagnostic_context_location const &loc,
                      std::string const &name);
    };

    class environment_variable_name : public visitable_ast_node<environment_variable_name> {
    public:
        std::string name;

        environment_variable_name(diagnostic_context_location const &loc,
                                  std::string const &name);
    };

    class argument : public visitable_ast_node<argument> {
    public:
        ast_list_node<word*> *words;

        argument(diagnostic_context_location const &loc,
                 ast_list_node<word*> *words);
    };

    /* Commands */

    class subcommand : public visitable_ast_node<subcommand> {
    public:
        ast_list_node<argument*> *arguments;

        subcommand(diagnostic_context_location const &loc,
                   ast_list_node<argument*> *arguments);
    };

    class pipe_command;
    using command = variant<pipe_command*>;

    class pipe_command : public visitable_ast_node<pipe_command> {
    public:
        ast_list_node<subcommand*> *subcommands;

        pipe_command(diagnostic_context_location const &loc,
                     ast_list_node<subcommand*> *subcommands);
    };

    /* Statements */

    class compound_statement;
    class command_statement;
    class var_declaration_statement;
    class func_declaration_statement;
    class assignment_statement;
    class if_statement;
    class if_else_statement;
    class return_statement;
    class call_statement;
    using statement = variant<compound_statement*,
                              command_statement*,
                              var_declaration_statement*,
                              func_declaration_statement*,
                              assignment_statement*,
                              if_statement*,
                              if_else_statement*,
                              return_statement*,
                              call_statement*>;

    class compound_statement : public visitable_ast_node<compound_statement> {
    public:
        ast_list_node<statement*> *code;

        compound_statement(diagnostic_context_location const &loc,
                           ast_list_node<statement*> *code);
    };

    class command_statement : public visitable_ast_node<command_statement> {
    public:
        command *cmd;

        command_statement(diagnostic_context_location const &loc,
                          command *cmd);
    };

    class var_declaration_statement : public visitable_ast_node<var_declaration_statement> {
    public:
        variable_name *var;
        maybe<ast_list_node<argument*>*> value;

        var_declaration_statement(diagnostic_context_location const &loc,
                                  variable_name *var,
                                  maybe<ast_list_node<argument*>*> value);
    };

    class func_declaration_statement : public visitable_ast_node<func_declaration_statement> {
    public:
        simple_word *name;
        ast_list_node<simple_word *> *arguments;
        statement *code;

        func_declaration_statement(diagnostic_context_location const &loc,
                                   simple_word *name,
                                   ast_list_node<simple_word *> *arguments,
                                   statement *code);
    };

    class assignment_statement : public visitable_ast_node<assignment_statement> {
    public:
        lvalue *var;
        ast_list_node<argument*> *value;

        assignment_statement(diagnostic_context_location const &loc,
                             lvalue *var,
                             ast_list_node<argument*> *value);
    };

    class if_statement : public visitable_ast_node<if_statement> {
    public:
        expression *condition;
        statement *code;

        if_statement(diagnostic_context_location const &loc,
                     expression *condition,
                     statement *code);
    };

    class if_else_statement : public visitable_ast_node<if_else_statement> {
    public:
        expression *condition;
        statement *code;
        statement *elsecode;

        if_else_statement(diagnostic_context_location const &loc,
                          expression *condition,
                          statement *code,
                          statement *elsecode);
    };

    class return_statement : public visitable_ast_node<return_statement> {
    public:
        maybe<expression*> value;

        return_statement(diagnostic_context_location const &loc,
                         maybe<expression*> value);
    };

    class call_statement : public visitable_ast_node<call_statement> {
    public:
        expression *value;

        call_statement(diagnostic_context_location const &loc,
                       expression *value);
    };

    class translation_unit : public visitable_ast_node<translation_unit> {
    public:
        ast_list_node<statement*> *code;

        translation_unit(diagnostic_context_location const &loc,
                         ast_list_node<statement*> *code);
    };

}
