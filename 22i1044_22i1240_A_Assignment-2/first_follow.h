#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define EPSILON "\xce\xb5"

#define MAX_SYMBOLS 50
#define MAX_RULES 50
#define MAX_LENGTH 100

// Done to clean tokens
void trim(char *str) {
    int start = 0;
    while (str[start] && isspace((unsigned char)str[start]))
        start++;
    
    int end = strlen(str) - 1;
    while (end >= start && isspace((unsigned char)str[end]))
        end--;
    
    int i, j = 0;
    for (i = start; i <= end; i++) {
        str[j++] = str[i];
    }
    str[j] = '\0';
}


typedef struct {
    char symbol[MAX_LENGTH];                   
    char first[MAX_SYMBOLS][MAX_LENGTH];
    int firstCount;
    char follow[MAX_SYMBOLS][MAX_LENGTH];
    int followCount;
} FirstFollowSet;

// A grammar rule with a multi-character LHS and several RHS productions.
typedef struct {
    char lhs[MAX_LENGTH];
    char rhs[MAX_RULES][MAX_LENGTH];
    int rhsCount;
} GrammarRule;

// The overall grammar structure.
typedef struct {
    GrammarRule rules[MAX_RULES];
    int ruleCount;
    FirstFollowSet sets[MAX_SYMBOLS];
    int setCount;
} GrammarFirstFollow;

bool inSet(char set[][MAX_LENGTH], int count, const char* sym) {
    for (int i = 0; i < count; i++) {
        if (strcmp(set[i], sym) == 0)
            return true;
    }
    return false;
}

void addToSet(char set[][MAX_LENGTH], int* count, const char* sym) {
    if (!inSet(set, *count, sym)) {
        strncpy(set[*count], sym, MAX_LENGTH - 1);
        set[*count][MAX_LENGTH - 1] = '\0';
        (*count)++;
    }
}

// Read Grammar, Expected to be "LF/LR Solved"
void readGrammar(GrammarFirstFollow* g, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening grammar file");
        exit(1);
    }
    g->ruleCount = 0;
    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, file)) {
        if (g->ruleCount >= MAX_RULES) break;
        line[strcspn(line, "\n")] = 0;
        GrammarRule* rule = &g->rules[g->ruleCount];
        // Get LHS token (which may be multi-character)
        char* token = strtok(line, " ");
        if (!token) continue;
        strcpy(rule->lhs, token);

        token = strtok(NULL, " ");
        token = strtok(NULL, "\n");
        rule->rhsCount = 0;
        if (token != NULL) {
            char* prodToken = strtok(token, "|");
            while (prodToken && rule->rhsCount < MAX_RULES) {
                char production[MAX_LENGTH];
                strncpy(production, prodToken, MAX_LENGTH - 1);
                production[MAX_LENGTH - 1] = '\0';
                trim(production);
                // Only add non-empty tokens.
                if (strlen(production) > 0) {
                    strcpy(rule->rhs[rule->rhsCount++], production);
                }
                prodToken = strtok(NULL, "|");
            }
        }
        g->ruleCount++;
    }
    fclose(file);
    
    g->setCount = g->ruleCount;
    for (int i = 0; i < g->ruleCount; i++) {
        strcpy(g->sets[i].symbol, g->rules[i].lhs);
        g->sets[i].firstCount = 0;
        g->sets[i].followCount = 0;
    }
}

// Uppercase here determines if non-terminal
bool isTerminal(const char* symbol) {
    if (symbol == NULL || symbol[0] == '\0')
        return true;
    return !(symbol[0] >= 'A' && symbol[0] <= 'Z');
}

// Compute FIRST sets.
// This simplified version extracts the first token from each production.
// It assumes that a token is either:
//   - EPSILON
//   - a non-terminal: an uppercase letter optionally followed by an apostrophe (e.g., "S'"),
//   - or a terminal: a single character (or string) that is not uppercase.
void computeFirst(GrammarFirstFollow* g) {
    bool changed;
    do {
        changed = false;
        for (int i = 0; i < g->ruleCount; i++) {
            GrammarRule* rule = &g->rules[i];
            FirstFollowSet* currSet = &g->sets[i];
            for (int j = 0; j < rule->rhsCount; j++) {
                char* production = rule->rhs[j];
                char token[MAX_LENGTH] = {0};

                // Check for epsilon first
                if (strncmp(production, EPSILON, strlen(EPSILON)) == 0) {
                    strcpy(token, EPSILON);
                }
                // Check for non-terminal (starts with uppercase)
                else if (production[0] >= 'A' && production[0] <= 'Z') {
                    token[0] = production[0];
                    if (production[1] == '\'') {
                        token[1] = '\'';
                        token[2] = '\0';
                    } else {
                        token[1] = '\0';
                    }
                }
                // Assume terminal (take first character)
                else {
                    token[0] = production[0];
                    token[1] = '\0';
                }

                if (strcmp(token, EPSILON) == 0 || isTerminal(token)) {
                    if (!inSet(currSet->first, currSet->firstCount, token)) {
                        addToSet(currSet->first, &currSet->firstCount, token);
                        changed = true;
                    }
                } else {
                    for (int k = 0; k < g->setCount; k++) {
                        if (strcmp(g->sets[k].symbol, token) == 0) {
                            for (int l = 0; l < g->sets[k].firstCount; l++) {
                                if (strcmp(g->sets[k].first[l], EPSILON) != 0 &&
                                    !inSet(currSet->first, currSet->firstCount, g->sets[k].first[l])) {
                                    addToSet(currSet->first, &currSet->firstCount, g->sets[k].first[l]);
                                    changed = true;
                                }
                            }
                            if (inSet(g->sets[k].first, g->sets[k].firstCount, EPSILON) &&
                                !inSet(currSet->first, currSet->firstCount, EPSILON)) {
                                addToSet(currSet->first, &currSet->firstCount, EPSILON);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    } while (changed);
}

// Compute FOLLOW sets.
// This version scans each production for non-terminals and applies the rules:
//   - If a non-terminal appears at the end of a production,
//     add the FOLLOW of the LHS non-terminal.
//   - If a non-terminal is followed by a terminal, add that terminal to its FOLLOW.
void computeFollow(GrammarFirstFollow* g) {
    // For the start symbol (first rule), add '$'
    addToSet(g->sets[0].follow, &g->sets[0].followCount, "$");
    
    bool changed;
    do {
        changed = false;
        for (int i = 0; i < g->ruleCount; i++) {
            GrammarRule* rule = &g->rules[i];
            for (int j = 0; j < rule->rhsCount; j++) {
                char* production = rule->rhs[j];
                int len = strlen(production);
                for (int pos = 0; pos < len; pos++) {
                    if (production[pos] >= 'A' && production[pos] <= 'Z') {
                        char nonTerm[MAX_LENGTH] = {0};
                        int idx = 0;
                        nonTerm[idx++] = production[pos];
                        if (pos + 1 < len && production[pos + 1] == '\'') {
                            nonTerm[idx++] = '\'';
                            nonTerm[idx] = '\0';
                            pos++; // skip apostrophe
                        } else {
                            nonTerm[idx] = '\0';
                        }
                        if (pos == len - 1) {
                            for (int s = 0; s < g->setCount; s++) {
                                if (strcmp(g->sets[s].symbol, rule->lhs) == 0) {
                                    for (int k = 0; k < g->sets[s].followCount; k++) {
                                        for (int t = 0; t < g->setCount; t++) {
                                            if (strcmp(g->sets[t].symbol, nonTerm) == 0) {
                                                if (!inSet(g->sets[t].follow, g->sets[t].followCount, g->sets[s].follow[k])) {
                                                    addToSet(g->sets[t].follow, &g->sets[t].followCount, g->sets[s].follow[k]);
                                                    changed = true;
                                                }
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        } else {
                            char next[MAX_LENGTH] = {0};
                            next[0] = production[pos+1];
                            next[1] = '\0';
                            if (isTerminal(next)) {
                                for (int t = 0; t < g->setCount; t++) {
                                    if (strcmp(g->sets[t].symbol, nonTerm) == 0) {
                                        if (!inSet(g->sets[t].follow, g->sets[t].followCount, next)) {
                                            addToSet(g->sets[t].follow, &g->sets[t].followCount, next);
                                            changed = true;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (changed);
}

// Print FIRST and FOLLOW sets to a file.
void printFirstFollow(GrammarFirstFollow* g, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening output file");
        exit(1);
    }
    for (int i = 0; i < g->setCount; i++) {
        fprintf(file, "FIRST(%s) = {", g->sets[i].symbol);
        for (int j = 0; j < g->sets[i].firstCount; j++) {
            fprintf(file, "%s", g->sets[i].first[j]);
            if (j < g->sets[i].firstCount - 1)
                fprintf(file, ", ");
        }
        fprintf(file, "}\n");
        fprintf(file, "FOLLOW(%s) = {", g->sets[i].symbol);
        for (int j = 0; j < g->sets[i].followCount; j++) {
            fprintf(file, "%s", g->sets[i].follow[j]);
            if (j < g->sets[i].followCount - 1)
                fprintf(file, ", ");
        }
        fprintf(file, "}\n");
    }
    fclose(file);

    FILE* out = fopen("output.txt", "a");
    if (!out) {
        perror("Error opening output file");
        exit(1);
    }
    fprintf(out, "\nFIRST and FOLLOW sets:\n\n");
    for (int i = 0; i < g->setCount; i++) {
        fprintf(out, "FIRST(%s) = {", g->sets[i].symbol);
        for (int j = 0; j < g->sets[i].firstCount; j++) {
            fprintf(out, "%s", g->sets[i].first[j]);
            if (j < g->sets[i].firstCount - 1)
                fprintf(out, ", ");
        }
        fprintf(out, "}\n");
        fprintf(out, "FOLLOW(%s) = {", g->sets[i].symbol);
        for (int j = 0; j < g->sets[i].followCount; j++) {
            fprintf(out, "%s", g->sets[i].follow[j]);
            if (j < g->sets[i].followCount - 1)
                fprintf(out, ", ");
        }
        fprintf(out, "}\n");
    }
    fclose(out);
}

#endif
