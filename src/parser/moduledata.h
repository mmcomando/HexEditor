#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "parser.h"
struct Var;
struct Variable {
    std::shared_ptr<Var> mVar;
};
/**
Var is a base class for classes which are representing type and data loaded from binary data
*/
struct Var {
    virtual ~Var() {}
    ///allows printing Var content to standard output
    virtual void Print()=0;
    ///Makes a copy of given Var
    virtual Var* Copy()=0;
};
///Contains loaded floating point number
struct Float:Var {
    double num;
    Float(double num_):num(num_) {}
    virtual void Print();
    virtual Var* Copy() {
        return new Float(num);
    };
};
///Contains loaded integer number
struct Integer:Var {
    long int num;
    Integer(long num_):num(num_) {}
    virtual void Print();
    virtual Var* Copy() {
        return new Integer(num);
    };
};
///Contains loaded string
struct String:Var {
    std::string mStr;
    String(std::string str):mStr(str) {}
    virtual void Print();
    virtual Var* Copy() {
        return new String(mStr);
    };
};
///Contains combined Vars
struct Custom:Var {
    std::vector<std::pair<std::string,Variable>> vars;
    virtual void Print();
    virtual Var* Copy() {
        Custom* var=new Custom;
        var->vars=vars;
        return var;
    };
};

/**
ModuleData interprets and executes Expressions(Expr) from ParserData and holds loaded data
*/
class ModuleData {
public:
    /**
    Represents namescpace for variables and function used as a global namespace and as functions namespace
    */
    struct NameSpace {
        NameSpace(NameSpace* nms=nullptr):mParent(nms) {}
        void AddVariable(const std::string& str,std::shared_ptr<Var>);
        void AddFunc(const std::string& str,uint func);
        void PrintVariables();
        ///Functions definded in namespace
        std::vector<std::pair<std::string,uint>> definedFunctions;
        ///Variables definded in namespace
        std::vector<std::pair<std::string,Variable>> variables;
        ///Parent namespace
        NameSpace* mParent;
    };
    /// Checks if function with given name is build in if so executes it and return true otherwise returns false
    bool BuildInFunction(const std::string& name);
    ///Clears all data. Used before working on different data
    void Clear();
    ///Executes user function
    void ComputeFuncDef(Parser::ParserData& data,NameSpace& nms,uint exprNum);
    ///Executes given expression
    void Compute(Parser::ParserData& data,NameSpace& nms,uint exprNum);
    void ComputeAll(Parser::ParserData& data,NameSpace& nms);
    /// Returns variable from given namespace
    std::shared_ptr<Var> GetVar(NameSpace& nms,const std::string& str);
    /// Returns function expression index from given namespace
    uint GetFunc(NameSpace& nms,const std::string& str);

    NameSpace globalNameSpace;
    /// hold parts of data which was interpreted
    std::vector<std::pair<uint,uint>> usedBlocks;
    ///Data on which is working interpreter
    char* data;
    uint dataSize;
private:
    std::vector<std::shared_ptr<Var>> parStack;
    std::shared_ptr<Var> retStack;


};


