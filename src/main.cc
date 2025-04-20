#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

extern FILE *yyin;
extern int yyparse(std::unique_ptr<std::string> &ast);

int main (int argc, const char *argv[]) {
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    yyin = fopen(input, "r");
    assert(yyin);

    std::unique_ptr<std::string> ast;
    auto ret = yyparse(ast);
    assert(!ret);

    std::cout << *ast << std::endl;
    return 0;
}