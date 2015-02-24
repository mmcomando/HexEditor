#pragma once
#include <iostream>

#include <vector>
#include <string>
namespace Parser {

class ParserData;
/** Expr is a base class for all Expressions required by parser
 * to provide code logic
 * Expressions don't hold their data exacly but by index to corresponding data in ParserData class
*/
struct Expr {
    virtual ~Expr() {}
};
///Expression contain name of function or variable
struct VarExpr:public Expr {
    uint name;
    VarExpr(uint name_):name(name_) {}
};
///Expression contain floating point number
struct FloatExpr:public Expr {
    uint num;
    FloatExpr(uint num_):num(num_) {}
};
///Expression contain integral number
struct IntegerExpr:public Expr {
    uint num;
    IntegerExpr(uint num_):num(num_) {}
};
///Expression contain function name and it's parameters
struct CallExpr:public Expr {
    uint name;
    std::vector<uint> mParameters;
    CallExpr(uint name_):name(name_) {}
};
///Expression contain variable name and expression assigned to it
struct AssigmentExpr:public Expr {
    uint name;//TokNum
    uint expr;//ExprNum
    AssigmentExpr(uint name_,uint expr_):name(name_),expr(expr_) {}
};
///Expression contain function name, it's parameters and contained expressions
struct FuncDefinitionExpr:public Expr {
    uint name;//TokNum
    std::vector<uint> parameters;
    std::vector<uint> returns;
    std::vector<uint> exprToExecute;
    FuncDefinitionExpr(uint name_):name(name_) {}
};
///Expression contain variable name and it's element name
struct DerefGetExpr:public Expr {
    uint name;//TokNum
    uint derefName;//TokNum
    DerefGetExpr(uint name_,uint derefName_):name(name_),derefName(derefName_) {}
};
///Expression allows to assign dereferenced variable to another variable
struct DerefAssigmentExpr:public Expr {
    ///variable name
    uint name;//TokNum
    uint assigmentExpr;//ExprNum
    DerefAssigmentExpr(uint name_,uint expr):name(name_),assigmentExpr(expr) {}
};
///Expression represents array
struct ArrayExpr:public Expr {
    ///Number of array elements
    uint strOrFloat;//TokNum
    uint callExpr;//ExprNum
    ArrayExpr(uint d,uint expr_):strOrFloat(d),callExpr(expr_) {}
};

}
