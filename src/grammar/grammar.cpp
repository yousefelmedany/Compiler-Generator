#include "grammar.h"

void Grammar::addProduction(const std::string& lhs, const std::vector<std::string>& rhs) {
    if (start_symbol.empty()) {
        start_symbol = lhs;
    }
    
    non_terminals.insert(lhs);
    productions[lhs].push_back(rhs);
    
    for (const auto& symbol : rhs) {
        if (symbol[0] == '\'' || symbol[0] == '\"') {
            terminals.insert(symbol);
        } else if (symbol != "\\L") {  // not epsilon
            non_terminals.insert(symbol);
        }
    }
}

bool Grammar::isTerminal(const std::string& symbol) const {
    return terminals.find(symbol) != terminals.end();
}

bool Grammar::isNonTerminal(const std::string& symbol) const {
    return non_terminals.find(symbol) != non_terminals.end();
}

std::vector<std::string> Grammar::getProductions(const std::string& lhs) const {
    std::vector<std::string> result;
    for (const auto& production : productions.at(lhs)) {
        std::string rhs;
        for (const auto& symbol : production) {
            rhs += symbol + " ";
        }
        result.push_back(rhs);
    }
    return result;
}

std::string Grammar::getStartSymbol() const {
    return start_symbol;
}

std::set<std::string> Grammar::getTerminals() const {
    return terminals;
}

std::set<std::string> Grammar::getNonTerminals() const {
    return non_terminals;
}
