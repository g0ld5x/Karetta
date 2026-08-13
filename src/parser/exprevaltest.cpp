#include "../lexer/lexer.hpp"
#include "parser.hpp"
#include <iostream>

std::string tokenTypeToString(TokenType type)
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
    case TokenType::isType:
        return "isType";

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

void printExpr(const ExprPtr &expr, int depth = 0)
{
    std::string indent(depth * 2, ' ');

    std::visit([&](const auto &node)
               {
                   using T = std::decay_t<decltype(node)>;

                   if constexpr (std::is_same_v<T, IntLiteral>)
                   {
                       std::cout << indent << "Int: "
                                 << node.value << '\n';
                   }
                   else if constexpr (std::is_same_v<T, DoubleLiteral>)
                   {
                       std::cout << indent << "Double: "
                                 << node.value << '\n';
                   }
                   else if constexpr (std::is_same_v<T, IndexExpr>)
                   {
                       std::cout << indent << "IndexExpression:\n";

                       std::cout << indent << "  Object:\n";
                       if (node.object)
                           printExpr(node.object, depth + 2);
                       else
                           std::cout << indent << "    <null>\n";

                       std::cout << indent << "  Index:\n";
                       if (node.index)
                           printExpr(node.index, depth + 2);
                       else
                           std::cout << indent << "    <null>\n";
                   }
                   else if constexpr (std::is_same_v<T, BoolLiteral>)
                   {
                       std::cout << indent << "Bool: "
                                 << node.value << '\n';
                   }
                   else if constexpr (std::is_same_v<T, CharLiteral>)
                   {
                       std::cout << indent << "Char: "
                                 << node.value << '\n';
                   }
                   else if constexpr (std::is_same_v<T, StringLiteral>)
                   {
                       std::cout << indent << "String: "
                                 << node.value << '\n';
                   }
                   else if constexpr (std::is_same_v<T, ArrayLiteral>)
                   {
                       std::cout << indent << "Array:\n";

                       for (const auto &element : node.elements)
                       {
                           printExpr(element, depth + 1);
                       }
                   }
                   else if constexpr (std::is_same_v<T, VariableCall>)
                   {
                       std::cout << indent << "Variable: "
                                 << node.name << '\n';
                   }
                   else if constexpr (std::is_same_v<T, FunctionCall>)
                   {
                       std::cout << indent << "Function: "
                                 << node.name << '\n';

                       for (size_t i = 0; i < node.arguments.size(); ++i)
                       {
                           std::cout << indent << "  Argument " << i << ":\n";
                           printExpr(node.arguments[i], depth + 2);
                       }
                   }
                   else if constexpr (std::is_same_v<T, BinaryExpr>)
                   {
                       std::cout << indent << "Binary: "
                                 << tokenTypeToString(node.op) << '\n';
                       printExpr(node.left, depth + 1);
                       printExpr(node.right, depth + 1);
                   }
                   else if constexpr (std::is_same_v<T, UnaryExpr>)
                   {
                       std::cout << indent << "Unary " << tokenTypeToString(node.op) << "\n";
                       printExpr(node.operand, depth + 1);
                   }
               },
               expr->value);
}

void printProgram(const Program &program)
{
    for (size_t i = 0; i < program.statements.size(); ++i)
    {
        std::cout << "Statement " << i << ":\n";

        const auto &stmt = program.statements[i];

        if (!stmt)
        {
            std::cout << "  <null>\n";
            continue;
        }

        std::visit([](const auto &node)
                   {
                       using T = std::decay_t<decltype(node)>;

                       if constexpr (std::is_same_v<T, VariableDeclaration>)
                       {
                           std::cout << "  VariableDeclaration\n";
                           std::cout << "    Name: "
                                     << node.name << '\n';

                           std::cout << "    Const: "
                                     << std::boolalpha
                                     << node.vardata.isConst << '\n';

                           std::cout << "    MonoType: "
                                     << std::boolalpha
                                     << node.vardata.isMonoType << '\n';
                           std::cout << "    Global: "
                                     << node.vardata.isGlobal << '\n';

                           std::cout << "    Strict: "
                                     << node.vardata.isStrict << '\n';

                           std::cout << "    Initializer:\n";

                           if (node.initializer)
                           {
                               printExpr(node.initializer, 3);
                           }
                           else
                           {
                               std::cout << "      <none>\n";
                           }
                       }
                       else if constexpr (std::is_same_v<T, Assignment>)
                       {
                           std::cout << "  " << "Assignment:\n";

                           std::cout << "    " << "  Target:\n";
                           printExpr(node.target, 3);

                           std::cout << "    " << "  Value:\n";
                           printExpr(node.value, 3);
                       }
                       else if constexpr (std::is_same_v<T, ImportStatement>)
                       {
                           std::cout << "  Import: \n";
                           std::cout << "     Path:" << node.pathS[0] << "\n";
                       }

                       else if constexpr (std::is_same_v<T, ExpressionStatement>)
                       {
                           std::cout << "  ExpressionStatement: \n";
                           std::cout << "    Expression: ";
                           printExpr(node.expression);
                       }
                       else if constexpr (std::is_same_v<T, ReturnStatement>)
                       {
                           std::cout << "  Return: \n";
                           std::cout << "    Expression: ";
                           if (node.value == nullptr)
                           {
                               std::cout << "      <none>\n";
                           }
                           else
                           {
                               printExpr(node.value);
                           }
                       }
                       else if constexpr (std::is_same_v<T, FunctionDeclaration>)
                       {
                           std::cout << "    Function Declaration: \n";
                           std::cout << "      Name: " << node.name << "\n";
                           for (auto &&i : node.parameters)
                           {
                               std::cout << "Parameters: " << "       " << i << "\n";
                           }
                           std::cout << "      Body: ";
                           printProgram(node.body);
                       }
                       else if constexpr (std::is_same_v<T,IfStatement>){
                        std::cout << "      If Statement: \n Condition:";
                        printExpr(node.condition);
                        std::cout<< "Body: \n";
                        printProgram(node.body);
                       }
                       else if constexpr (std::is_same_v<T,WhileStatement>){
                        std::cout << "      While Statement: \n Condition:";
                        printExpr(node.condition);
                        std::cout<< "Body: \n";
                        printProgram(node.body);
                       }
                       else if constexpr (std::is_same_v<T,ForStatement>){
                        std::cout << "      For Statement: \n Variable Name:  " << node.variableName<< "\n";
                        std::cout << "Iterable: "; printExpr(node.iterable);
                        std::cout << "\n Body: "; printProgram(node.body);
                        
                       }
                       //add more statement types here later.
                   },
                   stmt->value);
    }
}

int main()
{
    std::string source;
    while (true)
    {
        source = "";
        std::getline(std::cin, source);

        auto tokens = lex(source);

        size_t pos = 0;

        Program expr = parse(tokens);

        printProgram(expr);
    }
}