#ifndef PARSING_TABLE_H
#define PARSING_TABLE_H
#include <bits/stdc++.h>
// #include "grammar/grammar.h"
#define EPSILON "\\L"
#define DOLLAR "$"

using namespace std;

class ParsingTable
{
private:
    vector<pair<string, string>> productions;
    vector<pair<string,set<string>>> productions_set;
    map<string, set<pair<string, string>>> firstSet;
    map<string, set<string>> followSet;
    void FirstSetHelper(string LHS,string sub_production);
    void followSetHelper(string LHS);
    void elimnateLeftRecursion();
    map<string, map<string, vector<string>>> parsing_table;
    set<string> getFirstOfString(const string& str);
    void leftFactoring();

public:
    ParsingTable();
    void readGrammar();
    void computeFirstSet();
    void computeFollowSet();
    bool isLL1Grammar();

    // getters
    vector<pair<string, string>> getProductions();
    map<string, set<pair<string, string>>> getFirstSet();
    map<string, set<string>> getFollowSet();
    map<string, map<string, vector<string>>> getparsingtable();
    void generateParser();
    void printParsingTable() const;
    void printParsingTableToCSV(const string& filename);

};

#endif // PARSING_TABLE_H