#ifndef DFA_H
#define DFA_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <fstream>
#include <algorithm>


using namespace std;

class DFA {

private:
    map<pair<int,char>,vector<int>> transition_map;
    vector<int> final_states;
    vector<char> alphabet;
    int start_state;
    map<pair<int,char>,int> transition_map_dfa;
    vector<int> final_states_dfa;
    map<set<int>,int> state_map;
    map<int,string> final_states_ids;
    set<int> epsilon_closure(set<int> state);
    void sub_set_construction();
    void minimize();

    map<int,int> priority;
public:
    DFA(const map<pair<int,char>,vector<int>>& transition_map, const vector<char>& alphabet, const int& start_state, const vector<int>& final_states,map<int,string> final_states_ids, map<int,int> priority);
    void print();
    void export_to_dot(const string& filename);
    void save(const string& filename);
    bool is_final_state(const int& state);
    int get_start_state();
    int get_next_state(const int& state, const char& symbol);
    string get_final_state_id(const int& state);
};

#endif // DFA_H