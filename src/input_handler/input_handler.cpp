#include <fstream>
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include "input_handler.h"


using namespace std;
void InputHandler::read_file()
{
    ifstream file("input/rules.txt");  // Open the file
    if (!file.is_open()) {             // Check if the file opened successfully
        cerr << "Error: Could not open file." << std::endl;
        return;
    }

    string line;
    while (getline(file, line)) {  // Read file line by line
        lines.push_back(line);     // Add line to the vector
    }

    file.close();  // Close the file
}
vector<string> InputHandler::get_lines()
{
    return lines;
}
string InputHandler::trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

void InputHandler::prepareKeywords(){
    for(string line : lines){
        if(line[0]=='{' && line[line.size()-1]=='}') {
            line= line.substr(1, line.size()-2);
            line = trim(line);
            // split line by space
            istringstream ss(line);
            string word;
            while(ss >> word){
                keywords.push_back(word);
            }
        }
    }

    for(string keyword : keywords){
        cout << keyword << endl;
    }
}
void InputHandler::preparePunctuations(){
    for(string line : lines){
        if(line[0]=='[' && line[line.size()-1]==']') {
            line= line.substr(1, line.size()-2);
            line = trim(line);
            // split line by space
            istringstream ss(line);
            string word;
            while(ss >> word){
                //remove \ from the word if it is there
                if(word[0]=='\\'){
                    word = word.substr(1, word.size()-1);
                }
                punctuations.push_back(word);
            }
        }
    }

    for(string punctuation : punctuations){
        cout << punctuation << endl;
    }
}



