#ifndef BUILD_NFA_H
#define BUILD_NFA_H
#include <bits/stdc++.h>
#include "NFA.h"

using namespace std;
class BuildNFA
{
private:
    const set<string> operators = {"|", " ", "*", "+", "-", "(", ")"};
    map<string, string> regDefs;
    map<string, NFA> regDefNFAs;
    map<int, string> finalStates;
    vector<pair<NFA, string>> nfas;
    vector<string> lines;
    vector<string>kws;
    NFA finalNFA;
    string trim(const string &str);
    void parse();
    set<char> expandRange(const string &range);
    void sort_final_states();

    map<int, int> priority;

public:
    BuildNFA(vector<string> lines);
    void buildKeywordNFA(string line);
    void buildPunctuationNFA(string line);
    void buildRegDefNFA(string line, int equalIdx);
    void buildRegExpNFA(string line, int colonIdx);
    NFA makeNFA(vector<string> tokens);
    string removeSpaces(string inStr);
    void combine();
    vector<string> tokenize(string regDef);
    vector<string> transformToCanonicalReg(vector<string> tokens);
    vector<string> addConcatSymbol(vector<string> tokens);
    // getters
    vector<NFA> getNfas();
    NFA getFinalNFA();
    map<int, string> getFinalStates();
    map<int,int> getPriority();
};

#endif