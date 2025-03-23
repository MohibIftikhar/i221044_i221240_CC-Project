#ifndef LEFT_FACTORING_H
#define LEFT_FACTORING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>

#define MAX_PROD 10
#define MAX_RHS 10
#define MAX_LEN 60

typedef struct {
    char nt;
    char rhs[MAX_RHS][MAX_LEN];
    int rhsCount;
} Production;

typedef struct {
    Production prods[MAX_PROD];
    int prodCount;
} lfGrammar;

lfGrammar g = {0};

void readCFG(const char* filename);
void leftFactoring();
void printGrammar();
void factorProduction(Production* p, char newNT);
char getNewNonTerminal();
bool needsFactoring(Production* p, char* commonPrefix, int* prefixLen);
void readCFG(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        perror("Details");
        exit(1);
    }

    char line[MAX_LEN];
    g.prodCount = 0;

    while (fgets(line, MAX_LEN, file) && g.prodCount < MAX_PROD) {
        line[strcspn(line, "\n")] = 0;
        Production* p = &g.prods[g.prodCount];
        p->nt = line[0];
        char* rhs = strchr(line, '>') + 2;
        p->rhsCount = 0;

        char* token = strtok(rhs, "|");
        while (token && p->rhsCount < MAX_RHS) {
            while (*token == ' ') token++;
            char* end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';
            strncpy(p->rhs[p->rhsCount++], token, MAX_LEN - 1);
            p->rhs[p->rhsCount - 1][MAX_LEN - 1] = '\0';
            token = strtok(NULL, "|");
        }
        g.prodCount++;
    }
    fclose(file);
}

bool productionExists(char rhs[MAX_RHS][MAX_LEN], int rhsCount, char* existingNT) {
    for (int i = 0; i < g.prodCount; i++) {
        Production* p = &g.prods[i];
        if (p->rhsCount == rhsCount) {
            bool match = true;
            for (int j = 0; j < rhsCount; j++) {
                if (strcmp(p->rhs[j], rhs[j]) != 0) {  // Fixed: Compare rhs[j]
                    match = false;
                    break;
                }
            }
            if (match) {
                *existingNT = p->nt;
                return true;
            }
        }
    }
    return false;
}

void leftFactoring() {
    bool changed;
    do {
        changed = false;
        int origProdCount = g.prodCount; // Process original + new productions

        for (int i = 0; i < origProdCount; i++) {
            Production* p = &g.prods[i];
            if (p->rhsCount <= 1) continue;

            char commonPrefix[MAX_LEN] = {0};
            int prefixLen = 0;
            if (needsFactoring(p, commonPrefix, &prefixLen)) {
                printf("Factoring %c with prefix '%s'\n", p->nt, commonPrefix);
                char newNT = getNewNonTerminal();
                factorProduction(p, newNT);
                changed = true;
            }
        }
    } while (changed); // Repeat until no more changes
}
bool needsFactoring(Production* p, char* commonPrefix, int* prefixLen) {
    if (p->rhsCount <= 1) return false;

    *prefixLen = 0;
    commonPrefix[0] = '\0';

    // Find the maximum possible prefix length across all RHS entries
    int max_possible_len = 0;
    for (int i = 0; i < p->rhsCount; i++) {
        int len = strlen(p->rhs[i]);
        if (len > max_possible_len) max_possible_len = len;
    }

    // Check from longest possible down to 1
    for (int len = max_possible_len; len > 0; len--) {
        // For each RHS entry, check if others share its prefix of this length
        for (int i = 0; i < p->rhsCount; i++) {
            if (strlen(p->rhs[i]) < len) continue;

            char currentPrefix[MAX_LEN];
            strncpy(currentPrefix, p->rhs[i], len);
            currentPrefix[len] = '\0';

            int count = 0;
            for (int j = 0; j < p->rhsCount; j++) {
                if (strncmp(p->rhs[j], currentPrefix, len) == 0) {
                    count++;
                }
            }

            if (count >= 2) { // At least two entries share this prefix
                *prefixLen = len;
                strcpy(commonPrefix, currentPrefix);
                return true; // Return immediately for the longest prefix
            }
        }
    }

    return false;
}
void factorProduction(Production* p, char newNT) {
    char commonPrefix[MAX_LEN] = {0};
    int prefixLen = 0;
    needsFactoring(p, commonPrefix, &prefixLen); // Recheck to confirm

    char suffixes[MAX_RHS][MAX_LEN];
    int suffixCount = 0;
    int remaining[MAX_RHS], remCount = 0;

    for (int i = 0; i < p->rhsCount; i++) {
        if (strncmp(p->rhs[i], commonPrefix, prefixLen) == 0) {
            char* suffix = p->rhs[i] + prefixLen;
            if (*suffix == '\0') {
                strcpy(suffixes[suffixCount++], "\xce\xb5"); // Replace empty with ε
            } else {
                strncpy(suffixes[suffixCount++], suffix, MAX_LEN - 1);
                suffixes[suffixCount - 1][MAX_LEN - 1] = '\0';
            }
        } else {
            remaining[remCount++] = i;
        }
    }

    // Reuse existing production if possible
    char existingNT = '\0';
    if (productionExists(suffixes, suffixCount, &existingNT)) {
        newNT = existingNT;
    } else {
        // Add new production for suffixes
        Production* newP = &g.prods[g.prodCount++];
        newP->nt = newNT;
        newP->rhsCount = 0;
        for (int i = 0; i < suffixCount; i++) {
            strcpy(newP->rhs[newP->rhsCount++], suffixes[i]);
        }
    }

    // Update original production
    Production temp = { .nt = p->nt, .rhsCount = 0 };
    char newRHS[MAX_LEN];
    snprintf(newRHS, MAX_LEN, "%s%c", commonPrefix, newNT);
    strcpy(temp.rhs[temp.rhsCount++], newRHS);

    // Add remaining productions
    for (int i = 0; i < remCount; i++) {
        strcpy(temp.rhs[temp.rhsCount++], p->rhs[remaining[i]]);
    }

    *p = temp;
}
void printGrammar() {
    FILE* file = fopen("grammar.txt", "w");
    if (!file) {
        perror("Error opening grammar.txt");
        exit(1);
    }
    for (int i = 0; i < g.prodCount; i++) {
        fprintf(file, "%c -> ", g.prods[i].nt);
        for (int j = 0; j < g.prods[i].rhsCount; j++) {
            fprintf(file, "%s", g.prods[i].rhs[j]);
            if (j < g.prods[i].rhsCount - 1) fprintf(file, " | ");
        }
        fprintf(file, "\n");
    }
    fclose(file);

    FILE* out = fopen("output.txt", "w");
    if (!out) {
        perror("Error opening grammar.txt");
        exit(1);
    }
    fprintf(out, "Left Factored Grammar:\n");
    for (int i = 0; i < g.prodCount; i++) {
        fprintf(out, "%c -> ", g.prods[i].nt);
        for (int j = 0; j < g.prods[i].rhsCount; j++) {
            fprintf(out, "%s", g.prods[i].rhs[j]);
            if (j < g.prods[i].rhsCount - 1) fprintf(out, " | ");
        }
        fprintf(out, "\n");
    }
    fclose(out);
}

char getNewNonTerminal() {
    char nt = 'A';
    while (nt <= 'Z') {
        bool inUse = false;
        for (int i = 0; i < g.prodCount; i++) {
            if (g.prods[i].nt == nt) {
                inUse = true;
                break;
            }
        }
        if (!inUse) return nt;
        nt++;
    }
    printf("Error: Exceeded non-terminal limit\n");
    exit(1);
}


#endif