#include "NFA.h"
#include <bits/stdc++.h>

using namespace std;

NFA::NFA(){}

void NFA::addTransition(int from,char symbol,int to){
    stateTransitions[from].push_back(make_pair(to,symbol));
}

void NFA::createSymbolNFA(char symbol){
    endState=1;
    addTransition(0,symbol,1);   
}
map<int, vector<pair<int, char>>> NFA::get_stateTransitions(){
    return stateTransitions;
}

map<pair<int,char>,vector<int>> NFA::get_dfa_state_transitions(){
    map<pair<int,char>,vector<int>> dfa_state_transitions;
    for(auto &mp_pair:stateTransitions){
        int from = mp_pair.first;
        for(auto &to_pair:mp_pair.second){
            int to = to_pair.first;
            char symbol = to_pair.second;
            dfa_state_transitions[make_pair(from,symbol)].push_back(to);
        }
    }
    return dfa_state_transitions;
}

vector<char> NFA::getAlphabet(){
    vector<char> alphabet;
    for(auto &mp_pair:stateTransitions){
        for(auto &to_pair:mp_pair.second){
            char symbol = to_pair.second;
            if(symbol!='~'){
                alphabet.push_back(symbol);
            }
        }
    }
    return alphabet;
}


void NFA::export_to_dot(const string& filename) {

    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    outfile << "digraph NFA {" << endl;
    outfile << "rankdir=LR;" << endl;
    outfile << "size=\"8,5\"" << endl;
    outfile << "node [shape = doublecircle]; " << endState << ";" << endl;
    outfile << "node [shape = point ]; start" << endl;
    outfile << "node [shape = circle];" << endl;
    outfile << "start -> 0;" << endl;

    for (auto &mp_pair : stateTransitions) {
        int from = mp_pair.first;
        for (auto &to_pair : mp_pair.second) {
            int to = to_pair.first;
            char symbol = to_pair.second;
            outfile << from << " -> " << to << " [ label = \"" << symbol << "\" ]" << endl;
        }
    }

    outfile << "}" << endl;
    outfile.close();
    cout << "NFA exported to " << filename << endl;

}





NFA NFA::orNFAs(const NFA &nfa1,const NFA &nfa2){
    NFA result;
    int offset1 = 1;
    int offset2 = nfa1.stateTransitions.size() + offset1+1;
    // copy all transitions of nfa1 to result nfa
    for(auto &mp_pair:nfa1.stateTransitions){
        int from =mp_pair.first;
        for(auto &to_pair:mp_pair.second){
           int to = to_pair.first;
           char symbol=to_pair.second;
           result.addTransition(from+offset1,symbol,to+offset1);
        }
    }

    // copy all transitions of nfa2 to result nfa
    for(auto &mp_pair:nfa2.stateTransitions){
        int from =mp_pair.first;
        for(auto &to_pair:mp_pair.second){
           int to = to_pair.first;
           char symbol=to_pair.second;
           result.addTransition(from+offset2,symbol,to+offset2);
        }
    }
    result.endState=nfa2.endState+offset2+1;
    //connect start state of result to two start states of nfa1 and nfa2
    result.addTransition(0,'~',nfa1.startState+offset1);
    result.addTransition(0,'~',nfa2.startState+offset2);
    result.addTransition(nfa1.endState+offset1,'~',result.endState);
    result.addTransition(nfa2.endState+offset2,'~',result.endState);

    return result;
}
NFA NFA::concatenateNFAs(const NFA &nfa1, const NFA &nfa2){
    NFA result;
    int offset = nfa1.stateTransitions.size();
    // copy all transitions of nfa1 to result nfa
    for(auto &mp_pair:nfa1.stateTransitions){
        int from =mp_pair.first;
        for(auto &to_pair:mp_pair.second){
           int to = to_pair.first;
           char symbol=to_pair.second;
           result.addTransition(from,symbol,to);
        }
    }

    // copy all transitions of nfa2 to result nfa
    for(auto &mp_pair:nfa2.stateTransitions){
        int from =mp_pair.first;
        for(auto &to_pair:mp_pair.second){
           int to = to_pair.first;
           char symbol=to_pair.second;
           result.addTransition(from+offset,symbol,to+offset);
        }
    }
    result.endState=nfa2.endState+offset;
    return result;
}
NFA NFA::kleeneClosureNFA(const NFA &nfa){
    NFA result;
    int offset=1;

    for(auto &mp_pair:nfa.stateTransitions){
        int from =mp_pair.first;
        for(auto &to_pair:mp_pair.second){
           int to = to_pair.first;
           char symbol=to_pair.second;
           result.addTransition(from+offset,symbol,to+offset);
        }
    }

    result.endState=nfa.endState+offset+1;
    result.addTransition(0,'~',nfa.startState+offset);
    result.addTransition(result.startState,'~',result.endState);
    result.addTransition(nfa.endState+offset,'~',result.startState+offset);
    result.addTransition(nfa.endState+offset,'~',result.endState);

    return result;
}

// Helper Function to Create an NFA from a Character Set
NFA NFA::createSetNFA(const set<char> &charSet) {
    NFA nfa;
    for (char c : charSet) {
        nfa.addTransition(0, c, 1);
    }
    return nfa;
}

int NFA::get_startState(){
    return startState;
}
int NFA::get_endState(){
    return endState;
}
