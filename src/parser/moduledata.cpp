#include "moduledata.h"
#include <algorithm>
using namespace std;
using namespace Parser;
void ModuleData::ComputeAll(Parser::ParserData& data,NameSpace& nms) {
    for(uint num:data.exprToExecute)Compute(data,nms,num);
}
void ModuleData::Clear() {
    globalNameSpace=NameSpace();
    usedBlocks.clear();
    parStack.clear();
    retStack=nullptr;
    data=nullptr;
}
bool ModuleData::BuildInFunction(const string& name) {
    //TODO to rework, don't scale
    if(name=="uint4") { //!find system defined function
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        if(parStack.size()!=1 or fl==nullptr)throw string("uint4 takes only 1 int parameter.");
        uint num=static_cast<uint>(fl->num);
        if(num+4>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(num,num+4));
        fl->num+=4;
        uint oo=*(unsigned int*)(data+num);
        Integer* aa=new Integer(oo);
        retStack=shared_ptr<Var>(aa);
    } else if(name=="uint2") {
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        if(parStack.size()!=1 or fl==nullptr)throw string("uint4 takes only 1 int parameter.");
        uint num=static_cast<uint>(fl->num);
        if(num+2>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(num,num+2));
        fl->num+=2;
        unsigned short oo=*(unsigned short*)(data+num);
        Integer* aa=new Integer(oo);
        retStack=shared_ptr<Var>(aa);
    } else if(name=="uint1") {
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        if(parStack.size()!=1 or fl==nullptr)throw string("uint4 takes only 1 int parameter.");
        uint num=static_cast<uint>(fl->num);
        if(num+1>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(num,num+1));
        fl->num+=1;
        unsigned char oo=*(unsigned char*)(data+num);
        Integer* aa=new Integer(oo);
        retStack=shared_ptr<Var>(aa);
    } else if(name=="int4") {
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        if(parStack.size()!=1 or fl==nullptr)throw string("uint4 takes only 1 int parameter.");
        uint num=static_cast<uint>(fl->num);
        if(num+4>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(num,num+4));
        fl->num+=4;
        int oo=*(int*)(data+num);
        Integer* aa=new Integer(oo);
        retStack=shared_ptr<Var>(aa);
    } else if(name=="int2") {
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        if(parStack.size()!=1 or fl==nullptr)throw string("uint4 takes only 1 int parameter.");
        uint num=static_cast<uint>(fl->num);
        if(num+2>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(num,num+2));
        fl->num+=2;
        short oo=*(short*)(data+num);
        Integer* aa=new Integer(oo);
        retStack=shared_ptr<Var>(aa);
    } else if(name=="int1") {
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        if(parStack.size()!=1 or fl==nullptr)throw string("uint4 takes only 1 int parameter.");
        uint num=static_cast<uint>(fl->num);
        if(num+1>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(num,num+1));
        fl->num+=1;
        char oo=*(char*)(data+num);
        Integer* aa=new Integer(oo);
        retStack=shared_ptr<Var>(aa);
    } else if(name=="float") {
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        if(parStack.size()!=1 or fl==nullptr)throw string("uint4 takes only 1 int parameter.");
        uint num=static_cast<uint>(fl->num);
        if(num+4>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(num,num+4));
        fl->num+=4;
        float oo=*(float*)(data+num);
        Float* aa=new Float(oo);
        retStack=shared_ptr<Var>(aa);
    }


    else if(name=="add") {
        Float* fl=dynamic_cast<Float*>(parStack[0].get());
        Float* fl2=dynamic_cast<Float*>(parStack[1].get());
        if(parStack.size()!=2 or fl==nullptr or fl2==nullptr)throw string("add function takes only 2 float parameters.");
        Float* aa=new Float(fl->num+fl2->num);
        retStack=shared_ptr<Var>(aa);
    } else if(name=="addto") {
        if(parStack.size()!=2 )throw string("addto takes only 2 parameters.");
        Integer* nt=dynamic_cast<Integer*>(parStack[0].get());
        Float* fl=dynamic_cast<Float*>(parStack[0].get());
        if(fl!=nullptr){
            Float* fl2=dynamic_cast<Float*>(parStack[1].get());
            if(fl2==nullptr)throw string("second argument have to be float as first in add function.");
            fl->num+=fl2->num;
        }else if(nt!=nullptr){
            Integer* nt2=dynamic_cast<Integer*>(parStack[1].get());
            if(nt2==nullptr)throw string("second argument have to be integr as first in add function.");
            nt->num+=nt2->num;
        }else{
            throw string("Expected float or integer as a parameter for add function");
        }


    } else if(name=="string") {
        Integer* fl=dynamic_cast<Integer*>(parStack[0].get());
        Integer* fl2=dynamic_cast<Integer*>(parStack[1].get());
        if(parStack.size()!=2 or fl==nullptr or fl2==nullptr)throw string("string takes only 2 Integer parameters.");
        string str;
        uint a1=fl->num;
        uint a2=fl2->num;
        if(a1+a2>dataSize)throw string("Data overflow.");
        usedBlocks.push_back(make_pair(a1,a1+a2));
        str.append(data+a1,a2);
        String* aa=new String(str);
        retStack=shared_ptr<Var>(aa);
    } else {
        return false;
    }
    return true;
}

void ModuleData::Compute(Parser::ParserData& data,NameSpace& nms,uint exprNum) {
    switch(data.expressions[exprNum]) {
    case expr_assigment: {
        AssigmentExpr* expr=dynamic_cast<AssigmentExpr*>(data.exprData[exprNum]);
        Compute(data,nms,expr->expr);
        string name=data.tokens.GetString(expr->name);
        nms.AddVariable(data.tokens.GetString(expr->name),retStack);
        retStack=nullptr;
        break;
    }
    case expr_call: {
        CallExpr* expr=dynamic_cast<CallExpr*>(data.exprData[exprNum]);
        if(retStack!=nullptr)throw string("Stack isn't empty while calling a function.");//TODO shold be caught internally
        if(!parStack.empty())throw string("Parameter stack should be empty before calling a function.");// --
        for(uint par:expr->mParameters) {
            Compute(data,nms,par);
            if(retStack!=nullptr)
                parStack.push_back(retStack);
            retStack=nullptr;
        }
        retStack=nullptr;
        string funcName=data.tokens.GetString(expr->name);
        if(!BuildInFunction(funcName)) {
            ComputeFuncDef(data,nms,GetFunc(nms,funcName));
        }
        parStack.clear();
        //Dont clear  retStack, leave it for assigment expr
        break;
    }
    case expr_float: {
        FloatExpr* expr=dynamic_cast<FloatExpr*>(data.exprData[exprNum]);
        retStack=shared_ptr<Var>(new Float(data.tokens.GetFloat(expr->num)));
        break;
    }
    case expr_integer: {
        IntegerExpr* expr=dynamic_cast<IntegerExpr*>(data.exprData[exprNum]);
        retStack=shared_ptr<Var>(new Integer(data.tokens.GetInteger(expr->num)));
        break;
    }
    case expr_var: {
        VarExpr* expr=dynamic_cast<VarExpr*>(data.exprData[exprNum]);
        shared_ptr<Var> var=GetVar(nms,data.tokens.GetString(expr->name));
        string nam=data.tokens.GetString(expr->name);
        if(var==nullptr)throw string("Variable with such a name dont exist");
        retStack=shared_ptr<Var>(var);
        break;
    }
    case expr_func_def: { //only register function
        FuncDefinitionExpr* expr=dynamic_cast<FuncDefinitionExpr*>(data.exprData[exprNum]);
        nms.AddFunc(data.tokens.GetString(expr->name),exprNum);
        break;
    }
    case expr_chars: {
        VarExpr* expr=dynamic_cast<VarExpr*>(data.exprData[exprNum]);
        String* var=new String(data.tokens.GetChars(expr->name));
        retStack=shared_ptr<Var>(var);
        break;
    }
    case expr_deref_get: {
        DerefGetExpr* expr=dynamic_cast<DerefGetExpr*>(data.exprData[exprNum]);
        shared_ptr<Custom> custom=dynamic_pointer_cast<Custom>(GetVar(nms,data.tokens.GetString(expr->name)));
        if(custom==nullptr)throw string("Can dereference only from custom");
        string str=data.tokens.GetString(expr->derefName);
        auto it=find_if(custom->vars.begin(),custom->vars.end(),
        [&str](const pair<string,Variable>& p) {
            return (str==p.first);
        }
                       );
        if(it==custom->vars.end())throw string("Custom dont hava such a variable");
        if(retStack!=nullptr)throw string("RerStack should be clear");
        //Var* vvvvv=it->second.mVar;//TODO aa
        retStack=it->second.mVar;
        break;
    }
    case expr_deref_assigment: {
        DerefAssigmentExpr* expr=dynamic_cast<DerefAssigmentExpr*>(data.exprData[exprNum]);
        AssigmentExpr* exprAssi=dynamic_cast<AssigmentExpr*>(data.exprData[expr->assigmentExpr]);
        Compute(data,nms,exprAssi->expr);
        shared_ptr<Custom> custom=dynamic_pointer_cast<Custom>(GetVar(nms,data.tokens.GetString(expr->name)));
        if(custom==nullptr)throw string("Can dereference only from custom");
        string str=data.tokens.GetString(exprAssi->name);
        auto it=find_if(custom->vars.begin(),custom->vars.end(),
        [&str](const pair<string,Variable>& p) {
            return (str==p.first);
        }
                       );
        if(it==custom->vars.end())throw string("Custom don't hava such a variable");
        it->second= {retStack}; //TODO ??
        retStack=nullptr;
        break;
    }
    case expr_array: {
        ArrayExpr* expr=dynamic_cast<ArrayExpr*>(data.exprData[exprNum]);
        int a;
        if(data.tokens[expr->strOrFloat]==tok_integer) {
            a=data.tokens.GetInteger(expr->strOrFloat);
        } else {
            string name=data.tokens.GetString(expr->strOrFloat);
            auto var=GetVar(nms,data.tokens.GetString(expr->strOrFloat));
            auto fl=dynamic_pointer_cast<Float>(var);
            if(fl==nullptr) {
                auto fl2=dynamic_pointer_cast<Integer>(var);
                if(fl2==nullptr)throw string("Number required.");

                a=fl2->num;
            } else {
                a=fl->num;
            }

        }
        Custom* custom= new Custom;
        for(int i=0; i<a; ++i) {
            Compute(data,nms,expr->callExpr);
            custom->vars.push_back(make_pair(to_string(i),Variable {retStack})); // TODO ??
            retStack=nullptr;
        }
        retStack=shared_ptr<Var>(custom);
        break;
    }
    }
}

void ModuleData::ComputeFuncDef(Parser::ParserData& data,NameSpace& nms,uint exprNum) {
    FuncDefinitionExpr* expr=dynamic_cast<FuncDefinitionExpr*>(data.exprData[exprNum]);
    NameSpace my(&nms);
    if(expr->parameters.size()!=parStack.size())throw string("Wrong number of parameters.");
    for(uint i=0; i<parStack.size(); ++i) {
        VarExpr* var=dynamic_cast<VarExpr*>(data.exprData[expr->parameters[i]]);
        my.AddVariable(data.tokens.GetString(var->name),parStack[i]);
    }
    parStack.clear();
    for(uint par:expr->exprToExecute) {
        Compute(data,my,par);
        //here out should add returns
    }
    switch(expr->returns.size()) {
    case 0:
        retStack=shared_ptr<Var>(new String("Nothing interesting."));
        break;
    case 1:
        retStack=shared_ptr<Var>(GetVar(my,data.tokens.GetString(expr->returns[0])));
        break;
    default:
        Custom* custom=new Custom;
        for(uint par:expr->returns) {
            string name=data.tokens.GetString(par);
            shared_ptr<Var> var=GetVar(my,name);
            if(var==nullptr)var=shared_ptr<Var>(new String("No such a variable"));
            custom->vars.push_back(make_pair(name,Variable {shared_ptr<Var>(var)})); // TODO assda
        }
        retStack=shared_ptr<Var>(custom);
        break;
    }
}
void ModuleData::NameSpace::AddVariable(const string& str,shared_ptr<Var> var) {
    auto it=find_if(variables.begin(),variables.end(),
    [&str](const pair<string,Variable>& p) {
        return (str==p.first);
    }
                   );
    if(it==variables.end()) {
        variables.push_back(pair<string,Variable>(str,Variable {var})); // TODO ??
    } else {
        it->second= {var}; // TODO ??
    }
}
shared_ptr<Var> ModuleData::GetVar(NameSpace& nms,const string& str) {
    auto it=find_if(nms.variables.begin(),nms.variables.end(),
    [&str](const pair<string,Variable>& p) {
        return (str==p.first);
    }
                   );
    if(it!=nms.variables.end()) {
        return it->second.mVar;// TODO ??
    } else if(nms.mParent!=nullptr) {
        return GetVar(*nms.mParent,str);
    }
    return nullptr;
}
void ModuleData::NameSpace::AddFunc(const string& str,uint func) {
    auto it=find_if(definedFunctions.begin(),definedFunctions.end(),
    [&str](const pair<string,uint>& p) {
        return (str==p.first);
    }
                   );
    if(it==definedFunctions.end()) {
        definedFunctions.push_back(pair<string,uint>(str,func));
    } else {
        it->second=func;
    }
}
uint ModuleData::GetFunc(NameSpace& nms,const string& str) {
    auto it=find_if(nms.definedFunctions.begin(),nms.definedFunctions.end(),
    [&str](const pair<string,uint>& p) {
        return (str==p.first);
    }
                   );
    if(it!=nms.definedFunctions.end()) {
        return it->second;
    } else if(nms.mParent!=nullptr) {
        return GetFunc(*nms.mParent,str);
    }
    throw string("Function don't exist.");
}


static int shift=0;
static void PrintShift() {
    for(int i=0; i<shift; ++i)cout<<" ";
}
void ModuleData::NameSpace::PrintVariables() {
    shift=0;
    for(auto var:variables) {
        PrintShift();
        printf("%-15s: ",var.first.c_str());
        var.second.mVar->Print();
    }
}
void Float::Print() {
    cout<<num<<endl;
}
void Integer::Print() {
    cout<<num<<endl;
}
void String::Print() {
    cout<<mStr<<endl;
}
void Custom::Print() {
    PrintShift();
    cout<<endl;
    shift+=4;
    for(auto var:vars) {
        PrintShift();
        printf("%-15s: ",var.first.c_str());
        string name=var.first;
        var.second.mVar->Print();
    }
    shift-=4;
}
