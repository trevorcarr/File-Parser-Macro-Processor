/* Tyler Grass, Trevor Carr, Luke Jasem
 cssc0614
 Team Indiana
 macroPP.h
 CS530, Fall 2016
 */

#ifndef MACROPP_H
#define MACROPP_H

#include <string>
#include <sstream>
#include <map>
#include <vector>

using namespace std;

class macroPP {
    
public:
    macroPP(string);
    ~macroPP();
    void print_macros();
    
private:
    
    string line, key;
    //multi map: string is key, and the values are the vector and the number of times the macro is invoked
    map< string, pair<vector<string>, int> >  name_tab;
    string expansion_file;
};
#endif
