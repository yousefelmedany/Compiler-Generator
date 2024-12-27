#ifndef NFA_H
#define NFA_H
#include <bits/stdc++.h>
using namespace std;
class NFA
{
private:
    map<int, vector<pair<int, char>>> stateTransitions;
    int startState = 0, endState = 0;

public:
    NFA();
    void addTransition(int from, char symbol, int to);     // done
    void createSymbolNFA(char symbol);                     // done
    NFA concatenateNFAs(const NFA &nfa1, const NFA &nfa2); // done
    NFA orNFAs(const NFA &nfa1, const NFA &nfa2);          // done
    NFA kleeneClosureNFA(const NFA &nfa);
    NFA createSetNFA(const set<char> &charSet);
    map<int, vector<pair<int, char>>> get_stateTransitions();
    map<pair<int, char>, vector<int>> get_dfa_state_transitions();
    int get_startState();
    int get_endState();
    vector<char> getAlphabet();
    void export_to_dot(const string &filename);
};

#endif