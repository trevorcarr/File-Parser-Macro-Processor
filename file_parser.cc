/*  Luke Jasem, Tyler Grass, Trevor Carr
 cssc0614
 Team Indiana
 file_parser.cc
 CS530, Fall 2016
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "file_parser.h"
#include "file_parse_exception.h"

using namespace std;

// ctor, filename is the parameter.  A driver program will read
// the filename from the command line, and pass the filename to
// the file_parser constructor.  Filenames must not be hard-coded.
file_parser::file_parser(string s){
    numlines = 0;
    filename = s;
}

// dtor
file_parser::~file_parser(){}

// reads the source file, storing the information is some
// auxiliary data structure you define in the private section.
// Throws a file_parse_exception if an error occurs.
// if the source code file fails to conform to the above
// specification, this is an error condition.
void file_parser::read_file() {
    
    ifstream input;
    string line;
    //open and begin reading file
    
    try {
        input.open(filename.c_str(), ios::in);
        
        if (!input) throw file_parse_exception("Invalid Input File: File Not Found");
        
        while (!input.eof()){
            getline(input, line);
            original_content.push_back(line);
            parsed_content.push_back(parse_line(line));
            numlines++;
        }
        
        input.close();
    }
    
    catch (file_parse_exception& e) {
        cout << e.getMessage() << endl;
    }
}

// returns the token found at (row, column).  Rows and columns
// are zero based.  Returns the empty string "" if there is no
// token at that location. column refers to the four fields
// identified above.
string file_parser::get_token(unsigned int row, unsigned int col){
    try {
        if (col > 3 || row > numlines) throw file_parse_exception("Out of bounds, select row or column in range.");
        return parsed_content[row][col];
    }
    
    catch(file_parse_exception& e){
        cout << e.getMessage() << endl;
        exit(-1);
    }
    
}

// prints the source code file to stdout.  Should not repeat
// the exact formatting of the original, but uses tabs to align
// similar tokens in a column. The fields should match the
// order of token fields given above (label/opcode/operands/comments)
void file_parser::print_file(){
    for (unsigned int i = 0; i < numlines; i++){
        for (unsigned int j=0; j<4; j++){
            cout << parsed_content[i][j] << "\t";
        }
        cout << endl;
    }
}

// returns the number of lines in the source code file
int file_parser::size(){
    return numlines;
}

//returns int indicating if input is valid. 0-3 indicates which column had error, return 4 if valid.
vector<string> file_parser::parse_line(string s) {
    
    int spacepos; //position of first space/tab
    int charpos;  //position of first non-space/tab
    int tmp;
    vector<string> parsed_line(4); //this is the vector that will be returned
    
    //Initialize the parsed_line with empty strings
    parsed_line[0] = "";
    parsed_line[1] = "";
    parsed_line[2] = "";
    parsed_line[3] = "";
    
    
    if ( s[0] != ' ' && s[0] != '\t'  && s != "") //can either be label, comment, or #minclude
    {
        
        if (s[0] == '*' || s[0] == ';' ){ //the whole line is a comment
            
            parsed_line[3] = s;
            return parsed_line;
        }
        
        if (s[0] == '#') //the line is #include
            return catch_includes(s);
        
        // This must be a valid label
        
        parsed_line[0] = get_label(s);
        
        if (!check_for_next_token(s, 0))	//Line is only a label
            return parsed_line;
        
        spacepos = s.find_first_of(" \t");
        charpos = s.find_first_not_of(" \t");
        
        //Label caught, moving to opcode
        
        if (s[charpos] == '*' || s[charpos] == ';' ){
            parsed_line[3] = s.substr(charpos);
            return parsed_line;
        }
        
        spacepos = s.find_first_of(" \t");
        charpos = s.find_first_not_of(" \t", spacepos);
        tmp = get_opcode_and_operands(s, spacepos, charpos);
        parsed_line[1] = s.substr(charpos, tmp - charpos);
        
        charpos = tmp;
        if (!check_for_next_token(s, charpos))
            return parsed_line;
        
        
        //Continue to operand
        
        spacepos = s.find_first_of(" \t", charpos);
        charpos = s.find_first_not_of(" \t", spacepos);
        
        if (s[charpos] == '*' || s[charpos] == ';' ){
            parsed_line[3] = s.substr(charpos);
            return parsed_line;
        }
        
        tmp = get_opcode_and_operands(s, spacepos, charpos);
        parsed_line[2] = s.substr(charpos, tmp - charpos);
        
        charpos = tmp;
        
        if (check_for_quotes(s, charpos) != 0)
            charpos = check_for_quotes(s,charpos);
        
        if (!check_for_next_token(s, charpos))
            return parsed_line;
        
        //Last token must be a comment
        
        spacepos = s.find_first_of(" \t", charpos);
        charpos = s.find_first_not_of(" \t", spacepos);
        
        try {
            if (s[charpos] != '*' && s[charpos] != ';' ) throw file_parse_exception("Excess tokens on line: ");
            parsed_line[3] = s.substr(charpos);
            return parsed_line;
        }
        
        catch(file_parse_exception& ex) {
            int i;
            ss << numlines;
            ss >> i;
            i++;
            cout<< ex.getMessage() << i << endl;
            exit(-1);
        }
        
    }
    //There is no label on the line
    spacepos = s.find_first_of("\t ");
    charpos = s.find_first_not_of("\t ");
    
    if(spacepos == -1 || charpos == -1) return parsed_line; //Line is only whitespace
    
    if (s[charpos] == '*' || s[charpos] == ';' ){
        parsed_line[3] = s.substr(charpos);
        return parsed_line;
    }
    
    tmp = get_opcode_and_operands(s, spacepos, charpos);
    parsed_line[1] = s.substr(charpos,tmp - charpos);
    charpos = tmp;
    if (!check_for_next_token(s, charpos))
        return parsed_line;
    
    
    //Continue to operand
    
    spacepos = s.find_first_of(" \t", charpos);
    charpos = s.find_first_not_of(" \t", spacepos);
    if (s[charpos] == '*' || s[charpos] == ';' ){
        parsed_line[3] = s.substr(charpos);
        return parsed_line;
    }
    
    tmp = get_opcode_and_operands(s, spacepos, charpos);
    parsed_line[2] = s.substr(charpos,tmp - charpos);
    charpos = tmp;
    
    if (!check_for_next_token(s, charpos))
        return parsed_line;
    
    //Last token must be a comment
    
    spacepos = s.find_first_of(" \t", charpos);
    charpos = s.find_first_not_of(" \t", spacepos);
    
    try {
        if (s[charpos] != '*' && s[charpos] != ';' ) throw file_parse_exception("Too many tokens on line: ");
        
        parsed_line[3] = s.substr(charpos);
        return parsed_line;
        
    }
    
    catch(file_parse_exception& e) {
        int i;
        ss << numlines;
        ss >> i;
        i++;
        cout << e.getMessage() << i << endl;
        exit(-1);
    }
    
    ss.clear();
    ss.str("");
}

int file_parser::check_for_quotes(string s, int start_pos)  {
    
    int quote_pos = s.find_first_of("'", start_pos); //Checks if quote exists
    int spos = s.find_first_of(" \t", start_pos+1); //Checks for next space
    int pos = s.find_first_of("*;");
    
    if (spos < quote_pos)
        return 0;
    
    if (quote_pos != -1) {	//There is a quote mark
        if (quote_pos > pos && pos != -1)
            return start_pos;
        
        quote_pos = s.find_first_of("'", quote_pos+1);
        
        
        try {			//There doesn't exist a second quote
            if (quote_pos == -1) throw file_parse_exception("Quote mark mismatch on line: ");
            
            spos = s.find_first_of(" \t", quote_pos); //Checking for spaces after the quotes
            
            if (spos != -1)
                return spos;
            
            return s.length();  //If there is no space after this token
            //it is last on the line and its location is equal to s.length
        }
        catch (file_parse_exception& e) {
            int i;
            ss << numlines;
            ss >> i;
            i++;
            cout << e.getMessage() << i << endl;
            exit(-1);
        }
        
    }
    return 0;
}


//Checks to make sure there is a token after the one from the current location.
bool file_parser::check_for_next_token(string s, int pos) {
    
    int tmp = s.find_first_of("\t ", pos);
    if (tmp == -1)
        return false;
    
    tmp = s.find_first_not_of("\t ", tmp+1);
    if (tmp == -1)
        return false;
    
    return true;
}

//Catches and deals with any lines which contain the #minclude on them
vector<string> file_parser::catch_includes(string s) {
    
    int space_loc, char_loc;
    vector<string> tmp_line(4);
    
    tmp_line[2] = "";
    tmp_line[3] = "";
    
    try {
        if(!check_for_next_token(s, 0))	throw file_parse_exception("No path for #minclude on line: ");
        
        space_loc = s.find_first_of(" \t"); //find first space or tab
        tmp_line[0] = s.substr(0, space_loc); //put the substring into first column
        
        if(!check_for_next_token(s, space_loc + 1)) {	//Is there a token after the 2nd one?
            
            char_loc = s.find_first_not_of(" \t", space_loc);
            tmp_line[1] = s.substr(char_loc, s.length() - char_loc);	//Path goes to end of line, return
            
            return tmp_line;
        }
        
    }
    
    catch (file_parse_exception& e) {
        int i;
        ss << numlines;
        ss >> i;
        i++;
        cout << e.getMessage() << i << endl;
        exit(-1);
    }
    
    char_loc = s.find_first_not_of(" \t", space_loc);
    space_loc = s.find_first_of(" \t", char_loc);
    tmp_line[1] = s.substr(char_loc, space_loc - char_loc);
    
    char_loc = s.find_first_not_of(" \t", space_loc);	//Move to 3rd token which SHOULD be a comment, if not throw error
    try {
        if ( s[char_loc] != '*' || s[char_loc] != ';') throw file_parse_exception("Too many tokens on line: ");
        tmp_line[3] = s.substr(char_loc, s.length() - char_loc);
        
        return tmp_line;
    }
    
    catch( file_parse_exception& e ) {
        int i;
        ss << numlines;
        ss >> i;
        i++;
        cout << e.getMessage() << i << endl;
        exit(-1);
    }
}

string file_parser::get_label(string s) {
    
    unsigned int tmp = 0;
    unsigned int pos;
    try {
        
        if(!isalpha(s[0]) && s[0] != '.') throw file_parse_exception("Invalid label start character on line: "); 
    }
    
    catch (file_parse_exception& e) {
        int i;
        ss << numlines;
        ss >> i;
        i++;
        cout << e.getMessage() << i << endl;
        exit(-1);
    }
    
    if(!check_for_next_token(s, tmp)) { //only label exists on the line
        //labels must end with a colon
        
        while(s[tmp] != ':' && s[tmp] != ' ' && s[tmp] != '\t'){
            try {
                if(!isalnum(s[tmp]) && s[tmp] != '@' && s[tmp] != '\\')
                    throw file_parse_exception("Dumb label on line: "); // -------Line #
            }
            
            catch (file_parse_exception& e){
                cout << s[tmp] << endl;
                int i;
                ss << numlines;
                ss >> i;
                i++;
                cout << e.getMessage()  << i << endl;
                break;
            }
            tmp++;
        }
        
    }
    
    pos = s.find_first_of(" \t");
    
    return s.substr(0, pos);
}

int file_parser::get_opcode_and_operands(string s, int spacepos, int charpos) {
    
    int tmp;
    
    tmp = check_for_quotes(s, charpos); //Quotes found in token, grabs location of the end of the whole token
    
    if (tmp != 0)
        return tmp;
    
    spacepos = s.find_first_of(" \t", charpos);
    
    if (!check_for_next_token(s, charpos)) {
        
        if (spacepos == -1)
            return s.length();
        
        tmp = charpos;
        
        while ( tmp != spacepos)
            tmp ++;
        
        return tmp;
    }
				
    return spacepos;		
}
