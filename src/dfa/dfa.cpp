#include "dfa.h"
#include <algorithm>


DFA::DFA(const map<pair<int,char>,vector<int>>& transition_map, const vector<char>& alphabet, const int& start_state, const vector<int>& final_states,map<int,string> final_states_ids,map<int,int> priority) {
    this->transition_map = transition_map;
    this->alphabet = alphabet;
    this->start_state = start_state;
    this->final_states = final_states;
    this->final_states_ids = final_states_ids;
    this->priority = priority;
    sub_set_construction();
    minimize();

}


void DFA::sub_set_construction() {
    queue<set<int>> q;  
    set<int> start_set_epsilon = epsilon_closure({this->start_state});  
    q.push(start_set_epsilon);

    map<pair<int, char>, int> transition_map_dfa;  
    map<set<int>, int> state_map;  
    state_map[start_set_epsilon] = 0;  
    vector<int> final_states_dfa;  
    vector<set<int>> final_states_sets;  

    while (!q.empty()) {
        set<int> current_state = q.front();
        q.pop();

         
        for (int final_state : final_states) {
            if (current_state.find(final_state) != current_state.end()) {
                if (find(final_states_dfa.begin(), final_states_dfa.end(), state_map[current_state]) == final_states_dfa.end()) {
                    final_states_dfa.push_back(state_map[current_state]);
                    final_states_sets.push_back(current_state);
                }
                break;
            }
        }

         
        for (char symbol : this->alphabet) {
            set<int> next_state;

             
            for (int state : current_state) {
                pair<int, char> key = {state, symbol};
                if (this->transition_map.find(key) != this->transition_map.end()) {
                    next_state.insert(this->transition_map[key].begin(), this->transition_map[key].end());
                }
            }

             
            set<int> next_state_epsilon = epsilon_closure(next_state);

             
            if (state_map.find(next_state_epsilon) == state_map.end()) {
                state_map[next_state_epsilon] = state_map.size();
                q.push(next_state_epsilon);
            }

             
            transition_map_dfa[{state_map[current_state], symbol}] = state_map[next_state_epsilon];
        }
    }

     
    map<int, string> new_final_states_ids;
    for (auto& final_set : final_states_sets) {
        vector<int> prioritized_states;
        for (int state : final_set) {
            if (find(final_states.begin(), final_states.end(), state) != final_states.end()) {
                prioritized_states.push_back(state);
            }
        }

        if (!prioritized_states.empty()) {
             
            auto max_priority_state = min_element(
                prioritized_states.begin(),
                prioritized_states.end(),
                [this](int a, int b) { return this->priority[a] < this->priority[b]; });

             
            int dfa_state = state_map[final_set];
            new_final_states_ids[dfa_state] = this->final_states_ids[*max_priority_state];
        }
    }

     
    this->transition_map_dfa = transition_map_dfa;
    this->final_states_dfa = final_states_dfa;
    this->state_map = state_map;
    this->start_state = state_map[start_set_epsilon];
    this->final_states_ids = new_final_states_ids;
}

set<int> DFA::epsilon_closure(set<int> state){
     
    set<int> closure = state;
    stack<int> s;
    for(int i : state){
        s.push(i);
    }

    while(!s.empty()){
        int current_state = s.top();
        s.pop();
        pair<int,char> key = {current_state,'~'};
        if(this->transition_map.find(key) != this->transition_map.end()){
            for(int next : this->transition_map[key]){
                if(closure.find(next) == closure.end()){
                    closure.insert(next);
                    s.push(next);
                }
            }
        }
    }
    return closure;
}

void DFA::minimize() {
    vector<set<int>> partitions;
    map<string, set<int>> final_state_partitions;
    for (const auto& [state, id] : this->final_states_ids) {
        final_state_partitions[id].insert(state);
    }
    for (const auto& [_, partition] : final_state_partitions) {
        partitions.push_back(partition);
    }

    set<int> non_final_states;
    for (const auto& [state_set, state_id] : this->state_map) {
        if (this->final_states_ids.find(state_id) == this->final_states_ids.end()) {
            non_final_states.insert(state_id);
        }
    }
    if (!non_final_states.empty()) {
        partitions.push_back(non_final_states);
    }

     

    bool changed;
    do {
        changed = false;
        vector<set<int>> new_partitions;

        for (const auto& group : partitions) {
            map<map<char, int>, set<int>> group_map;

            for (int state : group) {
                map<char, int> transition_group;
                for (char symbol : this->alphabet) {
                    if (this->transition_map_dfa.count({state, symbol})) {
                        int next_state = this->transition_map_dfa[{state, symbol}];
                        for (size_t i = 0; i < partitions.size(); ++i) {
                            if (partitions[i].count(next_state)) {
                                transition_group[symbol] = i;
                                break;
                            }
                        }
                    } else {
                        transition_group[symbol] = -1;  

                    }
                }
                group_map[transition_group].insert(state);
            }

            for (const auto& [_, sub_group] : group_map) {
                new_partitions.push_back(sub_group);
                if (sub_group.size() < group.size()) {
                    changed = true;
                }
            }
        }

        partitions = new_partitions;
    } while (changed);

    map<int, int> state_map_new;
    map<pair<int, char>, int> transition_map_dfa_new;
    vector<int> final_states_dfa_new;
    map<int, string> final_states_ids_new;

    for (size_t i = 0; i < partitions.size(); ++i) {
        for (int state : partitions[i]) {
            state_map_new[state] = i;
        }
    }

    for (const auto& [key, value] : this->transition_map_dfa) {
        int src = state_map_new[key.first];
        int dest = state_map_new[value];
        transition_map_dfa_new[{src, key.second}] = dest;
    }

    for (const auto& group : partitions) {
        int representative = *group.begin();
        if (this->final_states_ids.find(representative) != this->final_states_ids.end()) {
            final_states_dfa_new.push_back(state_map_new[representative]);
            final_states_ids_new[state_map_new[representative]] = this->final_states_ids[representative];
        }
    }

    this->transition_map_dfa = transition_map_dfa_new;
    this->final_states_dfa = final_states_dfa_new;
    this->final_states_ids = final_states_ids_new;
    this->state_map.clear();
    for (size_t i = 0; i < partitions.size(); ++i) {
        this->state_map[partitions[i]] = i;
    }
    this->start_state = state_map_new[this->start_state];
}



void DFA::print(){
    int number_of_states = state_map.size();

    cout << "Number of States: " << number_of_states << endl;
    map<pair<int,int>,string> state_transitions;
    for(auto i : transition_map_dfa){
        state_transitions[{i.first.first,i.second}] += i.first.second;
    }

    for(auto i : state_transitions){
        cout << "State: " << i.first.first << " ";
        cout << "Transition: " << i.second << " ";
        cout << "To State: " << i.first.second << endl;
    }

    cout << "Start State: " << this->start_state << endl;
    cout << endl;
    cout << "Final States: " << endl;
    for(int i : this->final_states_dfa){
        cout << i << " ";
    }
    cout << endl;
    cout << "Final States IDs: " << endl;
    for(auto i : this->final_states_ids){
        cout << i.first << " " << i.second << endl;
    }
}
void DFA::save(const string& filename){
    int number_of_states = state_map.size();
    map<pair<int,int>,string> state_transitions;
    for(auto i : transition_map_dfa){
        state_transitions[{i.first.first,i.second}] += i.first.second;
    }

    ofstream file(filename);

    // write in csv format
    file << "State,Transition,To State\n";
    for (auto i : state_transitions) {
        string transition = i.second;

        // Wrap transition in quotes if it contains a comma
        if (transition.find(',') != string::npos) {
            transition = "\"" + transition + "\"";
        }

        file << i.first.first << "," << transition << "," << i.first.second << "\n";
    }

    file << "Start State," << this->start_state << "\n";
    file << "Final States,";
    for(int i : this->final_states_dfa){
        file << i << " ";
    }
    file << "\n";
    file << "Final States IDs\n";
    for(auto i : this->final_states_ids){
        file << i.first << "," << i.second << "\n";
    }
    file.close();
}

bool DFA::is_final_state(const int& state){

    return std::find(final_states_dfa.begin(),final_states_dfa.end(),state) != final_states_dfa.end();
}

int DFA::get_start_state(){
    return this->start_state;
}

int DFA::get_next_state(const int& state, const char& symbol){
    // search for the transition in the transition map
    pair<int,char> key = {state,symbol};
    if(this->transition_map_dfa.find(key) == this->transition_map_dfa.end()){
        return -1;
    }
    return this->transition_map_dfa[{state,symbol}];
}

string DFA::get_final_state_id(const int& state){
    return this->final_states_ids[state];
}


void DFA::export_to_dot(const string& filename) {
    map<pair<int, int>, string> state_transitions;
    for (auto i : transition_map_dfa) {
        state_transitions[{i.first.first, i.second}] += i.first.second;
    }

    ofstream file(filename);
    file << "digraph DFA {\n";
    file << "    rankdir=LR;\n";
    
    // Define final states with their IDs as labels
    file << "    node [shape = doublecircle];\n";
    for (int state : final_states_dfa) {
        file << "    " << state << " [label=\"" << state << " " << final_states_ids[state] <<"\"];\n";
    }

    // Define the start state
    file << "    node [shape = box];\n";
    file << "    secret_node [style=invis, width=0, height=0];\n";
    file << "    secret_node -> " << this->start_state << ";\n";

    // Define other states and transitions
    file << "    node [shape = circle];\n";
    for (auto i : state_transitions) {
        file << "    " << i.first.first << " -> " << i.first.second 
             << " [ label = \"" << i.second << "\" ];\n";
    }

    file << "}\n";
    file.close();
}

