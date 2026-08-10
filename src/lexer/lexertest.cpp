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

int main(){
    while(true){
        std::string code = "";
        std::getline(std::cin,code);

        std::vector<Token> lexed = lex(code);

        for (auto &i : lexed)
        {
            if(i.type == TokenType::Identifier){
                std::cout << "Identifier(" << variantToString(i.value) << ")";
            }
            if(i.type == TokenType::Int){
                std::cout << "Int(" << variantToString(i.value) << ")";
            }
            if(i.type == TokenType::Double){
                std::cout << "Double(" << variantToString(i.value) << ")";
            }
            if(i.type == TokenType::String){
                std::cout << "String(" << variantToString(i.value) << ")";
            }
            if(i.type == TokenType::Char){
                std::cout << "Char(" << variantToString(i.value) << ")";
            }
            if(i.type == TokenType::Bool){
                std::cout << "Bool(" << variantToString(i.value) << ")";
            }
            std::cout << "\n";
        }
        
    }
}