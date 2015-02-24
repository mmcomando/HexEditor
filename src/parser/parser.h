#pragma once

#include <string>
#include <vector>
#include "expressions.h"
namespace Parser {
///enum for special tokens
enum TokenEnum {
    tok_chars=-2,
    tok_float=-3,
    tok_string=-4,
    tok_custom=-5,
    tok_out=-6,
    tok_integer=-7,
};
///enum for expression types
enum ExprEnum {
    expr_var,
    expr_float,
    expr_integer,
    expr_call,
    expr_func_def,
    expr_assigment,
    expr_chars,
    expr_deref_get,
    expr_deref_assigment,
    expr_array,
};
/**
 *   Container for tokens holds tokens types and its data
  *  Each token has its data under the same index as its own
*/
class TokenContainer {
    std::vector<void*> mData;
    std::vector<char> mTokens;
public:
    ~TokenContainer();
    void Clear();
    uint size();
    void Add(char token);
    void AddString(const std::string& str);
    void AddChars(const std::string& str);
    void AddFloat(double num);
    void AddInteger(long int num);
    char operator[] (uint numToken);
    std::string GetString(uint numToken);
    std::string GetChars(uint numToken);
    double GetFloat(uint numToken);
    long int GetInteger(uint numToken);
};
/**
 *   Tokenize file and generates expressions from tokens
 *   vectors exprData and expressions
*/
class ParserData {
public:
    ///Loads file and tokenizes it
    ParserData(const std::string& fileName);
    ParserData() {}
    ~ParserData();
    ///gets string and tokenizes it
    void Init(const std::string& str);
    void Clear();
    ///Generates expressions from tokens
    void Parse();
    ///Allows to print tokens to standard output
    void PrintTokens();
    ///Prints given expression with given intend
    void PrintExpr(uint num,int shift=0);
    void Print();

private:
    void ParseAssigmentExpr();
    void ParseFuncParameters(std::vector<uint>& parameters);
    void ParseDerefAssigmentExpr();
    void ParseDerefGetExpr();
    void ParseOutExpr(FuncDefinitionExpr* expr);
    void ParseFunctionDefinitionExpr();
    void ParseCallExpr();
public:
    TokenContainer tokens;
    ///expressions
    std::vector<Expr*> exprData;
    ///expressions tokens
    std::vector<ExprEnum> expressions;
    ///expressions to be executed
    std::vector<uint> exprToExecute;
private:
    uint cPT=0;//Current parsed token

    std::string nameFile;


};


}

