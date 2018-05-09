#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <typeinfo>
#include <set>
#include <stack>
#include <deque>
#include "header.hpp"

TokenSyntax::TokenSyntax(TokenIterator t) {
    iter = t;
    curTok = *t;
    nextToken();
    S();
}

void TokenSyntax::pushAndSave(std::pair<std::string, std::vector<int> > newPair) {
    stateStack.push(newPair);
    result.push_back(newPair);
}

void TokenSyntax::nextToken() {
    ++iter;
    curTok = *iter;
}

void TokenSyntax::checkBinaryOp() {
    std::pair<std::string, std::vector<int> > op1, oper, op2, res;
    std::vector<int> resVector;

    op2 = stateStack.top();
    stateStack.pop();
    oper = stateStack.top();
    stateStack.pop();
    op1 = stateStack.top();
    stateStack.pop();

    if (op1.first == "INTTYPE" && op2.first == op1.first && (oper.first == "PLUS" || oper.first == "MULTIPLY")) {
        addToVector(resVector, op1.second);
        addToVector(resVector, oper.second);
        addToVector(resVector, op2.second);

        res = std::make_pair("INTTYPE", resVector);
        stateStack.push(res);
        result.push_back(res);
    } else {
        throw TokenException("Syntax error: Wrong type for binary operation!", opPos, opLine);
    }

}

void TokenSyntax::funcProcess(std::string& arrString, std::vector<int>::iterator it) {
    arrString += "FUNCTYPE ret value ";
    std::string tempString;
    bool ret = true, empty = true, arr = false;
    ++it;

    while ((*it == 'i') || (*it == 'j') || (*it == 'k') || (*it == 's') || (*it == 't') || (*it == 'a')) {
        empty = false;
        switch(*it){
        case 'i':case'j':case'k':
            arr = false;
            arrString += "INTTYPE ";
            ++it;
            break;
        case 's':case't':
            arr = false;
            arrString += "STRINGTYPE ";
            ++it;
            break;
        case 'a':
            arr = true;
            while (*it == 'a') {
                tempString += "ARRAYTYPE of ";
                ++it;
            }
            if ((*it != 'i') && (*it != 'j') && (*it != 'k') && (*it != 's') && (*it != 't')) throw TokenException("Array without elements is not allowed");
            arrString += tempString;
            tempString.clear();
            break;
        }
        if (ret && !arr) {
            arrString += "ARGS ";
            ret = false;
        }
    }
    if (isdigit(*it)) throw TokenException("Syntax error: Numberconst found in function! ", curTok.position, curTok.line);
    if (*it == 'f') throw TokenException("Syntax error: Second f found in function! ", curTok.position, curTok.line);
    if (empty) throw TokenException("Syntax error: Function must have at least return value type. ", curTok.position, curTok.line);
}

FinalState TokenSyntax::checkOp(Token tok, std::string& arrString) {
    auto it = tok.contents.begin();
    std::string funcInArr;
    switch(tok.tokenType) {
    case IDENT:
        if ((*it == 'i' || *it == 'j' || *it == 'k') && (tok.contents.size() <= 2)) {
            return INTTYPE;
        } else if (*it == 'a') {
            while (*it == 'a') {
                arrDeque.push_back("ARRAYTYPE of ");

                ++it;
            }
            if ((*it == 'i' || *it == 'j' || *it == 'k') && (++it == tok.contents.end())) {
                arrDeque.push_back("INTTYPE");
                return ARRAYTYPE;
            } else if ((*it == 's' || *it == 't') && (++it == tok.contents.end())) {
                arrDeque.push_back("STRINGTYPE");
                return ARRAYTYPE;
            } else if (*it == 'f') {
                funcProcess(funcInArr, it);
                arrDeque.push_back(funcInArr);
                return ARRAYTYPE;
            }
            throw TokenException("Syntax error: Unknown array type without any element type. ", curTok.position, curTok.line);

        } else if (*it == 'f') {
            funcProcess(arrString, it);
            return FUNCTYPE;
        } else if ((*it == 's' || *it == 't') && (tok.contents.size() <= 2)) {
            return STRINGTYPE;
        } else throw TokenException("Syntax error: Unknown IDENT literal. ", curTok.position, curTok.line);
        break;
    case NUMBERCONST:
        return INTTYPE;
        break;
    }
    return ERRTYPE;
}


void TokenSyntax::S() {
    FinalState curState;
    curState = E();
    if (curTok.tokenType != ENDOFFILE) throw TokenException("Syntax critical flaw: Error found in your expression. ", curTok.position, curTok.line);
    std::cout << "This expression is correct!\n";
    
    std::cout << "Result: \n";
    
    for (auto k: result) {
        std::cout << k << std::endl;
    }

}

FinalState TokenSyntax::E() {
    FinalState curState;
    if (curTok.tokenType == NUMBERCONST || curTok.tokenType == IDENT || curTok.tokenType == LEFTBR) {
        curState = F();

        while (curTok.tokenType == PLUS) {
            opPos = curTok.position;
            opLine = curTok.line;
            addToVector(arrVector, curTok.contents);
            stateStack.push(std::make_pair("PLUS", curTok.contents));
            nextToken();
            curState = F();
            checkBinaryOp();
            if (curState != INTTYPE) throw TokenException("Syntax error: Arithmetical operation supports INTTYPES only! ", curTok.position, curTok.line); 
        }
        return curState;
    } return EMPTYTYPE;

}

FinalState TokenSyntax::F() {
    FinalState curState;
    if (curTok.tokenType == NUMBERCONST || curTok.tokenType == IDENT || curTok.tokenType == LEFTBR) {
        curState = T();
        while (curTok.tokenType == MULTIPLY) {
            opPos = curTok.position;
            opLine = curTok.line;
            addToVector(arrVector, curTok.contents);
            stateStack.push(std::make_pair("MULTIPLY", curTok.contents));                
            nextToken();
            curState = T();
            checkBinaryOp();
            if (curState != INTTYPE) throw TokenException("Syntax error: Arithmetical operation supports INTTYPES only!", curTok.position, curTok.line); 
            
        }
        return curState;
    } else throw TokenException("Syntax error: Didn't find the expected literal in F"); 
}

FinalState TokenSyntax::T() {
    std::string arrString;
    std::pair<std::string, std::vector<int> > tempPair;
    
    FinalState curState;
    if (curTok.tokenType == NUMBERCONST) {
        curState = INTTYPE;
        pushAndSave(std::make_pair("INTTYPE", curTok.contents));

        addToVector(arrVector, curTok.contents);
        nextToken();
        return curState;
    } else if (curTok.tokenType == IDENT) {
        curState = checkOp(curTok, arrString);
        switch (curState) {
        case INTTYPE:
            addToVector(arrVector, curTok.contents);
            pushAndSave(std::make_pair("INTTYPE", curTok.contents));
            nextToken();
            return curState;
            break;
        case STRINGTYPE:
            pushAndSave(std::make_pair("STRINGTYPE", curTok.contents));
            nextToken();
            return curState;
            break;
        case FUNCTYPE:
            pushAndSave(std::make_pair(arrString, curTok.contents));
            nextToken();
            return curState;
            break;
        case ARRAYTYPE:
            arrVector.clear();
            addToVector(arrVector, curTok.contents);
            isArr = true;
            break;
        }
        nextToken();
        if (curTok.tokenType == LEFTSQUARE) {
            while (curTok.tokenType == LEFTSQUARE) {
                if ((curState != ARRAYTYPE) && !isArr) throw TokenException("Square brackets allowed only with arrays! ", curTok.position, curTok.line);
                arrVector.push_back('[');
                if (arrDeque.size() > 0) arrDeque.pop_front(); 
                else throw TokenException("Wrong array lexem", curTok.position, curTok.line); 
                dequeToString(arrDeque, arrString); 
                nextToken();
                if (E() != INTTYPE) throw TokenException("Syntax error: Indexing allowed only with INTTYPES! ", curTok.position, curTok.line);

                if (curTok.tokenType != RIGHTSQUARE) throw TokenException("Syntax error: Didn't find the RIGHTSQUARE literal ", curTok.position, curTok.line);
                stateStack.pop();
                arrVector.push_back(']');
                if (arrDeque.size() == 1) {
                    arrString = *(arrDeque.begin());
                    if (arrString == "INTTYPE") {
                        curState = INTTYPE;
                    } else if (arrString == "STRINGTYPE") {
                        curState = STRINGTYPE;
                    }
                    arrDeque.clear();
                    pushAndSave(std::make_pair(arrString, arrVector));
                    nextToken();
                } else {
                    nextToken();
                    if (*(curTok.contents.begin()) != '[') {
                        pushAndSave(std::make_pair(arrString, arrVector));
                    }
                }
            }
            isArr = false;
        } else {
            dequeToString(arrDeque, arrString);
            pushAndSave(std::make_pair(arrString, arrVector));
            nextToken();
        }

    } else if (curTok.tokenType == LEFTBR) {
        metBrackets = true;
        nextToken();
        arrDeque.clear();
        curState = E();
        if (curTok.tokenType != RIGHTBR) throw TokenException("Syntax error: Didn't find rightbr literal. ", curTok.position, curTok.line);

        tempPair = stateStack.top();
        stateStack.pop();
        tempPair.second.insert(tempPair.second.begin(), '(');
        tempPair.second.push_back(')');
        stateStack.push(tempPair);

        nextToken();
    } else throw TokenException("Syntax error: Didn't find the expected literal in T. ", curTok.position, curTok.line); 
    return curState;
}
