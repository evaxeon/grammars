#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <typeinfo>
#include <set>
#include <stack>
#include <deque>
#include "header.hpp"

Token::Token(std::vector<int> c, State t, int pos, int l) {
    contents = c;
    tokenType = t;
    position = pos;
    line = l;
}

Token& Token::operator=(Token other) {
    contents = other.contents;
    tokenType = other.tokenType;
    position = other.position;
    line = other.line;
}

bool Token::operator!= (Token const other) const {
    return (contents != other.contents) && (tokenType != other.tokenType) && (position != other.position) && (line != other.line);
}
bool Token::operator== (Token const other) const {
    return (contents == other.contents) && (tokenType == other.tokenType) && (position == other.position) && (line == other.line);
}

Token::~Token() {
        std::vector<int>().swap(contents);
}

TokenIterator::TokenIterator(bool newFlag) {
    endFlag = newFlag;
    if (newFlag == true) {
        lastToken.contents.clear();
    }
}

bool TokenIterator::operator!= (TokenIterator const other) const { 
    return (endFlag != other.endFlag); 
} 
bool TokenIterator::operator== (TokenIterator const other) const { 
    return (endFlag == other.endFlag); 
}

void TokenIterator::gc() {
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

void TokenIterator::lastTokenAssign(std::vector<int> c, State t, int pos, int l) {
    lastToken.contents = c;
    lastToken.tokenType = t;
    lastToken.position = pos;
    lastToken.line = l;
}

void TokenIterator::getToken() {
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
            } else throw TokenException("Scanner error: Not expected symbol. ", curPos, curLine);
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
            } else throw TokenException("Scanner error: Character expected in IDENT. ", curPos, curLine);
            break;
        case NUMBERCONST:
            if (isdigit(c)) {
                curVector.push_back(c);
                gc();
            } else if (c == ' ' || c == '\n' || c == '\t' || c == '(' || c == ')' || c == '[' || c == ']' || isalpha(c) || c == '+' || c == '*') {
                lastTokenAssign(curVector, CS, tokenPos, tokenLine);
                return;
            } else throw TokenException("Scanner error: Digit or divider expected in NUMBERCONST. ", curPos, curLine);
            break;
        case LEFTBR:case RIGHTBR:case LEFTSQUARE:case RIGHTSQUARE:case PLUS:case MULTIPLY:
            if (isalpha(c) || isdigit(c) || c == '+' || c == '*' || c == '(' || c == ')' || c == '[' || c == ']' || c == ' ' || c == '\n' || c == '\t') {
                lastTokenAssign(curVector, CS, tokenPos, tokenLine);
                return;
            } else throw TokenException("Scanner error: Not expected in SIGNS. ", curPos, curLine);
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
            std::cout << "I'm in default with State " << CS << std::endl;
        }


    } while ((c != EOF));
    lastToken.contents = curVector;
    lastToken.tokenType = CS;
    lastToken.position = tokenPos;
    lastToken.line = tokenLine;
    return;
}

TokenIterator& TokenIterator::operator++ () {
    try {
        getToken();
        return *this;
    } catch (TokenException& caught) {
        throw caught;
    }
}

TokenIterator& TokenIterator::operator++ (int) {
    try {
        getToken();
        return *this;
    } catch (TokenException& caught) {
        throw caught;
    }
}

Token TokenIterator::operator*(){
    return lastToken;
}