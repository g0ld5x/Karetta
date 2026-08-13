#ifndef AST_HPP
#define AST_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "../lexer/lexer.hpp"

// ============================================================
// Forward Declarations
// ============================================================

struct Expr;
struct Stmt;
struct Program;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;


// ============================================================
// Variable / Type Information
// ============================================================

enum class VarType : uint8_t
{
    Integer,
    Double,
    String,
    Char,
    Bool,
    Array
};

struct VarData
{
    bool isStrict;
    VarType variableType;
    bool isGlobal;
    bool isConst;
    bool isMonoType;
};

struct Variable
{
    Value value;
    VarData data;
};

using VariableTable = std::unordered_map<std::string, Variable>;
using ScopeStack = std::vector<VariableTable>;


// ============================================================
// Expressions
// ============================================================

struct IntLiteral
{
    int value;
};

struct DoubleLiteral
{
    double value;
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

struct VariableCall
{
    std::string name;
};

struct IndexExpr
{
    ExprPtr object;
    ExprPtr index;
};

struct ArrayLiteral
{
    std::vector<ExprPtr> elements;
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

struct FunctionCall
{
    std::string name;
    std::vector<ExprPtr> arguments;
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
        ArrayLiteral,
        IndexExpr,
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


// ============================================================
// Program
// ============================================================

struct Program
{
    std::vector<StmtPtr> statements;

};


// ============================================================
// Statements
// ============================================================

struct VariableDeclaration
{
    VarData vardata;
    ExprPtr initializer;
    std::string name;
};

struct Assignment
{
    ExprPtr value;
    ExprPtr target;
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


// ------------------------------------------------------------
// Control Flow
// ------------------------------------------------------------

struct BreakStatement
{
};

struct ContinueStatement
{
};

struct IfStatement
{
    ExprPtr condition;
    Program body;
};

struct WhileStatement
{
    ExprPtr condition;
    Program body;
};


// ------------------------------------------------------------
// Functions
// ------------------------------------------------------------

struct FunctionDeclaration
{
    std::string name;
    std::vector<std::string> parameters;
    Program body;
};


// ------------------------------------------------------------
// Imports
// ------------------------------------------------------------

struct ImportStatement
{
    std::vector<std::string> pathS;
};


// ============================================================
// Statement Container
// ============================================================

struct Stmt
{
    using Value = std::variant<
        VariableDeclaration,
        FunctionDeclaration,
        BreakStatement,
        ContinueStatement,
        Assignment,
        ExpressionStatement,
        ReturnStatement,
        IfStatement,
        WhileStatement,
        ImportStatement
    >;

    Value value;

    template <typename T>
    Stmt(T&& value)
        : value(std::forward<T>(value))
    {
    }
};


// Program destructor must be defined after Stmt is complete.



// ============================================================
// Parser
// ============================================================

ExprPtr parseExpression(
    const std::vector<Token>& tokens,
    size_t& pos,
    int minBindPower
);

Program parse(const std::vector<Token>& input);

#endif