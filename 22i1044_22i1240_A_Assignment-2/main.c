#include "leftFactoring.h"
#include "leftRecursion.h"
#include "first_follow.h"
#include "ll1_table.h"

#include <windows.h>


int main() {
    SetConsoleOutputCP(CP_UTF8);

    // Initial Grammar
    readCFG("input.txt");
    printGrammar("Original CFG");

    leftFactoring();
    printGrammar();

    Grammar g;
    printf("\n\n\nLeft Recursion Elimination\n");
    initGrammar(&g);
    inputData(&g, "grammar.txt");
    applyAlgorithm(&g);
    printf("After Left Recursion Elimination:\n");
    printRules(&g);
    printRulesToFile(&g);

    GrammarFirstFollow g1;
    readGrammar(&g1, "grammar_output.txt");
    computeFirst(&g1);
    computeFollow(&g1);
    printFirstFollow(&g1, "first_follow_output.txt");

    LL1Table table;
    buildLL1Table(&g1, &table);
    printLL1Table(&table, "ll1_table_output.txt");

    freeGrammar(&g);

    return 0;
}