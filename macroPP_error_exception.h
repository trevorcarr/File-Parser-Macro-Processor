/*  macroPP_exception.h
 Exception class for macroPP
 CS530 Fall 2016
 Alan Riggins
 */

#ifndef MACROPP_EXCEPTION_H
#define MACROPP_EXCEPTION_H
#include <string>

using namespace std;

class macroPP_exception {
    
public:
    macroPP_exception(string s) {
        message = s;
    }
    
    macroPP_exception() {
        message = "An error has occurred";
    }
    
    string getMessage() {
        return message;
    }
    
private:
    string message;
};

#endif
