#ifndef LL1_TABLE_H
#define LL1_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "first_follow.h"  // Uses GrammarFirstFollow, EPSILON, MAX_LENGTH, etc.

// ---------- Helper: Compute FIRST of a production string ----------
// We assume the production is a string with no spaces. Each symbol is either:
// - A non-terminal: starts with an uppercase letter (and optionally an apostrophe)
// - A terminal: any other character (or string)
// This function computes FIRST(production) and returns the set as an array of strings.
void computeFirstOfProduction(GrammarFirstFollow *g, const char *production, char firstProd[][MAX_LENGTH], int *firstCount) {
    *firstCount = 0;
    int len = strlen(production);
    bool derivesEpsilon = true;
    int pos = 0;
    while (pos < len && derivesEpsilon) {
        derivesEpsilon = false;
        char symbolToken[MAX_LENGTH] = {0};
        int tokLen = 0;
        // If production starts with EPSILON (the entire production is epsilon)
        if (strncmp(production + pos, EPSILON, strlen(EPSILON)) == 0) {
            strcpy(symbolToken, EPSILON);
            pos += strlen(EPSILON);
        }
        // If the symbol is a non-terminal (starts with an uppercase letter)
        else if (production[pos] >= 'A' && production[pos] <= 'Z') {
            symbolToken[tokLen++] = production[pos];
            pos++;
            if (pos < len && production[pos] == '\'') {
                symbolToken[tokLen++] = '\'';
                pos++;
            }
            symbolToken[tokLen] = '\0';
        }
        // Otherwise, assume terminal (take one character)
        else {
            symbolToken[0] = production[pos];
            symbolToken[1] = '\0';
            pos++;
        }
        // If symbolToken is a terminal or EPSILON, add it and break.
        if (strcmp(symbolToken, EPSILON) == 0 || isTerminal(symbolToken)) {
            if (!inSet(firstProd, *firstCount, symbolToken))
                addToSet(firstProd, firstCount, symbolToken);
            if (strcmp(symbolToken, EPSILON) != 0)
                break;
            else
                derivesEpsilon = true; // continue to next symbol
        }
        // Otherwise, symbolToken is a non-terminal.
        else {
            for (int i = 0; i < g->setCount; i++) {
                if (strcmp(g->sets[i].symbol, symbolToken) == 0) {
                    for (int j = 0; j < g->sets[i].firstCount; j++) {
                        if (strcmp(g->sets[i].first[j], EPSILON) != 0 &&
                            !inSet(firstProd, *firstCount, g->sets[i].first[j])) {
                            addToSet(firstProd, firstCount, g->sets[i].first[j]);
                        }
                    }
                    if (inSet(g->sets[i].first, g->sets[i].firstCount, EPSILON)) {
                        derivesEpsilon = true;
                    }
                    break;
                }
            }
        }
    }
    if (pos >= len && derivesEpsilon) {
        if (!inSet(firstProd, *firstCount, EPSILON))
            addToSet(firstProd, firstCount, EPSILON);
    }
}

// ---------- Build Terminal Set ----------
// This function collects all terminal symbols that appear in the FIRST and FOLLOW sets.
void buildTerminalSet(GrammarFirstFollow *g, char terminals[][MAX_LENGTH], int *termCount) {
    *termCount = 0;
    addToSet(terminals, termCount, "$");  // include end marker '$'
    for (int i = 0; i < g->setCount; i++) {
        FirstFollowSet *set = &g->sets[i];
        for (int j = 0; j < set->firstCount; j++) {
            if (strcmp(set->first[j], EPSILON) != 0 && isTerminal(set->first[j]))
                addToSet(terminals, termCount, set->first[j]);
        }
        for (int j = 0; j < set->followCount; j++) {
            if (isTerminal(set->follow[j]))
                addToSet(terminals, termCount, set->follow[j]);
        }
    }
}

typedef struct {
    int numRows;
    int numCols;
    char nonTerminals[MAX_SYMBOLS][MAX_LENGTH];
    char terminals[MAX_SYMBOLS][MAX_LENGTH];
    char table[MAX_SYMBOLS][MAX_SYMBOLS][MAX_LENGTH];
} LL1Table;

// Built using FIRST and FOLLOW Sets Calculated in previous step
void buildLL1Table(GrammarFirstFollow *g, LL1Table *ll1) {
    ll1->numRows = g->setCount;
    for (int i = 0; i < g->setCount; i++) {
        strcpy(ll1->nonTerminals[i], g->sets[i].symbol);
    }
    char termSet[MAX_SYMBOLS][MAX_LENGTH];
    int termCount = 0;
    buildTerminalSet(g, termSet, &termCount);
    ll1->numCols = termCount;
    for (int i = 0; i < termCount; i++) {
        strcpy(ll1->terminals[i], termSet[i]);
    }
    for (int i = 0; i < ll1->numRows; i++) {
        for (int j = 0; j < ll1->numCols; j++) {
            ll1->table[i][j][0] = '\0';
        }
    }
    for (int i = 0; i < g->ruleCount; i++) {
        GrammarRule *rule = &g->rules[i];
        int row = -1;
        for (int r = 0; r < ll1->numRows; r++) {
            if (strcmp(ll1->nonTerminals[r], rule->lhs) == 0) {
                row = r;
                break;
            }
        }
        if (row == -1) continue;
        for (int j = 0; j < rule->rhsCount; j++) {
            char *production = rule->rhs[j];
            char firstProd[MAX_SYMBOLS][MAX_LENGTH];
            int firstProdCount = 0;
            computeFirstOfProduction(g, production, firstProd, &firstProdCount);
            for (int k = 0; k < firstProdCount; k++) {
                if (strcmp(firstProd[k], EPSILON) != 0) {
                    for (int c = 0; c < ll1->numCols; c++) {
                        if (strcmp(ll1->terminals[c], firstProd[k]) == 0) {
                            strncpy(ll1->table[row][c], production, MAX_LENGTH-1);
                            ll1->table[row][c][MAX_LENGTH-1] = '\0';
                        }
                    }
                }
            }
            if (inSet(firstProd, firstProdCount, EPSILON)) {
                for (int r = 0; r < g->setCount; r++) {
                    if (strcmp(g->sets[r].symbol, rule->lhs) == 0) {
                        for (int f = 0; f < g->sets[r].followCount; f++) {
                            for (int c = 0; c < ll1->numCols; c++) {
                                if (strcmp(ll1->terminals[c], g->sets[r].follow[f]) == 0) {
                                    strncpy(ll1->table[row][c], production, MAX_LENGTH-1);
                                    ll1->table[row][c][MAX_LENGTH-1] = '\0';
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void printLL1Table(LL1Table *ll1, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening LL1 table output file");
        exit(1);
    }
    int colWidth = 15;
    // Print header row.
    fprintf(file, "%*s", colWidth, "");
    for (int j = 0; j < ll1->numCols; j++) {
        fprintf(file, "%*s", colWidth, ll1->terminals[j]);
    }
    fprintf(file, "\n");
    int totalWidth = colWidth * (ll1->numCols + 1);
    for (int i = 0; i < totalWidth; i++)
        fprintf(file, "-");
    fprintf(file, "\n");
    for (int i = 0; i < ll1->numRows; i++) {
        fprintf(file, "%*s", colWidth, ll1->nonTerminals[i]);
        for (int j = 0; j < ll1->numCols; j++) {
            fprintf(file, "%*s", colWidth, ll1->table[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);

    FILE *out = fopen("output.txt", "a");
    if (!out) {
        perror("Error opening output.txt");
        exit(1);
    }
    fprintf(out, "\nLL(1) Parsing Table:\n");
    fprintf(out, "%*s", colWidth, "");
    for (int j = 0; j < ll1->numCols; j++) {
        fprintf(out, "%*s", colWidth, ll1->terminals[j]);
    }
    fprintf(out, "\n");

    for (int i = 0; i < totalWidth; i++)
        fprintf(out, "-");
    fprintf(out, "\n");
    
    for (int i = 0; i < ll1->numRows; i++) {
        fprintf(out, "%*s", colWidth, ll1->nonTerminals[i]);
        for (int j = 0; j < ll1->numCols; j++) {
            fprintf(out, "%*s", colWidth, ll1->table[i][j]);
        }
        fprintf(out, "\n");
    }
    fclose(out);

}

#endif
