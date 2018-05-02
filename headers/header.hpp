#ifndef HEADER_HPP
#define HEADER_HPP
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <typeinfo>
#include <set>
#include <stack>
#include <deque>



template<typename It, typename It_const, typename Val>
class Iterated {
public:
    virtual It begin() = 0;
    virtual It end() = 0;

    virtual It_const begin() const = 0;
    virtual It_const end() const = 0;
};


struct TokenException: public std::exception {
    std::string s;
    TokenException(std::string uString, int pos = 0, int line = 0) {
        s += uString + "Position: " + std::to_string(pos) + " Line: " + std::to_string(line);
    }
    ~TokenException() throw () {}
    const char* what() const throw() { return s.c_str(); }
};


enum finalState {
    INTTYPE,
    ARRAYTYPE,
    STRINGTYPE,
    FUNCTYPE,
    PLUSTYPE,
    MULTIPLYTYPE,
    ERRTYPE,
    EMPTYTYPE
};

enum state {
    ENDOFFILE,
    IDENT,
    NUMBERCONST,
    LEFTBR,
    RIGHTBR,
    LEFTSQUARE,
    RIGHTSQUARE,
    PLUS,
    MULTIPLY,
    DIVIDER   
};


std::ostream& operator<<(std::ostream &s, const state &type);
std::ostream& operator<<(std::ostream &s, const finalState &type);

struct Token
{
    std::vector<int> contents;
    state tokenType;
    int position;
    int line;

    Token(std::vector<int> c = { 0 }, state t = ENDOFFILE, int pos = 0, int l = 0) {
        contents = c;
        tokenType = t;
        position = pos;
        line = l;
    }

    bool operator!= (Token const other) const {
        return (contents != other.contents) && (tokenType != other.tokenType) && (position != other.position) && (line != other.line);
    }
    bool operator== (Token const other) const {
        return (contents == other.contents) && (tokenType == other.tokenType) && (position == other.position) && (line == other.line);
    }

    Token& operator=(Token other) {
        contents = other.contents;
        tokenType = other.tokenType;
        position = other.position;
        line = other.line;
    }
    ~Token() {
        std::vector<int>().swap(contents);
    }
};

std::ostream& operator<<(std::ostream& os, const Token& t);

class TokenIterator: public std::iterator<std::input_iterator_tag, int>
{
    Token lastToken;
    int curPos = 0, curLine = 0, tokenPos = 0, tokenLine = 0, c;
    bool endFlag, spaceFlag = true;
    state CS;
    std::string curString;
    std::vector<int> curVector;


public:
    TokenIterator(bool newFlag = false) {
        endFlag = newFlag;
        if (newFlag == true) {
            lastToken.contents.clear();
        }
    }
    bool operator!= (TokenIterator const other) const { 
        return (endFlag != other.endFlag); 
    } 
    bool operator== (TokenIterator const other) const { 
        return (endFlag == other.endFlag); 
    }

    void gc() {
        int tempChar = getchar();
        if (tempChar != EOF) {
            c = tempChar;
            
            if (isspace(c)) {
                spaceFlag = true;
            } else spaceFlag = false;
            if (c == '\n') {
                curPos = 0;
                curLine++;
            } else curPos++;

        } else {
            endFlag = true;
            c = EOF;
        }
    }

    void lastTokenAssign(std::vector<int> c, state t, int pos, int l) {
        lastToken.contents = c;
        lastToken.tokenType = t;
        lastToken.position = pos;
        lastToken.line = l;
    }


    void getToken() {
        bool metType = false, metFunc = false, metArr = false, metFuncGlobal = false;
        CS = ENDOFFILE;
        curVector.clear();
        if (spaceFlag) gc();
        do {
            switch(CS) {
            case ENDOFFILE:
                tokenPos = curPos;
                tokenLine = curLine;
                if (isalpha(c)) {
                    CS = IDENT;
                    curVector.push_back(c);
                    gc();
                } else if (isdigit(c)) {
                    CS = NUMBERCONST;
                    curVector.push_back(c);
                    gc();
                } else if (c == '(') {
                    CS = LEFTBR;
                    curVector.push_back(c);
                    gc();
                } else if (c == ')') {
                    CS = RIGHTBR;
                    curVector.push_back(c);
                    gc();
                } else if (c == '[') {
                    CS = LEFTSQUARE;
                    curVector.push_back(c);
                    gc();
                } else if (c == ']') {
                    CS = RIGHTSQUARE;
                    curVector.push_back(c);
                    gc();
                } else if (c == '+') {
                    CS = PLUS;
                    curVector.push_back(c);
                    gc();
                } else if (c == '*') {
                    CS = MULTIPLY;
                    curVector.push_back(c);
                    gc();
                } else if (c == ' ' || c == '\n' || c == '\t') {
                    CS = DIVIDER;
                    curVector.push_back(c);
                    gc();
                } else if (c == EOF) {
                    CS = ENDOFFILE;
                    break;
                } else throw TokenException("Not expected symbol. ", curPos, curLine);
                break;
            case IDENT: 
                if (isalpha(c) || isdigit(c)) {
                    curVector.push_back(c);
                    gc();
                    while (isalpha(c) || isdigit(c)) {
                        curVector.push_back(c);
                        gc();
                    }
                    lastTokenAssign(curVector, CS, tokenPos, tokenLine);
                    return;
                } else if (c == ' ' || c == '\n' || c == '\t' || c == '+' || c == '*' || c == ')' || c == '(' || c == '[' || c == ']') {
                    lastTokenAssign(curVector, CS, tokenPos, tokenLine);
                    return;
                } else throw TokenException("Character expected in IDENT. ", curPos, curLine);
                break;
            case NUMBERCONST:
                if (isdigit(c)) {
                    curVector.push_back(c);
                    gc();
                } else if (c == ' ' || c == '\n' || c == '\t' || c == '(' || c == ')' || c == '[' || c == ']' || isalpha(c) || c == '+' || c == '*') {
                    lastTokenAssign(curVector, CS, tokenPos, tokenLine);
                    return;
                } else throw TokenException("Digit or divider expected in NUMBERCONST. ", curPos, curLine);
                break;
            case LEFTBR:case RIGHTBR:case LEFTSQUARE:case RIGHTSQUARE:case PLUS:case MULTIPLY:
                if (isalpha(c) || isdigit(c) || c == '+' || c == '*' || c == '(' || c == ')' || c == '[' || c == ']' || c == ' ' || c == '\n' || c == '\t') {
                    lastTokenAssign(curVector, CS, tokenPos, tokenLine);
                    return;
                } else throw TokenException("Not expected in SIGNS. ", curPos, curLine);
                break;
            case DIVIDER:
                if (c == ' ' || c == '\n' || c == '\t') {
                    while (c == ' ' || c == '\n' || c == '\t') gc();
                    CS = ENDOFFILE;
                } else if (isdigit(c) || isalpha(c) || c == '+' || c == '*' || c == '(' || c == ')' || c == '[' || c == ']') {
                    lastTokenAssign(curVector, CS, tokenPos, tokenLine);
                    return;
                }
                break;                
            default:
                std::cout << "I'm in default with state " << CS << std::endl;
            }


        } while ((c != EOF));
        lastToken.contents = curVector;
        lastToken.tokenType = CS;
        lastToken.position = tokenPos;
        lastToken.line = tokenLine;
        return;
    }


    TokenIterator& operator++ () {
        try {
            getToken();
            return *this;
        } catch (TokenException& caught) {
            throw caught;
        }
    }

    TokenIterator& operator++ (int) {
        try {
            getToken();
            return *this;
        } catch (TokenException& caught) {
            throw caught;
        }
    }

    Token operator*(){
        return lastToken;
    }
}; 




template<class It, class It_const>
class TokenContainer: public Iterated<It, It_const, int>
{
public:
    It begin() {
        return TokenIterator();
    }

    It end() {
        return TokenIterator(true);
    }
    It_const begin() const {
        return TokenIterator();
    }
    It_const end() const {
        return TokenIterator(true);
    }


};

std::string vecToString(std::vector<int> vec);

void addToVector(std::vector<int>& dest, std::vector<int> from);

std::ostream& operator<<(std::ostream& os, std::pair<std::string, std::vector<int> > & t);

void dequeToString (std::deque<std::string> from, std::string& to);

std::ostream& operator<<(std::ostream& os, std::deque<std::string> & t);

std::ostream& operator<<(std::ostream& os, const std::vector<int>& from);

void addToPair(std::pair<std::string, std::vector<int> > from, std::pair<std::string, std::vector<int> >& to);

class TokenSyntax {
public:
    Token curTok;
    TokenIterator iter;
    std::deque<std::string> arrDeque;
    std::stack<std::pair<std::string, std::vector<int> > > mainStack;
    std::vector<int> arrVector;
    std::vector<std::pair<std::string, std::vector<int> > > result;

    bool first = true, isArr = false, metBrackets = false;

    int opPos = 0, opLine = 0;

    TokenSyntax(TokenIterator t) {
        iter = t;
        curTok = *t;
        //++iter;
        //curTok = *iter;
        nextToken();
        S();
    }

    void pushAndSave(std::pair<std::string, std::vector<int> > newPair) {
        mainStack.push(newPair);
        result.push_back(newPair);
    }

    void nextToken() {
        ++iter;
        curTok = *iter;
    }

    void checkBinaryOp() {
        std::pair<std::string, std::vector<int> > op1, oper, op2, res;
        std::vector<int> resVector;

        op2 = mainStack.top();
        mainStack.pop();
        oper = mainStack.top();
        mainStack.pop();
        op1 = mainStack.top();
        mainStack.pop();

        if (op1.first == "INTTYPE" && op2.first == op1.first && (oper.first == "PLUS" || oper.first == "MULTIPLY")) {
            addToVector(resVector, op1.second);
            addToVector(resVector, oper.second);
            addToVector(resVector, op2.second);

            res = std::make_pair("INTTYPE", resVector);
            mainStack.push(res);
            result.push_back(res);
        } else {
            throw TokenException("Wrong type for binary operation!", opPos, opLine);
        }

    }

    void funcProcess(std::string& arrString, std::vector<int>::iterator it) {
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
        if (isdigit(*it)) throw TokenException("Numberconst found in function! ", curTok.position, curTok.line);
        if (*it == 'f') throw TokenException("Second f found in function! ", curTok.position, curTok.line);
        if (empty) throw TokenException("Function must have at least return value type. ", curTok.position, curTok.line);
    }

    finalState checkOp(Token tok, std::string& arrString) {
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
                throw TokenException("Unknown array type without any element type. ", curTok.position, curTok.line);

            } else if (*it == 'f') {
                funcProcess(arrString, it);
                return FUNCTYPE;
            } else if ((*it == 's' || *it == 't') && (tok.contents.size() <= 2)) {
                return STRINGTYPE;
            } else throw TokenException("Unknown IDENT literal. ", curTok.position, curTok.line);
            break;
        case NUMBERCONST:
            return INTTYPE;
            break;
        }
        return ERRTYPE;
    }

    

    void S() {
        finalState curState;
        curState = E();
        if (curTok.tokenType != ENDOFFILE) throw TokenException("Error found in your expression. ", curTok.position, curTok.line);
        std::cout << "This expression is correct!\n";
        
        std::cout << "Result: \n";
        
        for (auto k: result) {
            std::cout << k << std::endl;
        }

    }

    finalState E() {
        finalState curState;
        if (curTok.tokenType == NUMBERCONST || curTok.tokenType == IDENT || curTok.tokenType == LEFTBR) {
            curState = F();

            while (curTok.tokenType == PLUS) {
                opPos = curTok.position;
                opLine = curTok.line;
                addToVector(arrVector, curTok.contents);
                mainStack.push(std::make_pair("PLUS", curTok.contents));
                //++iter;
                //curTok = *iter;
                nextToken();
                curState = F();
                checkBinaryOp();
                if (curState != INTTYPE) throw TokenException("Arithmetical operation supports INTTYPES only! ", curTok.position, curTok.line); 
            }
            return curState;
        } return EMPTYTYPE;

    }

    finalState F() {
        finalState curState;
        if (curTok.tokenType == NUMBERCONST || curTok.tokenType == IDENT || curTok.tokenType == LEFTBR) {
            curState = T();
            while (curTok.tokenType == MULTIPLY) {
                opPos = curTok.position;
                opLine = curTok.line;
                addToVector(arrVector, curTok.contents);
                mainStack.push(std::make_pair("MULTIPLY", curTok.contents));                
                //++iter;
                //curTok = *iter;
                nextToken();
                curState = T();
                checkBinaryOp();
                if (curState != INTTYPE) throw TokenException("Arithmetical operation supports INTTYPES only!", curTok.position, curTok.line); 
                
            }
            return curState;
        } else throw TokenException("Didn't find the expected literal in F"); 
    }

    finalState T() {
        std::string arrString;
        std::pair<std::string, std::vector<int> > tempPair;
        
        finalState curState;
        if (curTok.tokenType == NUMBERCONST) {
            curState = INTTYPE;

            //result.push_back(std::make_pair("INTTYPE", curTok.contents));
            //mainStack.push(std::make_pair("INTTYPE", curTok.contents));

            pushAndSave(std::make_pair("INTTYPE", curTok.contents));

            addToVector(arrVector, curTok.contents);
            //++iter;
            //curTok = *iter;
            nextToken();
            return curState;
        } else if (curTok.tokenType == IDENT) {
            curState = checkOp(curTok, arrString);
            switch (curState) {
            case INTTYPE:
                addToVector(arrVector, curTok.contents);

                //mainStack.push(std::make_pair("INTTYPE", curTok.contents));

                //result.push_back(std::make_pair("INTTYPE", curTok.contents));
                pushAndSave(std::make_pair("INTTYPE", curTok.contents));

                //++iter;
                //curTok = *iter;
                nextToken();
                return curState;
                break;
            case STRINGTYPE:
                
                //mainStack.push(std::make_pair("STRINGTYPE", curTok.contents));
                //result.push_back(std::make_pair("STRINGTYPE", curTok.contents));
                pushAndSave(std::make_pair("STRINGTYPE", curTok.contents));

                //++iter;
                //curTok = *iter;
                nextToken();
                return curState;
                break;
            case FUNCTYPE:
                
                //mainStack.push(std::make_pair(arrString, curTok.contents));
                //result.push_back(std::make_pair(arrString, curTok.contents));

                pushAndSave(std::make_pair(arrString, curTok.contents));

                //++iter;
                //curTok = *iter;
                nextToken();
                return curState;
                break;
            case ARRAYTYPE:
                arrVector.clear();
                addToVector(arrVector, curTok.contents);
                isArr = true;
                break;
            }
            //++iter;
            //curTok = *iter;
            nextToken();
            if (curTok.tokenType == LEFTSQUARE) {
                while (curTok.tokenType == LEFTSQUARE) {
                    if ((curState != ARRAYTYPE) && !isArr) throw TokenException("Square brackets allowed only with arrays! ", curTok.position, curTok.line);
                    arrVector.push_back('[');
                    if (arrDeque.size() > 0) arrDeque.pop_front(); 
                    else throw TokenException("Wrong array lexem", curTok.position, curTok.line); // this fixed the problem but why is it empty tho
                    //if (arrDeque.size() == 0) { std::cout << "DEQUE IS SOMEHOW EMPTY " << std::endl; }
                    //std::cout << "DEQUE SIZE: " << arrDeque.size();
                    dequeToString(arrDeque, arrString); //aai[6+i*i*(6+1)][6] рушится на этом


                    //++iter;
                    //curTok = *iter;
                    nextToken();
                    if (E() != INTTYPE) throw TokenException("Indexing allowed only with INTTYPES! ", curTok.position, curTok.line);

                    if (curTok.tokenType != RIGHTSQUARE) throw TokenException("Didn't find the RIGHTSQUARE literal ", curTok.position, curTok.line);
                    mainStack.pop();
                    arrVector.push_back(']');
                    if (arrDeque.size() == 1) {
                        arrString = *(arrDeque.begin());
                        if (arrString == "INTTYPE") {
                            curState = INTTYPE;
                        } else if (arrString == "STRINGTYPE") {
                            curState = STRINGTYPE;
                        }
                        arrDeque.clear();
                        //mainStack.push(std::make_pair(arrString, arrVector));
                        //result.push_back(std::make_pair(arrString, arrVector));

                        pushAndSave(std::make_pair(arrString, arrVector));

                        //++iter;
                        //curTok = *iter;
                        nextToken();
                    } else {
                        //++iter;
                        //curTok = *iter;
                        nextToken();
                        if (*(curTok.contents.begin()) != '[') {
                            
                            //mainStack.push(std::make_pair(arrString, arrVector));
                            //result.push_back(std::make_pair(arrString, arrVector));

                            pushAndSave(std::make_pair(arrString, arrVector));
                        }
                    }
                }
                isArr = false;
            } else {
                dequeToString(arrDeque, arrString);
                
                //mainStack.push(std::make_pair(arrString, arrVector));
                //result.push_back(std::make_pair(arrString, arrVector));
                pushAndSave(std::make_pair(arrString, arrVector));

                //++iter;
                //curTok = *iter;
                nextToken();
            }

        } else if (curTok.tokenType == LEFTBR) {
            metBrackets = true;
            //++iter;
            //curTok = *iter;
            nextToken();
            arrDeque.clear();
            curState = E();
            if (curTok.tokenType != RIGHTBR) throw TokenException("Didn't find rightbr literal. ", curTok.position, curTok.line);

            tempPair = mainStack.top();
            mainStack.pop();
            tempPair.second.insert(tempPair.second.begin(), '(');
            tempPair.second.push_back(')');
            mainStack.push(tempPair);

            //++iter;
            //curTok = *iter;
            nextToken();
        } else throw TokenException("Didn't find the expected literal in T. ", curTok.position, curTok.line); 
        return curState;
    }

};



template<typename It, typename It_const, typename Val>
void printContainer(const Iterated<It, It_const, Val> &obj) { 
    for(auto l: obj) {
        std::cout << l << std::endl;
    }
}
#endif
