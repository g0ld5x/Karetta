#include <cstdint>
#include <sstream>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <variant>
#include <vector>
#include <iostream>
#include "../lexer/lexer.hpp"
#include "parser.hpp"

std::string variantToString1(const Value &a);
StmtPtr parseStatement(const std::vector<Token> &tokens, size_t &pos);
ExprPtr parseExpression(const std::vector<Token> &tokens, size_t &pos, int minBindPower);


StmtPtr parseAssignment(const std::vector<Token>& tokens, size_t& pos)
{
    ExprPtr target = parseExpression(tokens, pos, 0);

    if (pos >= tokens.size() ||
        tokens[pos].type != TokenType::Equal)
    {
        throw std::runtime_error("Expected '='");
    }

    ++pos; // consume '='

    ExprPtr value = parseExpression(tokens, pos, 0);

    Assignment assignment{
        std::move(value),
        std::move(target)
    };

    return std::make_unique<Stmt>(
        std::move(assignment)
    );
}

ExprPtr parseArrayLiteral(const std::vector<Token> &tokens, size_t &pos){
    ArrayLiteral array;
    ++pos; //this is to consume the OpenBracket.
        if (tokens[pos].type == TokenType::CloseBracket)
    {
        ++pos;
        return std::make_unique<Expr>(std::move(array));
    }

    while (true)
    {
        array.elements.push_back(
            parseExpression(tokens, pos, 0)
        );

        if (tokens[pos].type == TokenType::CloseBracket)
        {
            ++pos;
            break;
        }

        else if (tokens[pos].type == TokenType::Comma)
        {
            ++pos;
        }


    }

    return std::make_unique<Expr>(std::move(array));
}

    StmtPtr parseBreak(const std::vector<Token> & tokens,size_t & pos){
    ++pos;
    return std::make_unique<Stmt>(
    BreakStatement{}
    );

}

    StmtPtr parseContinue(const std::vector<Token> & tokens,size_t & pos){
    ++pos;
    return std::make_unique<Stmt>(
    ContinueStatement{}
    );

}

std::string tokenTypeToString1(TokenType type)
{
    switch (type)
    {
    case TokenType::Add:
        return "+";
    case TokenType::Minus:
        return "-";
    case TokenType::Multiply:
        return "*";
    case TokenType::Divide:
        return "/";
    case TokenType::Modulo:
        return "%";

    case TokenType::Equal:
        return "=";
    case TokenType::EqualEqual:
        return "==";
    case TokenType::NotEqual:
        return "!=";

    case TokenType::Smaller:
        return "<";
    case TokenType::SmallerEqual:
        return "<=";
    case TokenType::Bigger:
        return ">";
    case TokenType::BiggerEqual:
        return ">=";

    case TokenType::BitAnd:
        return "&";
    case TokenType::BitOr:
        return "|";
    case TokenType::BitXor:
        return "^";
    case TokenType::BitNot:
        return "~";

    case TokenType::AndAnd:
        return "&&";
    case TokenType::OrOr:
        return "||";
    case TokenType::UnaryNot:
        return "!";

    case TokenType::BitLeft:
        return "<<";
    case TokenType::BitRight:
        return ">>";

    case TokenType::OpenParan:
        return "(";
    case TokenType::CloseParan:
        return ")";
    case TokenType::Comma:
        return ",";
    case TokenType::StatementEnd:
        return ";";

    case TokenType::isType:
        return "inType";
    case TokenType::Identifier:
        return "identifier";
    case TokenType::Int:
        return "integer";
    case TokenType::Double:
        return "double";
    case TokenType::String:
        return "string";
    case TokenType::Char:
        return "char";
    case TokenType::Bool:
        return "bool";
    case TokenType::EndOfFile:
        return "EOF";

    default:
        return "unknown";
    }
}
 //cool stuff
double variantToDouble(const Value &value)
{
    if (const auto *p = std::get_if<double>(&value))
        return *p;

    if (const auto *p = std::get_if<int>(&value))
        return static_cast<double>(*p);

    if (const auto *p = std::get_if<bool>(&value))
        return *p ? 1.0 : 0.0;

    if (const auto *p = std::get_if<std::string>(&value))
    {
        try
        {
            return std::stod(*p);
        }
        catch (...)
        {
            return 0.0;
        }
    }

    return 0.0;
}

int variantToInt(const Value &value)
{
    if (const auto *p = std::get_if<int>(&value))
        return *p;

    if (const auto *p = std::get_if<double>(&value))
        return static_cast<int>(*p);

    if (const auto *p = std::get_if<bool>(&value))
        return *p ? 1 : 0;

    if (const auto *p = std::get_if<std::string>(&value))
    {
        try
        {
            return std::stoi(*p);
        }
        catch (...)
        {
            return 0;
        }
    }

    return 0;
}

bool variantToBool(const Value &value)
{
    if (const bool *val_ptr = std::get_if<bool>(&value))
    {
        return *val_ptr;
    }
    else
    {
        std::cout << "bad";
        return false;
        // handle error here
    }
}

std::string arrayToString(const std::shared_ptr<ArrayValue> &array)
{
    std::string result = "[";

    for (size_t i = 0; i < array->values.size(); i++)
    {
        result += variantToString1(array->values[i]);

        if (i != array->values.size() - 1)
            result += ",";
    }

    result += "]";

    return result;
}

char variantToChar(const Value &value)
{
    if (const char *c = std::get_if<char>(&value))
        return *c;

    throw std::runtime_error("Value does not contain a char");
}

int getBindingPower(TokenType type)
{
    switch (type)
    {
    // Multiplicative
    case TokenType::OpenBracket:
        return 100;
    case TokenType::UnaryNot:
        return 80;
    case TokenType::Multiply:
    case TokenType::Divide:
    case TokenType::Modulo:
        return 70;

    // Additive
    case TokenType::Add:
    case TokenType::Minus:
        return 60;

    // Shift
    case TokenType::BitLeft:
    case TokenType::BitRight:
        return 50;

    // Relational
    case TokenType::Smaller:
    case TokenType::SmallerEqual:
    case TokenType::Bigger:
    case TokenType::BiggerEqual:
        return 40;

    // Equality
    case TokenType::EqualEqual:
    case TokenType::isType:
    case TokenType::NotEqual:
        return 35;

    // Bitwise AND
    case TokenType::BitAnd:
        return 30;

    // Bitwise XOR
    case TokenType::BitXor:
        return 25;

    // Bitwise OR
    case TokenType::BitOr:
        return 20;

    // Logical AND
    case TokenType::AndAnd:
        return 15;

    // Logical OR
    case TokenType::OrOr:
        return 10;

    default:
        return -1;
    }
}

StmtPtr parseReturn(const std::vector<Token>& tokens, size_t& pos)
{
    ++pos; // consume return

    if ((tokens[pos].type == TokenType::StatementEnd  || tokens[pos].type == TokenType::EndOfFile)|| tokens[pos].type == TokenType::CloseCurl)
    {
        return std::make_unique<Stmt>(
            ReturnStatement{nullptr}
        );
    }

    return std::make_unique<Stmt>(
        ReturnStatement{parseExpression(tokens, pos, 0)}
    );
}

std::string variantToString1(const Value &a)
{
    return std::visit([](const auto &arg) -> std::string
                      {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::monostate>)
        {
            return "null";
        }
        else if constexpr (std::is_same_v<T, ArrayPtr>)
        {
            if (!arg)
                return "null";

            return arrayToString(arg);
        }
        else
        {
            std::ostringstream oss;
            oss << arg;
            return oss.str();
        } }, a);
}


StmtPtr parseExpressionStatement(
    const std::vector<Token>& tokens,
    size_t& pos)
{
    ExprPtr expr = parseExpression(tokens, pos, 0);

    if (pos < tokens.size() &&
        tokens[pos].type == TokenType::StatementEnd)
    {
        ++pos;
    }

    return std::make_unique<Stmt>(
        ExpressionStatement{std::move(expr)}
    );
}

StmtPtr parseImport(const std::vector<Token>& tokens, size_t& pos)
{
    std::vector<std::string> pathS;

    ++pos; // consume 'using'

    if (pos >= tokens.size())
        throw std::runtime_error("Expected import path");

    if (tokens[pos].type == TokenType::String)
    {
        pathS.push_back(
            variantToString1(tokens[pos].value)
        );

        ++pos;

        return std::make_unique<Stmt>(
            ImportStatement{std::move(pathS)}
        );
    }

    throw std::runtime_error(
        "Expected string after 'using'"
    );
}

ExprPtr parseFunctionCall(
    const std::vector<Token> & tokens,
    size_t & pos)
{
    std::string name =
        variantToString1(tokens[pos].value);

    ++pos; 

    if (pos >= tokens.size() ||
        tokens[pos].type != TokenType::OpenParan)
    {
        throw std::runtime_error(
            "Expected '(' after function name.");
    }

    ++pos; // consume '('

    std::vector<ExprPtr> arguments;

    // No arguments: foo()
    if (pos < tokens.size() &&
        tokens[pos].type == TokenType::CloseParan)
    {
        ++pos;
    }
    else
    {
        while (true)
        {
            arguments.push_back(
                parseExpression(tokens, pos, 0));

            if (pos >= tokens.size())
            {
                throw std::runtime_error(
                    "Expected ')' after function arguments.");
            }

            if (tokens[pos].type == TokenType::Comma)
            {
                ++pos;
                continue;
            }
            else if (tokens[pos].type == TokenType::CloseParan)
            {
                ++pos;
                break;
            }
        }
    }

    return std::make_unique<Expr>(
        FunctionCall{
            std::move(name),
            std::move(arguments)});
}
bool isPrefixOperator(TokenType type)
{
    return type == TokenType::UnaryNot ||
           type == TokenType::Add ||
           type == TokenType::Minus;
}
ExprPtr makeUnary(TokenType op, ExprPtr operand)
{
    return std::make_unique<Expr>(
        UnaryExpr{
            std::move(operand),
            op
        }
    );
}
ExprPtr parseExpression(const std::vector<Token> &tokens, size_t &pos, int minBindPower)
{
    ExprPtr rhs;
    ExprPtr lhs;
    if (isPrefixOperator(tokens[pos].type))
    {
        TokenType op = tokens[pos++].type;
        ExprPtr operand = parseExpression(tokens, pos, 80);

        return makeUnary(op, std::move(operand));
    }
    if (tokens[pos].type == TokenType::Int)
    {
        lhs = std::make_unique<Expr>(
            IntLiteral{variantToInt(tokens[pos].value)});
        ++pos;
    }
    else if (tokens[pos].type == TokenType::OpenParan)
    {
        ++pos; // consume '('

        lhs = parseExpression(tokens, pos, 0);

        if (pos >= tokens.size() ||
            tokens[pos].type != TokenType::CloseParan)
        {
            throw std::runtime_error("Expected ')'");
        }

        ++pos; // consume ')'
    }
    else if (tokens[pos].type == TokenType::Double)
    {
        lhs = std::make_unique<Expr>(
            DoubleLiteral{variantToDouble(tokens[pos].value)});
        ++pos;
    }
    else if (tokens[pos].type == TokenType::OpenBracket)
    {
        lhs = parseArrayLiteral(tokens, pos);
    }
    else if (tokens[pos].type == TokenType::String)
    {
        lhs = std::make_unique<Expr>(
            StringLiteral{variantToString1(tokens[pos].value)});
        ++pos;
    }
    else if (tokens[pos].type == TokenType::Identifier)
    {
        if (pos + 1 < tokens.size() && tokens[pos + 1].type == TokenType::OpenParan)
        {
            lhs = parseFunctionCall(tokens, pos);
        }
        else
        {

            lhs = std::make_unique<Expr>(
                VariableCall{variantToString1(tokens[pos].value)});
            ++pos;
        }
    }
    else if (tokens[pos].type == TokenType::Bool)
    {
        lhs = std::make_unique<Expr>(
            BoolLiteral{variantToBool(tokens[pos].value)});
        ++pos;
    }
    else if (tokens[pos].type == TokenType::Char)
    {
        lhs = std::make_unique<Expr>(
            CharLiteral{variantToChar(tokens[pos].value)});
        ++pos;
    }
    else
    {
        // unary expression
    }
while (pos < tokens.size() &&
       getBindingPower(tokens[pos].type) > minBindPower)
{
    TokenType op = tokens[pos].type;

    // Array indexing
    if (op == TokenType::OpenBracket)
    {
        ++pos; // consume '['
        
        ExprPtr index = parseExpression(tokens, pos, 0);

        if (pos >= tokens.size() ||
            tokens[pos].type != TokenType::CloseBracket)
        {
            std::cout << "Pos: " << pos << " Tokens size: " << tokens.size() << "\n";
            throw std::runtime_error("Expected ']'");
        }

        ++pos; // consume ']'

        lhs = std::make_unique<Expr>(
            IndexExpr{
                std::move(lhs),
                std::move(index)
            });

        continue;
    }

    // Normal binary operator
    int bindPower = getBindingPower(op);

    ++pos;

    ExprPtr rhs = parseExpression(
        tokens,
        pos,
        bindPower);

    lhs = std::make_unique<Expr>(
        BinaryExpr{
            std::move(lhs),
            std::move(rhs),
            op});
}
    return lhs;
}

StmtPtr parseVariableDeclare(const std::vector<Token> &tokens, size_t &pos)
{
    if (variantToString1(tokens[pos].value) == "let")
    {
        VariableDeclaration vardeclare;

        bool isStrict = false;
        bool isMonoType = false;
        bool isGlobal = false;
        bool unInitialized = false;

        ++pos; // global a = 100;

        while (pos < tokens.size() &&
               tokens[pos].type == TokenType::Identifier)
        {
            std::string value =
                variantToString1(tokens[pos].value);

            if (pos + 1 < tokens.size() &&
                tokens[pos + 1].type == TokenType::Equal)
            {
                vardeclare.name = value;
                ++pos;
                break;
            }
            else if (
                pos + 1 < tokens.size() &&
                (tokens[pos + 1].type == TokenType::StatementEnd ||
                 tokens[pos + 1].type == TokenType::EndOfFile))
            {
                vardeclare.name = value;
                vardeclare.initializer = nullptr;
                unInitialized = true;
                break;
            }

            if (value == "strict")
            {
                if (isStrict)
                {
                    throw std::runtime_error("Already used 'strict' modfier.");
                }
                isStrict = true;
            }
            else if(value == "monotype"){
                if(isMonoType){
                    throw std::runtime_error("Already used 'monotype' modfier.");
                }
                isMonoType = true;
            }
            else if (value == "global")
            {
                if (isGlobal)
                {
                    throw std::runtime_error("Already used 'global' modfier.");
                }
                isGlobal = true;
            }
            else if (
                pos + 1 < tokens.size() &&
                (tokens[pos + 1].type == TokenType::StatementEnd ||
                 tokens[pos + 1].type == TokenType::EndOfFile))
            {
                vardeclare.name = value;
                vardeclare.initializer = nullptr;
                unInitialized = true;

                ++pos; // consume the variable name

                break;
            }
            else
            {
                throw std::runtime_error(
                    "Unknown modifier '" + value + "'");
            }

            ++pos;
        }

        vardeclare.vardata.isConst = false;
        vardeclare.vardata.isGlobal = isGlobal;
        vardeclare.vardata.isMonoType = isMonoType;
        vardeclare.vardata.isStrict = isStrict;

        if (tokens[pos].type == TokenType::StatementEnd || tokens[pos].type == TokenType::EndOfFile)
        {
            // pos should be at ';' or EOF
            return std::make_unique<Stmt>(std::move(vardeclare));
        }

        if (pos >= tokens.size() || (tokens[pos].type != TokenType::Equal && !unInitialized))
        {
            throw std::runtime_error(
                "Expected '=' in variable declaration.");
        }

        ++pos;

        vardeclare.initializer =
            parseExpression(tokens, pos, 0);


        if (pos < tokens.size() &&
            tokens[pos].type == TokenType::StatementEnd)
        {
            ++pos;
        }

        return std::make_unique<Stmt>(
            std::move(vardeclare));
    }
if (variantToString1(tokens[pos].value) == "const")
    {
        VariableDeclaration vardeclare;

        bool isStrict = false;
        bool isGlobal = false;
        bool isMonoType = false;
        bool unInitialized = false;

        ++pos;

        while (pos < tokens.size() &&
               tokens[pos].type == TokenType::Identifier)
        {
            std::string value =
                variantToString1(tokens[pos].value);

            if (pos + 1 < tokens.size() &&
                tokens[pos + 1].type == TokenType::Equal)
            {
                vardeclare.name = value;
                ++pos;
                break;
            }
            else if (
                pos + 1 < tokens.size() &&
                (tokens[pos + 1].type == TokenType::StatementEnd ||
                 tokens[pos + 1].type == TokenType::EndOfFile))
            {
                throw std::runtime_error("Constant '"+ variantToString1(tokens[pos].value)+ "' cannot be left in a uninitialized state.");
                vardeclare.name = value;
                vardeclare.initializer = nullptr;
                unInitialized = true;
                break;
            }

            if (value == "strict")
            {
                if (isStrict)
                {
                    throw std::runtime_error("Already used 'strict' modfier.");
                }
                isStrict = true;
            }
            else if(value == "monotype"){
                if(isMonoType){
                    throw std::runtime_error("Already used 'monotype' modfier.");
                }
                isMonoType = true;
            }
            else if (value == "global")
            {
                if (isGlobal)
                {
                    throw std::runtime_error("Already used 'global' modfier.");
                }
                isGlobal = true;
            }
            else
            {
                throw std::runtime_error(
                    "Unknown modifier '" + value + "'");
            }

            ++pos;
        }

        vardeclare.vardata.isConst = true;
        vardeclare.vardata.isGlobal = isGlobal;
        vardeclare.vardata.isStrict = isStrict;
        vardeclare.vardata.isMonoType = isMonoType;

        if (tokens[pos].type == TokenType::StatementEnd || tokens[pos].type == TokenType::EndOfFile)
        {
            // pos should be at ';' or EOF
            return std::make_unique<Stmt>(std::move(vardeclare));
        }

        if (pos >= tokens.size() || (tokens[pos].type != TokenType::Equal && !unInitialized))
        {
            throw std::runtime_error(
                "Expected '=' in variable declaration.");
        }

        ++pos; 

        vardeclare.initializer =
            parseExpression(tokens, pos, 0);



        
        if (pos < tokens.size() &&
            tokens[pos].type == TokenType::StatementEnd)
        {
            ++pos;
        }

        return std::make_unique<Stmt>(
            std::move(vardeclare));
    }
}

StmtPtr parseFunctionDeclare(
    const std::vector<Token>& tokens,
    size_t& pos)
{
    FunctionDeclaration func;

    ++pos; // fn

    // function name
    if (tokens[pos].type != TokenType::Identifier)
        throw std::runtime_error("Expected function name");

    func.name = variantToString1(tokens[pos].value);
    ++pos;

    // (
    if (tokens[pos].type != TokenType::OpenParan)
        throw std::runtime_error("Expected '('");

    ++pos;

    if (tokens[pos].type != TokenType::CloseParan)
    {
        while (true)
        {
            if (tokens[pos].type != TokenType::Identifier)
                throw std::runtime_error("Expected parameter name");

            func.parameters.push_back(
                variantToString1(tokens[pos].value)
            );

            ++pos;

            if (tokens[pos].type == TokenType::CloseParan)
                break;

            if (tokens[pos].type != TokenType::Comma)
                throw std::runtime_error("Expected ',' or ')'");

            ++pos; // consume comma
        }
    }

    ++pos; // consume ')'


    // {
    if (tokens[pos].type != TokenType::OpenCurl)
        throw std::runtime_error("Expected '{'");

    ++pos; // consume {

    // Parse statements until matching }
while (tokens[pos].type != TokenType::CloseCurl)
{
    func.body.statements.push_back(
        parseStatement(tokens, pos)
    );
}

    if (pos >= tokens.size())
        throw std::runtime_error("Expected '}'");

    ++pos; // consume }

    return std::make_unique<Stmt>(
        std::move(func)
    );
}
// let global a = 10;
StmtPtr parseStatement(const std::vector<Token>& tokens, size_t& pos)
{
    switch (tokens[pos].type)
    {
    case TokenType::Identifier:
    {
        std::string name =
            variantToString1(tokens[pos].value);

        if (name == "let")
            return parseVariableDeclare(tokens, pos);
        else if(name == "return"){
            return parseReturn(tokens,pos);
        }
        else if (name == "fn")
            return parseFunctionDeclare(tokens, pos);

        else if (name == "using")
            return parseImport(tokens, pos);
        else if(name == "break")
            return parseBreak(tokens,pos);
        else if(name == "continue")
            return parseContinue(tokens,pos);
        if (pos + 1 < tokens.size() &&
            tokens[pos + 1].type == TokenType::OpenParan)
        {
            return parseExpressionStatement(tokens, pos);
        }

        return parseAssignment(tokens, pos);
    }

    default:
        break;
    }

    throw std::runtime_error("Unexpected token");
}

Program parse(const std::vector<Token> &tokens)
{
    Program program;

    size_t pos = 0;

    while (pos < tokens.size())
    {
        if (tokens[pos].type == TokenType::EndOfFile)
            break;
        program.statements.push_back(
            parseStatement(tokens, pos));
    }

    return program;
}