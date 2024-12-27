#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
using namespace std;
class InputHandler
{
    private:
        vector<string> lines;
        vector<string> keywords;
        vector<string> punctuations;


    public:
        vector<string> get_lines();
        void read_file();
        void prepareKeywords();
        void preparePunctuations();
        string trim(const string& str);
        
         
};


#endif // INPUTHANDLER_H