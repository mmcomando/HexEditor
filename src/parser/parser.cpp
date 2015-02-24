#include <fstream>
#include <string.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <stdlib.h>

#include "parser.h"
#include "exceptions.h"

using namespace Parser;;
using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;

void ParserData::Parse() {
    //for(uint numToken=0; numToken<tokens.size(); ++numToken) {
    //last is EOF
    if(tokens.size()!=0)
        while(cPT<tokens.size()-1) {
            if(tokens[cPT]==tok_string and tokens[cPT+1]=='=') {
                ParseAssigmentExpr();
                assert(expressions.size()==exprData.size());
                exprToExecute.push_back(expressions.size()-1);
            } else if(tokens[cPT]==tok_custom) {
                ParseFunctionDefinitionExpr();
                exprToExecute.push_back(expressions.size()-1);
            } else if(tokens[cPT]==tok_string and tokens[cPT+1]=='.' and tokens[cPT+2]==tok_string and tokens[cPT+3]=='=') {
                ParseDerefAssigmentExpr();
                exprToExecute.push_back(expressions.size()-1);
            }  else {
                throw ParseException {"Cant Parse this"};
            }
        }
}
void ParserData::ParseFuncParameters(vector<uint>& parameters) {
    uint tokNum=cPT;
    bool collon=false;
    do {
        if(collon and tokens[tokNum]!=',')throw ParseException {"Comma should be between parameters."};
        if(tokens[tokNum]==tok_string) {
            exprData.push_back(new VarExpr {tokNum});
            expressions.push_back(expr_var);
            parameters.push_back(expressions.size()-1);
        } else if(tokens[tokNum]==tok_float) {
            exprData.push_back(new FloatExpr {tokNum});
            expressions.push_back(expr_float);
            parameters.push_back(expressions.size()-1);
        }else if(tokens[tokNum]==tok_integer) {
            exprData.push_back(new IntegerExpr {tokNum});
            expressions.push_back(expr_integer);
            parameters.push_back(expressions.size()-1);
        } else if(tokens[tokNum]==',') {
        } else throw ParseException {"Expected name or float or integer"};
        tokNum++;
    } while(tokens[tokNum]!=')');
    if(tokens[tokNum-1]==',')throw ParseException {"Parameter list cant end with ','."};
    cPT=tokNum+1;
}
void ParserData::ParseCallExpr() {
    //name(name,88,...)
    CallExpr* expr=new CallExpr(cPT);
    cPT+=2;
    ParseFuncParameters(expr->mParameters);
    exprData.push_back(expr);
    expressions.push_back(expr_call);
}

void ParserData::ParseOutExpr(FuncDefinitionExpr* expr) {
    // out aa,bb,cc
    uint tokNum=cPT+1;
    bool collon=false;
    do {
        if(collon and tokens[tokNum]!=',')break;
        if(tokens[tokNum]==tok_string) {
            expr->returns.push_back(tokNum);
        } else if(tokens[tokNum]==',') {
        } else throw ParseException {"Expected name"};
        tokNum++;
        collon= !collon;
    } while(tokens[tokNum]==',' or tokens[tokNum]==tok_string);
    if(tokens[tokNum-1]==',')throw ParseException {"Outs list cant end with ','."};
    cPT=tokNum;
}
void ParserData::ParseFunctionDefinitionExpr() {
    //custom name(name,88,...){ expr1 expr2 }
    FuncDefinitionExpr* expr= new FuncDefinitionExpr(cPT+1);
    cPT+=3;
    ParseFuncParameters(expr->parameters);
    if(tokens[cPT]!='{')throw ParseException {"AAAAAAA"};
    cPT++;
    do {
        if(tokens[cPT]==tok_string and tokens[cPT+1]=='=') {
            ParseAssigmentExpr();
            expr->exprToExecute.push_back(expressions.size()-1);
        } else if(tokens[cPT]==tok_string and tokens[cPT+1]=='(') {
            ParseCallExpr();
            expr->exprToExecute.push_back(expressions.size()-1);
        } else if(tokens[cPT]==tok_out) {
            ParseOutExpr(expr);
        } else throw ParseException {"In function definition cant be such a thing"};
    } while(tokens[cPT]!='}');
    cPT++;
    exprData.push_back(expr);
    expressions.push_back(expr_func_def);
    //cout<<"FUNC DEFFFF PARSSSS"<<endl;
}
void ParserData::ParseDerefGetExpr() {
    // name.name
    exprData.push_back(new DerefGetExpr(cPT,cPT+2));
    expressions.push_back(expr_deref_get);
    cPT+=3;
}
void ParserData::ParseDerefAssigmentExpr() {
    //aa.aa=
    uint name=cPT;
    cPT+=2;
    ParseAssigmentExpr();
    exprData.push_back(new DerefAssigmentExpr {name,static_cast<uint>(expressions.size()-1)});
    expressions.push_back(expr_deref_assigment);
}
void ParserData::ParseAssigmentExpr() {
    //name=2.0
    if(tokens[cPT+2]==tok_float) {
        exprData.push_back(new FloatExpr {cPT+2});
        expressions.push_back(expr_float);
        exprData.push_back(new AssigmentExpr {cPT,static_cast<uint>(expressions.size()-1)});
        expressions.push_back(expr_assigment);
        cPT+=3;//eat 3 tokens, name=32.0
        return;
    }
    //name=2
    if(tokens[cPT+2]==tok_integer) {
        exprData.push_back(new IntegerExpr {cPT+2});
        expressions.push_back(expr_integer);
        exprData.push_back(new AssigmentExpr {cPT,static_cast<uint>(expressions.size()-1)});
        expressions.push_back(expr_assigment);
        cPT+=3;//eat 3 tokens, name=32
        return;
    }
    //name=name(
    if(tokens[cPT+2]==tok_string and tokens[cPT+3]=='(') {
        uint name=cPT;
        cPT+=2;//eat name=
        ParseCallExpr();
        if(tokens[cPT]=='[') {
            //name=name(..)[]
            exprData.push_back(new ArrayExpr {cPT+1,static_cast<uint>(expressions.size()-1)});
            expressions.push_back(expr_array);
            exprData.push_back(new AssigmentExpr {name,static_cast<uint>(expressions.size()-1)});
            expressions.push_back(expr_assigment);
            cPT+=3;
        } else {
            exprData.push_back(new AssigmentExpr {name,static_cast<uint>(expressions.size()-1)});
            expressions.push_back(expr_assigment);
        }
        return;
        //name=""
    }
    if(tokens[cPT+2]==tok_chars) {
        exprData.push_back(new VarExpr {cPT+2});
        expressions.push_back(expr_chars);
        exprData.push_back(new AssigmentExpr(cPT,expressions.size()-1));
        expressions.push_back(expr_assigment);
        cPT+=3;//we eat 3 tokens, name=name
        return;
        //name=name
    }
    if(tokens[cPT+2]==tok_string and tokens[cPT+3]=='.' and tokens[cPT+4]==tok_string) {
        uint name=cPT;
        cPT+=2;
        ParseDerefGetExpr();
        exprData.push_back(new AssigmentExpr(name,expressions.size()-1));
        expressions.push_back(expr_assigment);
        return;
    }
    if(tokens[cPT+2]==tok_string) {
        exprData.push_back(new VarExpr {cPT+2});
        expressions.push_back(expr_var);
        exprData.push_back(new AssigmentExpr(cPT,expressions.size()-1));
        expressions.push_back(expr_assigment);
        cPT+=3;//we eat 3 tokens, name=name
        return;
    }
    throw ParseException {"Couldn't parse assigment expression."};
}
ParserData::ParserData(const std::string& fileName):nameFile {fileName} {
    std::ifstream in;
    string mFile;
    in.exceptions(ifstream::failbit | ifstream::badbit);
    in.open(nameFile, std::ios::in);
    in.seekg(0, std::ios::end);
    mFile.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&mFile[0], mFile.size());
    in.close();
    Init(mFile);
}
void ParserData::Clear() {
    for(auto a:exprData)delete a;
    exprData.clear();
    expressions.clear();
    exprToExecute.clear();
    tokens.Clear();
    cPT=0;
}
ParserData::~ParserData() {
    Clear();
}
void ParserData::Init(const std::string& mFile) {
    for(uint numChar=0; numChar<mFile.size(); ++numChar) {
        char currChar=mFile[numChar];
        if(currChar=='"') {
            string str("");
            currChar=mFile[++numChar];
            while(currChar!='"') {// znaki dozwolone dla nazw
                str += currChar;
                currChar=mFile[++numChar];
            }
            currChar=mFile[++numChar];//eat "
            tokens.AddChars(str);
        }
        while(isspace(currChar))
            currChar=mFile[++numChar];
        if(isalpha(currChar)) {  // identifier: [a-zA-Z][a-zA-Z0-9]*
            string str;
            str+=currChar;
            currChar=mFile[++numChar];
            while(isalnum(currChar) or currChar=='_') {// znaki dozwolone dla nazw
                str += currChar;
                currChar=mFile[++numChar];
            }
            if(str=="custom")tokens.Add(tok_custom);
            else if(str=="out")tokens.Add(tok_out);
            else tokens.AddString(str);
            --numChar;// donk know what to do
            continue;
        }
        if(isdigit(currChar) or currChar=='-') {    // Number: [0-9]+
            std::string numStr;
            bool wasDot=false;
            do {
                if(currChar=='.')wasDot=true;
                numStr += currChar;
                currChar=mFile[++numChar];
            } while(isdigit(currChar) || (currChar == '.' and !wasDot));
            if(wasDot) { //float
                double num2;//strtod don't work somehow
                std::stringstream stream(numStr);
                stream >> num2;
                tokens.AddFloat(num2);
            } else {
                long int num = strtol(numStr.c_str(), 0,10);
                tokens.AddInteger(num);
            }


            --numChar;// last not touched
            continue;
        }
        if(currChar == '/' and mFile[numChar+1]=='/') {
            // Comment until end of line.
            do currChar=mFile[++numChar];
            while(currChar != EOF && currChar != '\n' && currChar != '\r');
            continue;
        }
        //some char maybe { or ( or somethin xD
        tokens.Add(currChar);
    }
}
void ParserData::PrintTokens() {
    for(uint numToken=0; numToken<tokens.size(); ++numToken) {
        char token=tokens[numToken];
        switch(token) {
        case tok_string:
            cout<<tokens.GetString(numToken)<<endl;
            break;
        case tok_chars:
            cout<<tokens.GetChars(numToken)<<endl;
            break;
        case tok_float:
            cout<<tokens.GetFloat(numToken)<<endl;
            break;
        default:
            cout<<token<<endl;
        }
    }
}
void ParserData::Print() {
    for(uint num:exprToExecute) {
        PrintExpr(num);
    }
}

static void PrintShift(int shift) {
    for(int i=0; i<shift; ++i)cout<<" ";
}
void ParserData::PrintExpr(uint num,int shift) {
    if(num>=expressions.size())throw ParseException {"Expr overflow :]"};
    switch(expressions[num]) {
    case expr_assigment: {
        AssigmentExpr* expr=dynamic_cast<AssigmentExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"AssigmentExpr: '"<<tokens.GetString(expr->name)<<"' ==="<<endl;
        PrintExpr(expr->expr,shift+4);
        PrintShift(shift);
        cout<<"==="<<endl;
        break;
    }
    case expr_call: {
        CallExpr* expr=dynamic_cast<CallExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"CallExpr: '"<<tokens.GetString(expr->name)<<"' ((("<<endl;
        for(uint par:expr->mParameters) {
            PrintExpr(par,shift+4);
        }
        PrintShift(shift);
        cout<<")))"<<endl;
        break;
    }
    case expr_float: {
        FloatExpr* expr=dynamic_cast<FloatExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"FloatExpr: "<<tokens.GetFloat(expr->num)<<endl;
        break;
    }
     case expr_integer: {
        IntegerExpr* expr=dynamic_cast<IntegerExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"IntegerExpr: "<<tokens.GetInteger(expr->num)<<endl;
        break;
    }
    case expr_var: {
        VarExpr* expr=dynamic_cast<VarExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"StringExpr: "<<tokens.GetString(expr->name)<<endl;
        break;
    }
    case expr_chars: {
        VarExpr* expr=dynamic_cast<VarExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"CharsExpr: "<<tokens.GetChars(expr->name)<<endl;
        break;
    }
    case expr_func_def: {
        FuncDefinitionExpr* expr=dynamic_cast<FuncDefinitionExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"FuncDefExpr: '"<<tokens.GetString(expr->name)<<"' ((("<<endl;
        for(uint par:expr->parameters) {
            PrintExpr(par,shift+4);
        }
        PrintShift(shift);
        cout<<"))){{{"<<endl;
        for(uint par:expr->exprToExecute) {
            PrintExpr(par,shift+4);
        }
        PrintShift(shift);
        cout<<"}}}out--"<<endl;
        for(uint par:expr->returns) {
            PrintShift(shift+4);
            cout<<tokens.GetString(par)<<endl;
        }
        PrintShift(shift);
        cout<<"--out"<<endl;
        break;
    }
    case expr_deref_get: {
        DerefGetExpr* expr=dynamic_cast<DerefGetExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"DerefGetExpr: '"<<tokens.GetString(expr->name)<<"."<<tokens.GetString(expr->derefName)<<"'"<<endl;
        break;
    }
    case expr_deref_assigment: {
        DerefAssigmentExpr* expr=dynamic_cast<DerefAssigmentExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"DerefAssigmentExpr: "<<tokens.GetString(expr->name)<<endl;
        PrintExpr(expr->assigmentExpr,shift+4);
        break;
    }
    case expr_array: {
        ArrayExpr* expr=dynamic_cast<ArrayExpr*>(exprData[num]);
        PrintShift(shift);
        cout<<"ArrayExpr: '[";
        if(tokens[expr->strOrFloat]==tok_float)
            cout<<tokens.GetFloat(expr->strOrFloat);
        else
            cout<<tokens.GetString(expr->strOrFloat);
        cout<<"]'"<<endl;
        PrintExpr(expr->callExpr,shift+4);
        break;
    }
    }
}



