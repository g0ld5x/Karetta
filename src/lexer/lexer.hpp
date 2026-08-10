#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
#include <variant>
#include <memory>


struct ArrayValue;

using ArrayPtr = std::shared_ptr<ArrayValue>;


using Value = std::variant<
    std::monostate,
    int,
    double,
    char,
    bool,
    std::string,
    ArrayPtr
>;



struct ArrayValue {
    std::vector<Value> values;
};
enum class TokenType
{
    Identifier, //if,else,for,while     --done
    Int, //1,2,3,10,422,13214,int       --done
    Double,//1.23,1.1,0.103,double      --done
    Char, //'A','b','\n',char           --done
    Bool, // true,false,null            --done
    AndAnd, // && , and                 --done
    OrOr, // || , or                    --done
    Dot, // .                           --done
    Modulo, // %                        --done
    Exponentiation, // **               --done
    Add, // +                           --done
    Minus, // -                         --done
    Divide, // this --> /               --done
    Multiply, // *                      --done
    String, // "hello!"                 --done
    OpenParan, // (                     --done
    CloseParan, // )                    --done
    OpenCurl,// {                       --done
    CloseCurl, // }                     --done
    OpenBracket, // [                   --done
    CloseBracket, // ]                  --done
    EqualEqual, // ==, is               --done
    Increment, // ++                    --done
    Decrement, // --                    --done
    CompoundPlus, // +=                 --done
    CompoundMinus, // -=                --done
    CompoundMultiply, // *=             --done
    CompoundDivision, // /=             --done
    CompoundModulo, // %=               --done
    Bigger, // >                        --done
    Smaller, // <                       --done
    NotEqual, // !=                     --done
    UnaryNot, // !                      --done
    BiggerEqual, // >=                  --done
    SmallerEqual, // <=                 --done
    isType, // ===,isType               --done
    Comma,//  ,                         --done
    Equal, // =                         --done
    BitLeft, // <<                      --done
    BitRight, // >>                     --done
    BitNot, // ~                        --done
    BitOr, // |                         --done
    BitAnd, // &                        --done
    BitXor, // ^                        --done
    StatementEnd, // Newline or ;       --done
    EndOfFile, //                       --done
    InOperator, // : or in              --done
};

struct Token
{
    TokenType type;
    Value value;
    int line;
    int column;
};
std::vector<Token> lex(std::string_view input);

#endif