#include "parser.h"
#include "exceptions.h"
#include <cstring>

using namespace Parser;
using namespace std;

TokenContainer::~TokenContainer() {
    Clear();
}
void TokenContainer::Clear() {
    for(uint numToken=0; numToken<size(); ++numToken) {
        switch(mTokens[numToken]) {
        case tok_string:
            delete(string*)mData[numToken];
            break;
        case tok_chars:
            delete(string*)mData[numToken];
            break;
        case tok_float:
            if(sizeof(double)>sizeof(void*))delete(double*)mData[numToken];
            break;
        }
    }
    mData.clear();
    mTokens.clear();
}
uint TokenContainer::size() {
    return mTokens.size();
}

char TokenContainer::operator[](uint numToken) {
    if(numToken>=mTokens.size())return 0;
    return mTokens[numToken];
}
string TokenContainer::GetString(uint numToken) {
    if(numToken>=mTokens.size())throw ParseException {"Token overflow :]"};
    if(mTokens[numToken]!=tok_string)throw ParseException {"Trying to get String from token which is not a String."};
    return *(string*)mData[numToken];
}
string TokenContainer::GetChars(uint numToken) {
    if(numToken>=mTokens.size())throw ParseException {"Token overflow :]"};
    if(mTokens[numToken]!=tok_chars)throw ParseException {"Trying to get Chars from token which is not a Chars token."};
    return *(string*)mData[numToken];
}
double TokenContainer::GetFloat(uint numToken) {
    if(numToken>=mTokens.size())throw ParseException {"Token overflow :]"};
    if(mTokens[numToken]!=tok_float)throw ParseException {"Trying to get Float from token which is not a Float."};
    if(sizeof(double)<=sizeof(void*)) {
        return *(double*)&mData[numToken];
    } else {
        return *(double*)mData[numToken];
    }
}
long int TokenContainer::GetInteger(uint numToken) {
    if(numToken>=mTokens.size())throw ParseException {"Token overflow :]"};
    if(mTokens[numToken]!=tok_integer)throw ParseException {"Trying to get Integer from token which is not a Integer."};
    if(sizeof(long int)<=sizeof(void*)) {
        return *(long int*)&mData[numToken];
    } else {
        return *(long int*)mData[numToken];
    }
}

void TokenContainer::Add(char token) {
    mTokens.push_back(token);
    mData.push_back(nullptr);
}

void TokenContainer::AddString(const string& str) {
    mTokens.push_back(Parser::tok_string);
    mData.push_back(new string(str));
}
void TokenContainer::AddChars(const string& str) {
    mTokens.push_back(Parser::tok_chars);
    mData.push_back(new string(str));
}
void TokenContainer::AddFloat(double num) {
    mTokens.push_back(Parser::tok_float);
    if(sizeof(double)<=sizeof(void*)) {
        void* ptr;
        memcpy(&ptr,&num,sizeof(void*));
        mData.push_back(ptr);
    } else {
        mData.push_back(new double(num));
    }
}
void TokenContainer::AddInteger(long int num) {
    mTokens.push_back(Parser::tok_integer);
    if(sizeof(long int)<=sizeof(void*)) {
        void* ptr;
        memcpy(&ptr,&num,sizeof(void*));
        mData.push_back(ptr);
    } else {
        mData.push_back(new long int(num));
    }
}
