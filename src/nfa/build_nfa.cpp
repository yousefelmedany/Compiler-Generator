#include "build_nfa.h"
#include "NFA.h"
#include "../input_handler/input_handler.h"
#include <bits/stdc++.h>

using namespace std;

BuildNFA::BuildNFA(vector<string> lines)
{
    this->lines = lines;
    parse();
}
string BuildNFA::trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

void BuildNFA::buildKeywordNFA(string line)
{
    line = line.substr(1, line.size() - 2);
    line = trim(line);
    // split line by space
    istringstream ss(line);
    string word;
    while (ss >> word)
    {
        // create NFA for each keyword and store it in nfas vector
        vector<NFA> keyword_nfas;
        for (char c : word)
        {
            NFA nfa;
            nfa.createSymbolNFA(c);
            keyword_nfas.push_back(nfa);
        }
        // concatenate all NFAs in keyword_nfas vector
        NFA result = keyword_nfas[0];
        for (int i = 1; i < keyword_nfas.size(); i++)
        {
            result = result.concatenateNFAs(result, keyword_nfas[i]);
        }
        kws.push_back(word);
        nfas.push_back(make_pair(result, word));
    }
}

void BuildNFA::buildPunctuationNFA(string line)
{
    line = line.substr(1, line.size() - 2);
    line = trim(line);
    // split line by space
    istringstream ss(line);
    string word;
    while (ss >> word)
    {
        // remove \ from the word if it is there
        if (word[0] == '\\')
        {
            word = word.substr(1, word.size() - 1);
        }
        // create NFA for each punctuation and store it in nfas vector
        NFA nfa;
        nfa.createSymbolNFA(word[0]);
        nfas.push_back(make_pair(nfa, word));
    }
}

void BuildNFA::combine()
{
    // combine all NFAs in nfas vector
    int offset = 1;
    for (auto p : nfas)
    {
        NFA nfa = p.first;
        for (auto &mp_pair : nfa.get_stateTransitions())
        {
            int from = mp_pair.first;
            for (auto &to_pair : mp_pair.second)
            {
                int to = to_pair.first;
                char symbol = to_pair.second;
                finalNFA.addTransition(from + offset, symbol, to + offset);
            }
        }
        // save final state
        finalStates[nfa.get_endState() + offset] = p.second;
        // connect start state of finalNFA to start state of current nfa
        finalNFA.addTransition(0, '~', nfa.get_startState() + offset);
        offset += nfa.get_stateTransitions().size() + 1;
    }
}

vector<string> BuildNFA::tokenize(string regDef) // a-z|A-Z
{
    int regDefLen = regDef.size();
    vector<string> tokens;
    string currToken = "", validToken = "";
    int currIdx = 0, validIdx = -1;
    while (validIdx < regDefLen - 1)
    {
        if (currIdx == regDefLen)
        {
            tokens.push_back(validToken);
            validToken = "";
            currToken = "";
            currIdx = validIdx + 1;
        }
        string ch = string(1, regDef[currIdx]);
        if (operators.count(ch) && validToken.empty())
        {
            validIdx = currIdx;
            currIdx++;
            tokens.push_back(ch);
        }
        else if (ch == "\\" && validToken.empty())
        {
            tokens.push_back(regDef.substr(currIdx, 2));
            validIdx = currIdx + 1;
            currIdx += 2;
        }
        else if (operators.count(ch) || ch == "\\")
        {
            tokens.push_back(validToken);
            validToken = "";
            currToken = "";
            currIdx = validIdx + 1;
        }
        else
        {
            currToken += ch;
            currIdx++;
            if (!regDefs[currToken].empty() || currToken.size() == 1)
            {
                validToken = currToken;
                validIdx = currIdx - 1;
            }
        }
    }
    if (!validToken.empty())
    {
        tokens.push_back(validToken);
    }
    return tokens;
}

vector<string> BuildNFA::transformToCanonicalReg(vector<string> tokens)
{
    vector<string> canonicalTokens;
    int tokensLen = tokens.size();
    for (int i = 0; i < tokensLen; i++)
    {
        if (tokens[i] == "-")
        {
            if (i == 0 || i == tokensLen - 1)
            {
                throw runtime_error("Invalid Position for -");
            }

            // figure out whether operands are regular expressions or regular definitions.
            // operands of - must be symbols
            string start = tokens[i - 1], end = tokens[i + 1];
            if (regDefs[start].size() == 1)
                start = regDefs[start];
            else if (start.size() != 1)
                throw runtime_error("invalid first operand for -");
            if (regDefs[end].size() == 1)
                end = regDefs[end];
            else if (end.size() != 1)
            {
                throw runtime_error("invalid second operand for -");
            }

            if (start[0] > end[0])
                throw runtime_error("Invalid Range for -");
            else if (start[0] == end[0])
                i++; // skip end token
            else
            {
                canonicalTokens.push_back("|");
                for (char ch = start[0] + 1; ch < end[0]; ch++)
                {
                    canonicalTokens.push_back(string(1, ch));
                    canonicalTokens.push_back("|");
                }
            }
        }
        else
            canonicalTokens.push_back(tokens[i]);
    }
    return canonicalTokens;
}

vector<string> BuildNFA::addConcatSymbol(vector<string> tokens)
{
    int n = tokens.size();
    vector<string> modifiedTokens;
    modifiedTokens.push_back(tokens[0]);
    for (int i = 1; i < n; i++)
    {
        if ((!operators.count(tokens[i]) && !operators.count(tokens[i - 1])) || (!operators.count(tokens[i]) && tokens[i - 1] == ")") || (tokens[i] == "(" && !operators.count(tokens[i - 1])) || (!operators.count(tokens[i]) && tokens[i - 1] == "*") || (!operators.count(tokens[i]) && tokens[i - 1] == "+") || (tokens[i] == "(" && tokens[i - 1] == ")") || (tokens[i] == "(" && tokens[i - 1] == "+") || (tokens[i] == "(" && tokens[i - 1] == "*"))
        {
            modifiedTokens.push_back(" ");
        }
        modifiedTokens.push_back(tokens[i]);
    }
    return modifiedTokens;
}
string BuildNFA::removeSpaces(string inStr)
{
    string outStr = "";
    for (auto ch : inStr)
    {
        if (ch == ' ')
            continue;
        outStr += ch;
    }
    return outStr;
}

NFA BuildNFA::makeNFA(vector<string> tokens)
{
    stack<NFA> nfaStack; // To hold partial NFAs
    stack<char> opStack; // To hold operators

    // Helper function to apply an operator
    auto applyOperator = [&]()
    {
        char op = opStack.top();
        opStack.pop();

        if (op == '|')
        { // OR
            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            nfaStack.push(nfa1.orNFAs(nfa1, nfa2));
        }
        else if (op == '*')
        { // Kleene Star
            NFA nfa = nfaStack.top();
            nfaStack.pop();
            nfaStack.push(nfa.kleeneClosureNFA(nfa));
        }
        else if (op == '+')
        { // One or more (a+ = aa*)
            NFA nfa = nfaStack.top();
            nfaStack.pop();
            nfaStack.push(nfa.concatenateNFAs(nfa, nfa.kleeneClosureNFA(nfa)));
        }
        else if (op == ' ')
        { // Concatenation
            NFA nfa2 = nfaStack.top();
            nfaStack.pop();
            NFA nfa1 = nfaStack.top();
            nfaStack.pop();
            nfaStack.push(nfa1.concatenateNFAs(nfa1, nfa2));
        }
    };

    for (auto token : tokens)
    {
        if (!regDefs[token].empty())
            nfaStack.push(regDefNFAs[token]);
        else if (token == "(")
        {
            opStack.push(token[0]);
        }
        else if (token == ")")
        {
            while (!opStack.empty() && opStack.top() != '(')
            {
                applyOperator();
            }
            opStack.pop(); // Pop '('
        }
        else if (token == "|")
        {
            while (!opStack.empty() && (opStack.top() == '*' || opStack.top() == '+' || opStack.top() == ' '))
            {
                applyOperator();
            }
            opStack.push('|');
        }
        else if (token == "*")
        {
            opStack.push('*');
        }
        else if (token == "+")
        {
            opStack.push('+');
        }
        else if (token == " ")
        {
            opStack.push(' ');
        }
        else
        { // alphabet symbol
            assert(token.size() == 1 || (token.size() == 2 && token[0] == '\\'));
            NFA nfa;
            if (token.size() == 1)
                nfa.createSymbolNFA(token[0]);
            else if (token == "\\L")
                nfa.createSymbolNFA('~');
            else if (token[0] == '\\')
                nfa.createSymbolNFA(token[1]);
            nfaStack.push(nfa);
        }
    }

    // Apply remaining operators
    while (!opStack.empty())
    {
        applyOperator();
    }

    return nfaStack.top(); // Final NFA
}

void BuildNFA::buildRegDefNFA(string line, int equalIdx)
{
    string name = line.substr(0, equalIdx);
    string regDef = line.substr(equalIdx + 1);
    vector<string> tokens = addConcatSymbol(transformToCanonicalReg(tokenize(regDef)));
    NFA nfa = makeNFA(tokens);
    regDefNFAs[name] = nfa;
}

void BuildNFA::buildRegExpNFA(string line, int colonIdx)
{
    string name = line.substr(0, colonIdx);
    string regExp = line.substr(colonIdx + 1);
    vector<string> tokens = addConcatSymbol(transformToCanonicalReg(tokenize(regExp)));
    NFA nfa = makeNFA(tokens);
    nfas.push_back(make_pair(nfa, name));
}

void BuildNFA::parse()
{
    for (string line : lines)
    {
        // if line is containes keywords
        if (line[0] == '{' && line[line.size() - 1] == '}')
        {
            buildKeywordNFA(line);
        }
        else if (line[0] == '[' && line[line.size() - 1] == ']')
        {
            buildPunctuationNFA(line);
        }
        else
        {
            line = removeSpaces(line);
            int equalIndex = line.find('=');
            int colonIndex = line.find(':');
            // regular definition
            if ((equalIndex != -1 && colonIndex != -1 && equalIndex < colonIndex) || (equalIndex != -1 && colonIndex == -1))
            {
                string name = line.substr(0, equalIndex);
                string regDef = line.substr(equalIndex + 1);
                regDefs[name] = regDef;
                buildRegDefNFA(line, equalIndex);
            }
            // regular expression
            else if ((equalIndex != -1 && colonIndex != -1 && colonIndex < equalIndex) || (colonIndex != -1 && equalIndex == -1))
            {
                buildRegExpNFA(line, colonIndex);
            }
            else
            {
                throw runtime_error("Invalid Line");
            }
        }
    }
    combine();
}

vector<NFA> BuildNFA::getNfas()
{
    vector<NFA> result;
    for (auto p : nfas)
        result.push_back(p.first);
    return result;
}
NFA BuildNFA::getFinalNFA()
{
    return finalNFA;
}
map<int, string> BuildNFA::getFinalStates()
{
    sort_final_states();
    return finalStates;
}

map<int, int> BuildNFA::getPriority()
{
    return priority;
}

void BuildNFA::sort_final_states()
{
    int priority_kws = 0;
    int priority_normal=kws.size();
    for (auto &p : finalStates)
    {
        if (find(kws.begin(), kws.end(), p.second) != kws.end())
        {
            this->priority[p.first] = priority_kws;
            priority_kws++;
        }
        else
        {
            this->priority[p.first] = priority_normal;
            priority_normal++;
        }

    }

}