
#ifndef AST_HPP
#define AST_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../lexer/lexer.hpp"
struct Expr;

using ExprPtr = std::unique_ptr<Expr>;

enum class VarType: uint8_t{
    Integer,
    Double,
    String,
    Char,
    Bool,
    Array
};

struct VarData{
    bool isStrict;
    VarType variableType;
    bool isGlobal;
    bool isConst;
};

struct Variable{
    Value value;
    VarData data;
};

using VariableTable =  std::unordered_map<std::string ,Variable>;
using ScopeStack = std::vector<VariableTable>;

ExprPtr parseExpression(const std::vector<Token> & tokens,size_t & pos, int  minBindPower);

struct IntLiteral
{
    int value;
};

struct DoubleLiteral
{
    double value;
};

struct VariableCall{
    std::string name;
};

struct FunctionCall{
    std::string name;
    std::vector<ExprPtr> arguments;
};

struct BoolLiteral
{
    bool value;
};

struct CharLiteral
{
    char value;
};

struct StringLiteral
{
    std::string value;
};




struct BinaryExpr
{
    ExprPtr left;
    ExprPtr right;
    TokenType op;
};

struct UnaryExpr
{
    ExprPtr operand;
    TokenType op;
};


struct Expr
{
    using Value = std::variant<
        IntLiteral,
        DoubleLiteral,
        BoolLiteral,
        CharLiteral,
        StringLiteral,
        VariableCall,
        FunctionCall,
        BinaryExpr,
        UnaryExpr
    >;

    Value value;

    template <typename T>
    Expr(T&& value)
        : value(std::forward<T>(value))
    {
    }
};



struct Stmt;


using StmtPtr = std::unique_ptr<Stmt>;

struct VariableDeclaration
{
    VarData vardata;
    ExprPtr initializer;
    std::string name;
};

struct Assignment
{
    ExprPtr value;
    std::string name;
    TokenType op;
};

struct ExpressionStatement
{
    ExprPtr expression;
};

struct ReturnStatement
{
    ExprPtr value;
};

struct IfStatement
{
    ExprPtr condition;
    std::vector<StmtPtr> body;
};

struct WhileStatement
{
    ExprPtr condition;
    std::vector<StmtPtr> body;
};



struct Stmt
{
    using Value = std::variant<
        VariableDeclaration,
        Assignment,
        ExpressionStatement,
        ReturnStatement,
        IfStatement,
        WhileStatement
    >;

    Value value;

    template <typename T>
    Stmt(T&& value)
        : value(std::forward<T>(value))
    {
    }
};



struct Program
{
    std::vector<StmtPtr> statements;
};



Program parse(const  std::vector<Token> & input);
#endif