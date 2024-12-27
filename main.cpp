#include <iostream>
#include <bits/stdc++.h>
#include "src/dfa/dfa.h"
#include "src/nfa/NFA.h"
#include "src/nfa/build_nfa.h"
#include "src/input_handler/input_handler.h"
#include "src/Tokenizer/Tokenizer.h"
#include "src/parsing_table/parsing_table.h"
#include "src/parser/parser.h"

using namespace std;
#include <fstream> 
int main()
{  

    InputHandler input_handler;
    input_handler.read_file();
    vector<string> lines = input_handler.get_lines();
    BuildNFA build_nfa(lines);

    map<int,string> finalStates=build_nfa.getFinalStates();
    map<pair<int,char>,vector<int>> transition_map = build_nfa.getFinalNFA().get_dfa_state_transitions();
    vector<char> alphabet = build_nfa.getFinalNFA().getAlphabet();
    int start_state = build_nfa.getFinalNFA().get_startState();
    vector<int> final_states;
    for(auto p:finalStates){
        final_states.push_back(p.first);
    }

    map<int,int> priority = build_nfa.getPriority();
    DFA dfa(transition_map,alphabet,start_state,final_states,finalStates,priority);
    dfa.save("out\\dfa.csv");
    dfa.export_to_dot("out\\dfa.dot");
    Tokenizer tokenizer(&dfa, "input\\input.txt");
    ParsingTable parsing_table;
    parsing_table.readGrammar();
    parsing_table.generateParser();
    parsing_table.printParsingTableToCSV("out\\parsing_table.csv");


    ParserGenerator parser;
    parser.parse(tokenizer, parsing_table.getparsingtable(), "METHOD_BODY", "out\\Stack.txt", "out\\derivations.txt");


    return 0;
}