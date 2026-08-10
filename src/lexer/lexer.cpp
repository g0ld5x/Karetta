#include "lexer.hpp"
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <iostream>
inline bool isCharacter(const char &ch)
{
    unsigned char lower = static_cast<unsigned char>(ch) | 0x20;
    return (static_cast<unsigned char>(lower - 'a') < 26) || (ch == '_');
}

std::vector<Token> lex(std::string_view input)
{
    std::vector<Token> tokens;
    for (size_t i = 0; i < input.size(); i++)
    {
        auto current = input[i];
        if (isCharacter(current))
        {
            Token token;
            std::string buffer;

            size_t k = 0;
            while (i + k < input.size() && (isCharacter(input[i + k]) || isdigit(input[i + k])))
            {
                buffer.push_back(input[i + k]);
                ++k;
            }
            if (buffer == "and")
            {
                token.type = TokenType::AndAnd;
            }
            else if (buffer == "int")
            {
                token.value = std::monostate();
                token.type = TokenType::Int;
            }
            else if (buffer == "isType")
            {
                token.type = TokenType::isType;
            }
            else if (buffer == "double")
            {
                token.value = std::monostate();
                token.type = TokenType::Double;
            }
            else if (buffer == "bool")
            {
                token.value = std::monostate();
                token.type = TokenType::Bool;
            }
            else if (buffer == "char")
            {
                token.value = std::monostate();

                token.type = TokenType::Char;
            }
            else if (buffer == "string")
            {
                token.value = std::monostate();

                token.type = TokenType::String;
            }
            else if (buffer == "true")
            {
                token.value = true;

                token.type = TokenType::Bool;
            }
            else if (buffer == "false")
            {
                token.value = false;
                token.type = TokenType::Bool;
            }
            else if (buffer == "is")
            {
                token.type = TokenType::EqualEqual;
            }
            else if (buffer == "in")
            {
                token.type = TokenType::InOperator;
            }
            else if (buffer == "or")
            {
                token.type = TokenType::OrOr;
            }
            else
            {
                token.type = TokenType::Identifier;
                token.value = buffer;
            }
            tokens.emplace_back(std::move(token));
            i += k - 1;
        }
        else if (current == '<')
        {
            Token token;
            if (input[i + 1] == '=')
            {
                ++i;
                token.type = TokenType::SmallerEqual;
                tokens.emplace_back(std::move(token));
                continue;
            }
            token.type = TokenType::Smaller;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '>')
        {
            Token token;
            if (input[i + 1] == '=')
            {
                ++i;
                token.type = TokenType::BiggerEqual;
                tokens.emplace_back(std::move(token));
                continue;
            }
            token.type = TokenType::Bigger;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '"')
        {
            Token token;
            std::string buffer;

            size_t k = 1;

            while (i + k < input.size() && input[i + k] != '"')
            {
                char currentChar = input[i + k];

                if (currentChar == '\\')
                {
                    if (i + k + 1 >= input.size())
                    {
                        throw std::runtime_error("Error: Incomplete escape sequence.");
                    }

                    char escape = input[i + k + 1];

                    switch (escape)
                    {
                    case 'n':
                        buffer.push_back('\n');
                        break;

                    case 't':
                        buffer.push_back('\t');
                        break;

                    case 'r':
                        buffer.push_back('\r');
                        break;

                    case '\\':
                        buffer.push_back('\\');
                        break;

                    case '"':
                        buffer.push_back('"');
                        break;
                    case '\'':
                        buffer.push_back('\'');
                        break;
                    default:
                        throw std::runtime_error("Error: Unknown escape sequence.");
                    }

                    k += 2;
                    continue;
                }

                buffer.push_back(currentChar);
                ++k;
            }

            if (i + k >= input.size())
            {
                throw std::runtime_error("Error: String never finished.");
            }

            token.type = TokenType::String;
            token.value = std::move(buffer);

            tokens.emplace_back(std::move(token));

            i += k;
        }
        else if (current == '%')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                token.type = TokenType::CompoundModulo;
                ++i;
            }
            else
            {
                token.type = TokenType::Modulo;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '+')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '+')
            {
                token.type = TokenType::Increment;
                ++i;
            }
            else if (i + 1 < input.size() && input[i + 1] == '=')
            {
                token.type = TokenType::CompoundPlus;
                ++i;
            }
            else
            {
                token.type = TokenType::Add;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '-')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '-')
            {
                token.type = TokenType::Decrement;
                ++i;
            }
            else if (i + 1 < input.size() && input[i + 1] == '=')
            {
                token.type = TokenType::CompoundMinus;
                ++i;
            }
            else
            {
                token.type = TokenType::Minus;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '=')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                token.type = TokenType::EqualEqual;
                ++i;
                if (i + 1 < input.size() && input[i + 1] == '=')
                {
                    token.type = TokenType::isType;
                    ++i;
                }
            }
            else
            {
                token.type = TokenType::Equal;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '<')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                token.type = TokenType::SmallerEqual;
                ++i;
            }
            else if (i + 1 < input.size() && input[i + 1] == '<')
            {
                token.type = TokenType::BitLeft;
                ++i;
            }
            else
            {
                token.type = TokenType::Smaller;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '>')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                token.type = TokenType::BiggerEqual;
                ++i;
            }
            else if (i + 1 < input.size() && input[i + 1] == '<')
            {
                token.type = TokenType::BitRight;
                ++i;
            }
            else
            {
                token.type = TokenType::Bigger;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '!')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '=')
            {
                token.type = TokenType::NotEqual;
                ++i;
            }
            else
            {
                token.type = TokenType::UnaryNot;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '*')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '*')
            {
                token.type = TokenType::Exponentiation;
                ++i;
            }
            else
            {
                token.type = TokenType::Multiply;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '&')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '*')
            {
                token.type = TokenType::BitAnd;
                ++i;
            }
            else
            {
                token.type = TokenType::AndAnd;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '|')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '*')
            {
                token.type = TokenType::BitOr;
                ++i;
            }
            else
            {
                token.type = TokenType::OrOr;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (current == '~')
        {
            Token token;
            token.type = TokenType::BitXor;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '/')
        {
            Token token;

            if (i + 1 < input.size() && input[i + 1] == '/')
            {
                i += 2;

                while (i < input.size() && input[i] != '\n')
                    ++i;

                continue;
            }
            else if (input[i] == '=')
            {
                token.type = TokenType::CompoundDivision;
                i++;
            }
            else
            {
                token.type = TokenType::Divide;
            }

            tokens.emplace_back(std::move(token));
        }
        else if (std::isdigit(static_cast<unsigned char>(current)))
        {
            Token token;
            token.type = TokenType::Int;

            int value = 0;
            size_t k = 0;

            while (i + k < input.size() &&
                   std::isdigit(static_cast<unsigned char>(input[i + k])))
            {
                value = value * 10 + (input[i + k] - '0');
                ++k;
            }

            if (i + k < input.size() && input[i + k] == '.')
            {
                token.type = TokenType::Double;

                double doubleValue = value;
                ++k;

                double place = 0.1;

                while (i + k < input.size() &&
                       std::isdigit(static_cast<unsigned char>(input[i + k])))
                {
                    doubleValue += (input[i + k] - '0') * place;
                    place *= 0.1;
                    ++k;
                }

                token.value = doubleValue;
            }
            else
            {
                token.value = value;
            }

            tokens.emplace_back(std::move(token));

            i += k - 1;
        }

        else if (current == '.')
        {
            Token token;
            token.type = TokenType::Dot;
            tokens.emplace_back(std::move(token));
        }
        else if (current == ':')
        {
            Token token;
            token.type = TokenType::InOperator;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '(')
        {
            Token token;
            token.type = TokenType::OpenParan;
            tokens.emplace_back(std::move(token));
        }
        else if (current == ')')
        {
            Token token;
            token.type = TokenType::CloseParan;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '{')
        {
            Token token;
            token.type = TokenType::OpenCurl;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '}')
        {
            Token token;
            token.type = TokenType::CloseCurl;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '[')
        {
            Token token;
            token.type = TokenType::OpenBracket;
            tokens.emplace_back(std::move(token));
        }
        else if (current == ']')
        {
            Token token;
            token.type = TokenType::CloseBracket;
            tokens.emplace_back(std::move(token));
        }
        else if (current == '\'')
        {
            Token token;
            token.type = TokenType::Char;

            ++i;

            if (i >= input.size())
                throw std::runtime_error("Error: Char never finished.");

            char value;

            if (input[i] == '\\')
            {
                if (i + 1 >= input.size())
                    throw std::runtime_error("Error: Incomplete escape sequence.");

                ++i;

                switch (input[i])
                {
                case 'n':
                    value = '\n';
                    break;

                case 't':
                    value = '\t';
                    break;

                case 'r':
                    value = '\r';
                    break;

                case '\\':
                    value = '\\';
                    break;

                case '"':
                    value = '"';
                    break;

                case '\'':
                    value = '\'';
                    break;

                default:
                    throw std::runtime_error("Error: Unknown escape sequence.");
                }

                ++i;
            }
            else
            {
                value = input[i];
                ++i;
            }

            if (i >= input.size() || input[i] != '\'')
                throw std::runtime_error(
                    "Error: Character literal must contain exactly one character.");

            token.value = value;

            tokens.emplace_back(std::move(token));
        }
    }
    Token token;
    token.type = TokenType::EndOfFile;
    tokens.emplace_back(std::move(token));

    return tokens;
}
