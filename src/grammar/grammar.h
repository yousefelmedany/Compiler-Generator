#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <map>
#include <set>

class Grammar {
public:
    std::map<std::string, std::vector<std::vector<std::string>>> productions;
    std::set<std::string> terminals;
    std::set<std::string> non_terminals;
    std::string start_symbol;

    void addProduction(const std::string& lhs, const std::vector<std::string>& rhs);
    bool isTerminal(const std::string& symbol) const;
    bool isNonTerminal(const std::string& symbol) const;

    std::vector<std::string> getProductions(const std::string& lhs) const;
    std::string getStartSymbol() const;
    std::set<std::string> getTerminals() const;
    std::set<std::string> getNonTerminals() const;
};

#endif