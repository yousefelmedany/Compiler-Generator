%option C++ noyywrap

%{
#include <iostream>
using namespace std;
%}

letter    [a-zA-Z]
digit     [0-9]
digits    {digit}+
id        {letter}({letter}|{digit})*
num       {digits}(\.{digits})?(E[+-]?{digits})?
rel_op    (==|!=|<=|>=|<|>)
assign_op =
add_op    [+|-]
mul_op    [*|/]

%%
boolean|int|float|if|else|while|do|break|continue|return|true|false {
    cout << yytext << endl;
}

{id} {
    cout << "id" << endl;
}

{num} {
    cout << "num" << endl;
}

{rel_op} {
    cout << "rel_op" << endl;
}

{assign_op} {
    cout << "assign_op" << endl;
}

{add_op} {
    cout << "add_op" << endl;
}

{mul_op} {
    cout << "mul_op" << endl;
}

[;,\(\)\\}\{] {
    cout << yytext << endl;
}

[ \t\n] {
    // ignore whitespace and newline
}

. {
    /* ignore invalid characters */
}

%%

int main() {
    FlexLexer* lexer = new yyFlexLexer;
    while (lexer->yylex() != 0);
    return 0;
}

