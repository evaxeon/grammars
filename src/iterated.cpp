#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <map>
#include <fstream>
#include <typeinfo>
#include <functional>
#include "header.hpp"


//LCOV_EXCL_START
std::ostream& operator<<(std::ostream &s, const State &type) {
    std::string typestring;
    switch(type) {
    case 0:
        typestring = "ENDOFFILE";
        break;
    case 1:
        typestring = "Ident";
        break;
    case 2:
        typestring = "Numberconst";
        break;
    case 3:
        typestring = "Leftbr";
        break;
    case 4:
        typestring = "Rightbr";
        break;
    case 5:
        typestring = "Leftsquare";
        break; 
    case 6:
        typestring = "Rightsquare";
        break; 
    case 7:
        typestring = "Plus";
        break;
    case 8:
        typestring = "Multiply";
        break;
    case 9:
        typestring = "Divider";
        break;
    }
    s << typestring;
    return s;
}

std::ostream& operator<<(std::ostream &s, const FinalState &type) {
    std::string typestring;
    switch(type) {
    case 0:
        typestring = "INTTYPE";
        break;
    case 1:
        typestring = "ARRAYTYPE";
        break;
    case 2:
        typestring = "STRINGTYPE";
        break;
    case 3:
        typestring = "FUNCTYPE";
        break;
    case 4:
        typestring = "PLUSTYPE";
        break;
    case 5:
        typestring = "MULTIPLYTYPE";
        break; 
    case 6:
        typestring = "ERRTYPE";
        break; 
    case 7:
    	typestring = "EMPTYTYPE";
    	break;
    }
    s << typestring;
    return s;
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
	os << "Contents: ";
	for (std::vector<int>::const_iterator i = t.contents.begin(); i != t.contents.end(); ++i)
    os << (char) *i << ' ';
	os << "Type: " << t.tokenType << " "  << "Position: " << t.position << " " << "Line: " << t.line; 
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<int>& from) {
    for (std::vector<int>::const_iterator i = from.begin(); i != from.end(); ++i)
    os << (char) *i;
    return os;
}


std::ostream& operator<<(std::ostream& os, std::deque<std::string> & t) {
    for (auto k: t) {
        os << k << " ";
    }
    return os;
}




std::string vecToString(std::vector<int> vec) {
    std::string res;
    for (std::vector<int>::const_iterator i = vec.begin(); i != vec.end(); ++i)
    res += *i;
    return res;
}



void addToPair(std::pair<std::string, std::vector<int> > from, std::pair<std::string, std::vector<int> >& to) {
    to.first += from.first;
    addToVector(to.second, from.second);
}

//LCOV_EXCL_STOP

std::ostream& operator<<(std::ostream& os, std::pair<std::string, std::vector<int> > & t) {
    std::cout << t.first << std::endl;
    for (std::vector<int>::const_iterator i = t.second.begin(); i != t.second.end(); ++i)
    os << (char) *i/* << ' '*/;
    std::cout << std::endl;
    return os;
}


void addToVector(std::vector<int>& dest, std::vector<int> from) {
    for (auto k: from) {
        dest.push_back(k);
    }
}

void dequeToString (std::deque<std::string> from, std::string& to) {
    for (auto k: from) {
        to += k;
    }
}