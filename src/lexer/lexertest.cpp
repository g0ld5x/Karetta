#include <iostream>
#include "lexer.hpp"
#include <vector>
#include <string>
#include <sstream>
std::string variantToString(const Value& a);

std::string arrayToString(const std::shared_ptr<ArrayValue> &array)
{
    std::string result = "[";

    for (size_t i = 0; i < array->values.size(); i++)
    {
        result += variantToString(array->values[i]);

        if (i != array->values.size() - 1)
            result += ",";
    }

    result += "]";

    return result;
}




std::string variantToString(const Value& a)
{
    return std::visit([](const auto& arg) -> std::string
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
        }
    }, a);
}



std::string tokenTypeToString2(TokenType type)
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

int main(){
    while(true){
        std::string code = "";
        std::getline(std::cin,code);

        std::vector<Token> lexed = lex(code);

        for (auto &i : lexed)
        {
            std::cout << tokenTypeToString2(i.type) << "\n";
        
    }
}
}