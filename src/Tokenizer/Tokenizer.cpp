#include "Tokenizer.h"

Tokenizer::Tokenizer(DFA *dfa, const string &inputfile) {
    this->dfa = dfa;
    this->input_program = this->ExtractProgram(inputfile);
    this->current_position = 0;  // Add current position tracker
}

string Tokenizer::ExtractProgram(const string &inputfile) {
    ifstream file(inputfile);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + inputfile);
    }

    ostringstream content;
    content << file.rdbuf();
    file.close();
    return content.str();
}

// New method to get the next token
string Tokenizer::getNextToken() {
    const string ENDTOKEN = "$";
    
    // Skip whitespace and newlines
    while (current_position < input_program.length() && 
           (input_program[current_position] == ' ' || input_program[current_position] == '\n')) {
        current_position++;
    }

    // Check if we've reached the end of input
    if (current_position >= input_program.length()) {
        return ENDTOKEN;
    }

    int current_state = this->dfa->get_start_state();
    int max_munch_state = -1;
    int max_munch_index = -1;
    size_t start = current_position;

    // Process characters until we find a complete token
    while (current_position < input_program.length() && 
           input_program[current_position] != ' ' && 
           input_program[current_position] != '\n') {
        
        int next_state = this->dfa->get_next_state(current_state, input_program[current_position]);

        if (next_state == -1) break;

        current_state = next_state;

        if (this->dfa->is_final_state(current_state)) {
            max_munch_state = current_state;
            max_munch_index = current_position;
        }

        current_position++;
    }

    if (max_munch_state != -1) {
        string token_text = input_program.substr(start, max_munch_index - start + 1);
        string token_type = this->dfa->get_final_state_id(max_munch_state);
        
        // Store in symbol map
        symbol_map[token_text] = token_type;
        
        // Move position to after the token
        current_position = max_munch_index + 1;
        
        return token_type;
    } else {
        // Handle error case
        cerr << "Error: Unable to identify a token starting at index " << start << endl;
        current_position = start + 1;
        return "ERROR";
    }
}

// Method to check if there are more tokens
bool Tokenizer::hasMoreTokens() {
    size_t temp_pos = current_position;
    
    // Skip whitespace and newlines
    while (temp_pos < input_program.length() && 
           (input_program[temp_pos] == ' ' || input_program[temp_pos] == '\n')) {
        temp_pos++;
    }
    
    return temp_pos < input_program.length();
}

// Method to reset the tokenizer
void Tokenizer::reset() {
    current_position = 0;
}

// Method to get the current symbol map
map<string, string> Tokenizer::getSymbolMap() {
    return symbol_map;
}

// Method to peek at the next token without consuming it
string Tokenizer::peekNextToken() {
    size_t saved_position = current_position;
    string next_token = getNextToken();
    current_position = saved_position;
    return next_token;
}