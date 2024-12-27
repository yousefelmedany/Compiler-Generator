#include "parser.h"
#include <stack>
#include <fstream>

using namespace std;

void ParserGenerator::parse(Tokenizer& tokenizer, 
                          map<string, map<string, vector<string>>> parsing_table,
                          string start_symbol,
                          const string& output_stack,
                          const string& output_derivations) {

    // Initialize stack with $ and start symbol
    stack<string> parse_stack;
    parse_stack.push("$");
    parse_stack.push(start_symbol);

    //initialize the left most derivations vector with the start symbol
    vector<string> derivs;
    derivs.push_back(start_symbol);
    int first_NT = 0;

    // Open output file for writing the Stack Tracing
    ofstream out_stack(output_stack);
    
    // Open output file for writing the derivations
    ofstream out_derivations(output_derivations);


    if (!out_stack.is_open()) {
        cerr << "Error: Could not open output stack file." << endl;
        return;
    
    }else if (!out_derivations.is_open()) {
        cerr << "Error: Could not open output derivations file." << endl;
        return;
    }

    // Get first token
    string current_token = tokenizer.getNextToken();
    
    // Write initial derivation
    out_stack << "$" << start_symbol << endl;

    // Continue parsing until stack is empty or error occurs
    while (!parse_stack.empty() && current_token!="EPSILON") {
        string top = parse_stack.top();
        
        // Debug output
        cout << "Stack top: " << top << ", Current token: " << current_token << endl;

        // If top is $
        if (top == "$") {
            if (current_token == "$") {
                parse_stack.pop();
                cout << "Parsing completed successfully!" << endl;
            } else {
                cerr << "Error: Expected end of input, got " << current_token << endl;
                out_stack.close();
                return;
            }
        }

        // If top is terminal (surrounded by quotes)
        else if (top[0] == '\'') {
            // Remove quotes from top
            string terminal = top.substr(1, top.length()-2);
            
            // check if top of stack equals current token, or the tokenizer passed 'assign' as the current token
            if (terminal == current_token || (terminal == "=" && current_token == "assign")) {
                parse_stack.pop();
                current_token = tokenizer.getNextToken();
            } else {
                cerr << "Error: Terminal mismatch. Expected " << terminal 
                         << ", got " << current_token << endl;
                parse_stack.pop();
            }

            // Write the current derivation in correct order to the output stack file
            stack<string> temp_stack = parse_stack;
            vector<string> derivation;
            while (!temp_stack.empty()) {
                derivation.push_back(temp_stack.top());
                temp_stack.pop();
            }
            for (int i = derivation.size() - 1; i >= 0; i--) {
                out_stack << derivation[i] << " ";
            }
            out_stack << endl;
        }
        
        // If top is non-terminal
        else {
            // Check if there's an entry in parsing table
            if (parsing_table.count(top) && parsing_table[top].count(current_token)) {
                // Get production rule
                vector<string> production = parsing_table[top][current_token];
                
                // Pop the non-terminal
                parse_stack.pop();
                
                // Push production in reverse order
                // If the production is not epsilon or synch
                if (production[0] != "EPSILON" && production[0] != "SYNCH") { 
                    //push the production to the parse stack
                    for (int i = production.size() - 1; i >= 0; i--) {
                        parse_stack.push(production[i]);
                    }   

                    //erase the NT from the left derivation and add the new production
                    derivs.erase(derivs.begin() + first_NT);
                    for(int i = 0; i< production.size(); i++){
                        derivs.insert(derivs.begin() + first_NT + i, production[i]);
                    }

                    //get the first NT in the derivations
                    for(int j = first_NT; j < derivs.size(); j++){
                        if(derivs[j][0] == '\''){
                            continue;
                        }else{
                            first_NT = j;
                            break;
                        }
                    }

                } else { //if the production gave epsilon or synch, then just remove the NT from derivations
                    
                    // if the production was synch, then report a misproduction
                    if(production[0] == "SYNCH"){
                        cerr << "Error: Non-Terminal misproduction. Synch is found" << endl;
                    }
                    derivs.erase(derivs.begin() + first_NT);
                    
                    //get the first NT in the derivations
                    for(int j = first_NT; j < derivs.size(); j++){
                        if(derivs[j][0] == '\''){
                            continue;
                        }else{
                            first_NT = j;
                            break;
                        }
                    }
                }

                //print the current derivation to the output derivations file
                string terminal;
                for(auto elem:derivs){
                    elem[0] == '\'' ? terminal = elem.substr(1, elem.length()-2) : terminal = elem; 
                    out_derivations << terminal << " ";
                }
                out_derivations << endl;
                    

                // Write current derivation to output stack
                stack<string> temp_stack = parse_stack;
                vector<string> derivation;
                while (!temp_stack.empty()) {
                    derivation.push_back(temp_stack.top());
                    temp_stack.pop();
                }
                
                // Write derivation in correct order to the output stack
                for (int i = derivation.size() - 1; i >= 0; i--) {
                    out_stack << derivation[i] << " ";
                }
                out_stack << endl;
            }
            else {
                cerr << "Error: No production rule found for non-terminal " << top 
                         << " and token " << current_token << endl;
                current_token = tokenizer.getNextToken();
            }
        }
    }
    out_stack.close();
    out_derivations.close();
}