/*
 * karan
 * CS280
 * Fall 2019
 */

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "lex.h"
#include "parse.h"
#include "parsetreeSKEL.h"

using namespace std;
int line = 0;
istream *input = &cin;
ifstream inFile;

int main(int argc, const char * argv[]) {
    
    for(int i=1; i<argc; i++){        
        string vari(argv[i]);        
        if( i == argc-1 ) { 
            inFile.open(vari);
            if( inFile.is_open() == false ) {
                cerr << "COULD NOT OPEN "<< vari << endl;
                return -1;
            }
            input = &inFile;
        }        
        else{            
            cerr << "TOO MANY FILENAMES" <<endl;
            return -1;
        }       
    }    
    ParseTree *tree = Prog(*input,line);
    if( tree == 0 ) {    
        return 0;
    } 
        
        cout<<"BANG COUNT: " << tree->BangCount()<<endl;                  
        cout<<"MAX DEPTH: " << tree->MaxDepth()<<endl;
            
}


