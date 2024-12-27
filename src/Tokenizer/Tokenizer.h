#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../dfa/dfa.h"

class Tokenizer {
private:
    DFA* dfa;
    string input_program;
    size_t current_position;
    map<string, string> symbol_map;
    
    string ExtractProgram(const string &inputfile);

public:
    Tokenizer(DFA *dfa, const string &inputfile);
    
    // Core tokenizing methods
    string getNextToken();
    bool hasMoreTokens();
    string peekNextToken();
    
    // Utility methods
    void reset();
    map<string, string> getSymbolMap();
};

#endif