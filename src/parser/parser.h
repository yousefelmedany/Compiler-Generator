#ifndef PARSER_GENERATOR_H
#define PARSER_GENERATOR_H

// #include "grammar/grammar.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include "../Tokenizer/Tokenizer.h"

class ParserGenerator {
private: 
public:
    static void parse(Tokenizer & tockenizer, std::map<std::string, std::map<std::string, std::vector<std::string>>> parsing_table, std::string start_symbol, const std::string& output_stack, const std::string& output_derivations);
};

#endif