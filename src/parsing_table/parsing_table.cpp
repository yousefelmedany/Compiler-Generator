#include "parsing_table.h"
#include <bits/stdc++.h>
using namespace std;

ParsingTable::ParsingTable()
{
    productions = vector<pair<string, string>>();
}
string trim(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
void ParsingTable::FirstSetHelper(string LHS, string sub_production)
{
    istringstream stream(sub_production);
    string token;
    while (stream >> token)
    {
        bool isTerminal = (token[0] == '\'');
        if (isTerminal || token == EPSILON)
        {
             
            if (isTerminal)
                token = token.substr(1, token.size() - 2);

             
            firstSet[LHS].emplace(make_pair(token, sub_production));
            return;
        }
        else
        {  
            if (token == LHS)
            {
                return;
            }
             
            set<pair<string, string>> firstSetOfToken = firstSet[token];
            bool hasEpsilon = false;
            for (auto p : firstSetOfToken)
            {
                firstSet[LHS].emplace(make_pair(p.first, sub_production));
                if (p.first == EPSILON)
                {
                    hasEpsilon = true;
                }
            }
            if (!hasEpsilon)
            {
                return;
            }
        }
    }
}

void ParsingTable::followSetHelper(string currNT)
{
    int n = productions.size();
    for (auto p : productions)
    {
        string RHS = p.second;
        istringstream stream(RHS);
        string token;
        while (stream >> token)
        {
            if (token == currNT)
            {
                 
                string nextToken;
                if (stream >> nextToken && nextToken != "|")
                {
                    bool isTerminal = (nextToken[0] == '\'');
                    if (isTerminal)
                        followSet[currNT].insert(nextToken.substr(1, nextToken.size() - 2));

                    else
                    {
                        set<pair<string, string>> firstSetOfToken = firstSet[nextToken];
                        bool hasEpsilon = false;
                        for (auto pp : firstSetOfToken)
                        {
                            if (pp.first != EPSILON)
                            {
                                followSet[currNT].insert(pp.first);
                            }
                            else
                            {
                                hasEpsilon = true;
                            }
                        }
                        if (hasEpsilon)
                        {
                            string LHS = p.first;
                            if (LHS != currNT)
                            {
                                set<string> followSetOfLHS = followSet[LHS];
                                for (auto s : followSetOfLHS)
                                {
                                    followSet[currNT].insert(s);
                                }
                            }
                        }
                    }
                }
                 
                else
                {
                    string LHS = p.first;
                    if (LHS != currNT)
                    {
                        set<string> followSetOfLHS = followSet[LHS];
                        for (auto s : followSetOfLHS)
                        {
                            followSet[currNT].insert(s);
                        }
                    }
                }
            }
        }
    }
}
void ParsingTable::readGrammar()
{
    ifstream file("input/grammar.txt");  
    if (!file.is_open())
    {  
        cerr << "Error: Could not open file." << std::endl;
        return;
    }
    string LHS = "";
    string RHS = "";
    string line;
     
    getline(file, line);
    LHS = line.substr(line.find("#") + 1, line.find("=") - line.find("#") - 1);
    RHS = line.substr(line.find("=") + 1);
    while (getline(file, line))
    {
        int index = line.find("#");
        int equalIndex = line.find("=");
        if (index != string::npos)
        {
            RHS += " " + line.substr(0, index);
            RHS = trim(RHS);
            LHS = trim(LHS);
            productions.push_back(make_pair(LHS, RHS));
            LHS = line.substr(index + 1, equalIndex - index - 1);
            RHS = line.substr(equalIndex + 1);
        }
        else
        {
            RHS += " " + line;
        }
    }
    RHS = trim(RHS);
    LHS = trim(LHS);
    productions.push_back(make_pair(LHS, RHS));

    file.close();
    // apply left recursion elimination
    elimnateLeftRecursion();
    leftFactoring();
    ofstream file_productions1("out/productions.txt");
    for (auto p : productions)
    {
        file_productions1 << p.first << " -> " << p.second << endl;
    }
    file_productions1.close();

}

void ParsingTable::computeFirstSet()
{
     
    int n = productions.size();
    for (int i = n - 1; i >= 0; i--)
    {
        pair<string, string> production = productions[i];
        string LHS = production.first;
        string RHS = production.second;
         
        istringstream stream(RHS);
        string sub_production;
        while (getline(stream, sub_production, '|'))
        {
             
            FirstSetHelper(LHS, sub_production);
        }
    }
}
void ParsingTable::computeFollowSet()
{
    int n = productions.size();
    for (int i = 0; i < n; i++)
    {
        pair<string, string> p = productions[i];
        string LHS = p.first;
        if (i == 0)
        {
            followSet[LHS].insert("$");
        }
        followSetHelper(LHS);
    }
}
pair<bool, string> checkLeftRecursion(string LHS, string sub_production)
{

    istringstream sub_stream(sub_production);
    string token;
    sub_stream >> token;
    if (token == LHS)
    {
        ostringstream resultStream;
        while (sub_stream >> token)
        {
            resultStream << token << " "; // Append tokens that are not LHS
        }
        return make_pair(true, resultStream.str());
    }

    return make_pair(false, "");
}
void ParsingTable::elimnateLeftRecursion()
{   // loop over productions
    for (int i = 0; i < productions.size(); i++)
    {
        auto production = productions[i];

        string LHS = production.first;
        string RHS = production.second;
        vector<string> betas;
        vector<string> alphas;
        string newLHS = LHS + "'";
        // split RHS into sub productions by '|'
        istringstream stream(RHS);
        string sub_production;
        while (getline(stream, sub_production, '|'))
        {
            pair<bool, string> result = checkLeftRecursion(LHS, sub_production);
            if (result.first)
            {
                alphas.push_back(result.second);
            }
            else
            {
                betas.push_back(sub_production);
            }
        }
        if (!alphas.empty()) // there is left recusrion
        {
            string newRHS = "";
            for (auto beta : betas)
            {
                newRHS += beta + " " + newLHS + " | ";
            }
            // remove last " | "
            newRHS = newRHS.substr(0, newRHS.size() - 3);
            productions[i] = make_pair(LHS, newRHS);

            // add new production
            string newRHS1 = "";
            for (auto alpha : alphas)
            {
                newRHS1 += alpha + " " + newLHS + " | ";
            }
            newRHS1 += EPSILON;
            productions.insert(productions.begin() + i + 1, make_pair(newLHS, newRHS1));
        }
    }
}

vector<pair<string, string>> ParsingTable::getProductions()
{
    return productions;
}
map<string, set<pair<string, string>>> ParsingTable::getFirstSet()
{
    return firstSet;
}
map<string, set<string>> ParsingTable::getFollowSet()
{
    return followSet;
}
map<string, map<string, vector<string>>> ParsingTable::getparsingtable()
{
    return parsing_table;
}


string findLongestCommonPrefix(const vector<string>& strs) {
    if (strs.empty()) return "";
    
    vector<vector<string>> tokenized;
    for (const string& str : strs) {
        vector<string> tokens;
        istringstream iss(str);
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        tokenized.push_back(tokens);
    }
    
    string result;
    size_t minLen = tokenized[0].size();
    for (const auto& tokens : tokenized) {
        minLen = min(minLen, tokens.size());
    }
    
    for (size_t i = 0; i < minLen; i++) {
        string current = tokenized[0][i];
        for (size_t j = 1; j < tokenized.size(); j++) {
            if (tokenized[j][i] != current) {
                return result;
            }
        }
        if (!result.empty()) result += " ";
        result += current;
    }
    
    return result;
}
void ParsingTable::leftFactoring() {
    bool changed;
    do {
        changed = false;
        vector<pair<string, string>> new_productions;
        
        for (const auto& p : productions) {
            string LHS = p.first;
            string RHS = p.second;
            
             
            vector<string> alternatives;
            istringstream stream(RHS);
            string sub_production;
            while (getline(stream, sub_production, '|')) {
                alternatives.push_back(trim(sub_production));
            }
            
             
            map<string, vector<string>> prefixGroups;
            for (const string& alt : alternatives) {
                istringstream ss(alt);
                string firstSymbol;
                ss >> firstSymbol;
                prefixGroups[firstSymbol].push_back(alt);
            }
            
            bool factored = false;
             
            for (const auto& group : prefixGroups) {
                if (group.second.size() > 1) {
                     
                    string commonPrefix = findLongestCommonPrefix(group.second);
                    if (!commonPrefix.empty()) {
                        factored = true;
                        changed = true;
                        
                         
                        string newNonTerminal = LHS + "'";
                        
                         
                        string newRHS = commonPrefix + " " + newNonTerminal;
                        new_productions.push_back({LHS, newRHS});
                        
                         
                        string suffixRHS;
                        bool first = true;
                        for (const string& alt : group.second) {
                            string suffix = alt.substr(commonPrefix.length());
                            suffix = trim(suffix);
                            if (suffix.empty()) suffix = EPSILON;
                            
                            if (first) {
                                suffixRHS = suffix;
                                first = false;
                            } else {
                                suffixRHS += " | " + suffix;
                            }
                        }
                        new_productions.push_back({newNonTerminal, suffixRHS});
                    }
                }
            }
            
             
            if (!factored) {
                new_productions.push_back(p);
            }
        }
        
        productions = new_productions;
    } while (changed);
}

set<string> ParsingTable::getFirstOfString(const string& str) {
    set<string> result;
    istringstream iss(str);
    string symbol;
    
    bool allCanBeEmpty = true;
    
    while (iss >> symbol) {
        if (symbol[0] == '\'') {    
            result.insert(symbol.substr(1, symbol.length()-2));
            return result;
        } else {    
            bool hasEpsilon = false;
            for (const auto& firstItem : firstSet[symbol]) {
                if (firstItem.first == EPSILON) {
                    hasEpsilon = true;
                } else {
                    result.insert(firstItem.first);
                }
            }
            if (!hasEpsilon) {
                allCanBeEmpty = false;
                break;
            }
        }
    }
    
    if (allCanBeEmpty) {
        result.insert(EPSILON);
    }
    return result;
}
void ParsingTable::generateParser() {
    computeFirstSet();
    computeFollowSet();


     
    set<string> terminals;
    for (const auto& production : productions) {
        istringstream stream(production.second);
        string token;
        while (stream >> token) {
            if (token[0] == '\'') {
                terminals.insert(token.substr(1, token.length()-2));
            }
        }
    }
    
     
    for (const auto& production : productions) {
        string nonTerminal = production.first;
         
        for (const auto& terminal : terminals) {
            parsing_table[nonTerminal][terminal] = vector<string>();
        }
         
        parsing_table[nonTerminal][DOLLAR] = vector<string>();
    }

     
    for (const auto& production : productions) {
        string nonTerminal = production.first;
        bool hasEpsilon = false;
        if (production.second.find(EPSILON) != string::npos) 
        {
            hasEpsilon = true;
        }
        
        for (const auto& follow : followSet[nonTerminal]) {
            if (parsing_table[nonTerminal][follow].empty()) {
                if (hasEpsilon) {
                    parsing_table[nonTerminal][follow] = vector<string>{"EPSILON"};
                } else {
                    parsing_table[nonTerminal][follow] = vector<string>{"SYNCH"};
                }
            }
        }
        if (followSet[nonTerminal].find(DOLLAR) != followSet[nonTerminal].end() && 
            parsing_table[nonTerminal][DOLLAR].empty()) {
                if (hasEpsilon) {
                    parsing_table[nonTerminal][DOLLAR] = vector<string>{"EPSILON"};
                } else {
                    parsing_table[nonTerminal][DOLLAR] = vector<string>{"SYNCH"};
                }
        }
    }
    
     
    for (const auto& production : productions) {
        string LHS = production.first;
        string RHS = production.second;
        istringstream stream(RHS);
        string subProduction;
        
        while (getline(stream, subProduction, '|')) {
            subProduction = trim(subProduction);
            set<string> firstOfRHS = getFirstOfString(subProduction);
            
             
            istringstream tokenStream(subProduction);
            vector<string> rhs;
            string token;
            while (tokenStream >> token) {
                rhs.push_back(token);
            }

            for (const auto& terminal : firstOfRHS) {
                if (terminal != EPSILON) {
                     
                    if (!parsing_table[LHS][terminal].empty() && 
                        parsing_table[LHS][terminal][0] != "SYNCH") {
                        cout << "Grammar is not LL(1): Conflict at " << LHS << " -> " << terminal << endl;
                        return;
                    }
                    parsing_table[LHS][terminal] = rhs;
                }
            }
            
             
            if (firstOfRHS.find(EPSILON) != firstOfRHS.end()) {
                for (const auto& follow : followSet[LHS]) {
                    if (!parsing_table[LHS][follow].empty() && 
                        parsing_table[LHS][follow][0] != "SYNCH") {
                        cout << "Grammar is not LL(1): Conflict at " << LHS << " -> " << follow << endl;
                        return;
                    }
                    if (rhs.size() == 1 && rhs[0] == EPSILON) {
                        parsing_table[LHS][follow] = rhs;   
                    } else {
                        parsing_table[LHS][follow] = rhs;   
                    }
                }
                
                 
                if (followSet[LHS].find(DOLLAR) != followSet[LHS].end()) {
                    if (!parsing_table[LHS][DOLLAR].empty() && 
                        parsing_table[LHS][DOLLAR][0] != "SYNCH") {
                        cout << "Grammar is not LL(1): Conflict at " << LHS << " -> " << DOLLAR << endl;
                        return;
                    }
                    if (rhs.size() == 1 && rhs[0] == EPSILON) {
                        parsing_table[LHS][DOLLAR] = rhs;   
                    } else {
                        parsing_table[LHS][DOLLAR] = rhs;   
                    }
                }
            }
        }
    }
}

void ParsingTable::printParsingTableToCSV(const string& filename ) {
    ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        cerr << "Error: Could not open file " << filename << " for writing." << endl;
        return;
    }

     
    set<string> terminals;
    for (const auto& row : parsing_table) {
        for (const auto& col : row.second) {
            terminals.insert(col.first);
        }
    }

     
    csvFile << "Non-Terminal,";
    for (const auto& terminal : terminals) {
        csvFile << terminal;
        if (terminal != *terminals.rbegin()) {
            csvFile << ",";
        }
    }
    csvFile << "\n";

     
    for (const auto& row : parsing_table) {
        csvFile << row.first << ",";   
        
        auto terminalIt = terminals.begin();
        for (size_t i = 0; i < terminals.size(); ++i) {
            if (row.second.find(*terminalIt) != row.second.end()) {
                const auto& production = row.second.at(*terminalIt);
                if (production.empty()) {
                    csvFile << "";
                } else
                if (production[0] == "SYNCH") {
                    csvFile << "SYNCH";
                } else {
                     
                    csvFile << row.first << " -> ";
                    for (size_t j = 0; j < production.size(); ++j) {
                        csvFile << production[j];
                        if (j < production.size() - 1) {
                            csvFile << " ";
                        }
                    }
                }
            }
            
            if (i < terminals.size() - 1) {
                csvFile << ",";
            }
            
            ++terminalIt;
        }
        csvFile << "\n";
    }

    csvFile.close();
    cout << "Parsing table has been written to " << filename << endl;
}
void ParsingTable::printParsingTable() const {
    cout << "Parsing Table:" << endl;
    
     
    set<string> terminals;
    for (const auto& row : parsing_table) {
        for (const auto& col : row.second) {
            terminals.insert(col.first);
        }
    }
    
     
    cout << setw(15) << "";
    for (const auto& terminal : terminals) {
        cout << setw(15) << terminal;
    }
    cout << endl;
    
     
    cout << string(15 + terminals.size() * 15, '-') << endl;
    
     
    for (const auto& row : parsing_table) {
        cout << setw(15) << row.first;
        for (const auto& terminal : terminals) {
            if (row.second.find(terminal) != row.second.end()) {
                string entry;
                const auto& production = row.second.at(terminal);
                if (production.empty()) {
                    cout << setw(15) << "";
                } else
                if (production[0] == "SYNCH") {
                    entry = "SYNCH";
                } else {
                    for (const auto& symbol : production) {
                        entry += symbol + " ";
                    }
                }
                cout << setw(15) << entry;
            } else {
                cout << setw(15) << "";
            }
        }
        cout << endl;
    }
}

bool ParsingTable::isLL1Grammar() {
     
    for (const auto& row : parsing_table) {
        for (const auto& col : row.second) {
            if (col.second.size() > 1) {
                return false;
            }
        }
    }
    return true;
}