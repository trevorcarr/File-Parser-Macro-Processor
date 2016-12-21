/*  Luke Jasem, Tyler Grass, Trevor Carr
 cssc0614
 team Indiana
 prog2
 CS530, Fall 2016
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "file_parse_exception.h"
#include "file_parser.h"
#include "macroPP.h"
#include "map"
using namespace std;

// ctor, file name is the parameter

macroPP::macroPP(string s) {
    
    expansion_file = s + "e";
    vector<string> tmp;
    
    file_parser in_file(s);
    in_file.read_file();
    ofstream output_file;
    
    output_file.open(expansion_file.c_str(), fstream::out | fstream::trunc);
    output_file.close();
    
    output_file.open(expansion_file.c_str(), fstream::out | fstream::app);
    
    for ( int i = 0; i  < in_file.size(); i++ ) {
        
        //Opens and parses the file following the #minclude statement
        if (in_file.get_token(i,0) == "#minclude"){
            file_parser include_file(in_file.get_token(i,1));
            include_file.read_file();
            
            for( int k = 0; k < include_file.size(); k++) {
                
                if (include_file.get_token(k,1) == "macro") {
                    key = include_file.get_token(k,0);
                    
                    while ( include_file.get_token(k-1,1) != "endm" ) {
                        for (int j = 0; j < 4; j++)
                            line += include_file.get_token(k,j) + "\t";
                        
                        tmp.push_back(line);
                        line = "";
                        k++;
                    }
                    name_tab.insert( make_pair(key, make_pair(tmp, 0)));
                    tmp.clear();
                }
            }
        }
        
        if (in_file.get_token(i,1) == "macro") { //storing macros in map
            key = in_file.get_token(i,0);
            
            while ( in_file.get_token(i-1,1) != "endm" ) {
                for (int j = 0; j < 4; j++)
                    line += in_file.get_token(i,j) + "\t";
                
                tmp.push_back(line);
                line = "";
                i++;
            }
            name_tab.insert( make_pair(key, make_pair(tmp, 0)));
            tmp.clear();
        }
    }
    
    int endm = -1;
    for ( int i = 0; i  < in_file.size(); i++ ) {
        if (in_file.get_token(i,0) == "#minclude"){ // printing the included files to screen
            int endm = -1;
            file_parser minclude_file(in_file.get_token(i,1));
            minclude_file.read_file();
            for( int k = 0; k < minclude_file.size(); k++) {
                int j = k;
                if (minclude_file.get_token(k,1) == "macro"){
                    
                    while (minclude_file.get_token(j,1) != "endm"){
                        j++;
                    }
                    endm = j;
                }
                if (k > endm){
                    if((minclude_file.get_token(k,0) == "") && (minclude_file.get_token(k,1) == "") && (minclude_file.get_token(k,2) == "")){
                        line += minclude_file.get_token(k,3);
                    } else {
                        for (int j = 0; j < 4; j++){
                            line += minclude_file.get_token(k,j) + "\t";
                        }
                    }
                    
                    output_file << line << endl;
                    line = "";
                }
            }
        }
        string macro_value = in_file.get_token(i,1); //checking if token is a macro or not
        map<string, pair< vector<string>, int > >::iterator it;
        vector<string> parameters;
        it = name_tab.find(macro_value);
        
        if(it != name_tab.end()){
            string str = in_file.get_token(i,2);
            stringstream ss(str);
            while ( ss.good() ) { // putting the parameters into a vector
                string substr;
                getline(ss, substr, ',' );
                parameters.push_back(substr);
            }
            
            line = "*";            //commenting out original macro line
            for( int j=0; j < 4; j++){
                line += in_file.get_token(i,j) + "\t";
            }
            
            output_file << line << endl;
            line = "";
            
            for (unsigned int i = 1; i < it->second.first.size()-1; i++) {
                string str = it->second.first[i];     //getting the macro
                int found = str.find("\\");
                
                if ( found > 0) {   //if there is an \\, put parameter in
                    while (found != -1){
                        char index = str.at(found+1);
                        int index_number;
                        
                        if(index == '@'){
                            int invocation_num = it->second.second;
                            char invocation_num_char = (char)invocation_num;
                            char* invocation_num_pointer;
                            invocation_num_pointer = &invocation_num_char;
                            str.replace(found, 2, invocation_num_pointer);
                        }
                        else {
                            index_number = index - '0'; //converting to int
                            str.replace(found, 2, parameters[index_number-1]);
                        }
                        
                        found = str.find("\\");
                    }
                    
                    line += str;
                    output_file << line << endl;
                    line = "";
                    
                } else {
                    line += str;
                    output_file << line << endl;
                    
                }
                line = "";

            }
            line += "*\tEND OF MACRO EXPANSION \n" ;
            output_file << line << endl;
            line = "";
            
        } else {
            int j = i;
            if (in_file.get_token(j,1) == "macro"){
                while (in_file.get_token(j,1) != "endm"){
                    j++;
                }
                endm = j;
            }
            if (i > endm){
                if((in_file.get_token(i,0) == "") && (in_file.get_token(i,1) == "") && (in_file.get_token(i,2) == "")){ //printing comment to first space
                    line += in_file.get_token(i,3);
                } else if (in_file.get_token(i,0) == "#minclude") { // dont print minclude
                } else {
                    for (int k = 0; k < 4; k++){ // print line to new file
                        line += in_file.get_token(i,k) + "\t";
                    }
                }
                output_file << line << endl;
                line = "";
            }
        }
        
    }
    output_file.close();
    cout << "Macro Preprocessing Complete" << endl;
}

macroPP::~macroPP(){}


int main(int argc, char* argv[]) {
    
    if (argc != 2){
        cout << "Incorrect amount of command line arguments." << endl;
        return 1;
    }
    
    try {
        macroPP macros(argv[1]);
        
    }
    
    catch (file_parse_exception& e){
        return 1;
    }
}
