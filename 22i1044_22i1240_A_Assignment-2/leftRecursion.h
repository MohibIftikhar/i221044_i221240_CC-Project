#ifndef LEFT_RECURSION_H
#define LEFT_RECURSION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#define MAX_RULE_LEN 50

typedef struct {
    char* name;
    char** rules;
    int ruleCount;
    int ruleCapacity;
} NonTerminal;

typedef struct {
    NonTerminal* nonTerminals;
    int ntCount;
    int ntCapacity;
} Grammar;

void printRules(Grammar* g);

void initNonTerminal(NonTerminal* nt, const char* name) {
    nt->name = _strdup(name);
    nt->ruleCount = 0;
    nt->ruleCapacity = 2;
    nt->rules = malloc(nt->ruleCapacity * sizeof(char*));
    if (!nt->rules) { perror("Failed to allocate rules"); exit(1); }
}

void addRule(NonTerminal* nt, const char* rule) {
    if (nt->ruleCount >= nt->ruleCapacity) {
        nt->ruleCapacity *= 2;
        char** newRules = realloc(nt->rules, nt->ruleCapacity * sizeof(char*));
        if (!newRules) { perror("Failed to realloc rules"); exit(1); }
        nt->rules = newRules;
    }
    nt->rules[nt->ruleCount] = _strdup(rule);
    nt->ruleCount++;
}

void freeNonTerminal(NonTerminal* nt) {
    free(nt->name);
    for (int i = 0; i < nt->ruleCount; i++) free(nt->rules[i]);
    free(nt->rules);
}

void printRule(NonTerminal* nt) {
    printf("%s ->", nt->name);
    for (int i = 0; i < nt->ruleCount; i++) {
        printf(" %s", nt->rules[i]);
        if (i < nt->ruleCount - 1) printf(" |");
    }
    printf("\n");
}

void initGrammar(Grammar* g) {
    g->ntCount = 0;
    g->ntCapacity = 2;
    g->nonTerminals = malloc(g->ntCapacity * sizeof(NonTerminal));
    if (!g->nonTerminals) { perror("Failed to allocate nonTerminals"); exit(1); }
}

void trimSpaces(char* str) {
    int start = 0, end = strlen(str) - 1;
    while (str[start] == ' ') start++;
    while (end >= 0 && str[end] == ' ') end--;
    if (start > end) { str[0] = '\0'; return; }
    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
}

void addRuleToGrammar(Grammar* g, const char* rule) {
    char name[MAX_RULE_LEN] = {0}, parse[MAX_RULE_LEN] = {0};
    int parseIdx = 0, pastArrow = 0, nameSet = 0;

    for (int i = 0; rule[i] != '\0' && rule[i] != '\n'; i++) {
        if (rule[i] == ' ' && !pastArrow) continue;
        if (rule[i] == '-' || rule[i] == '>') {
            pastArrow = 1;
            if (!nameSet) {
                strncpy(name, parse, sizeof(name) - 1);
                name[sizeof(name) - 1] = '\0';
                nameSet = 1;
                if (g->ntCount >= g->ntCapacity) {
                    g->ntCapacity *= 2;
                    NonTerminal* newNTs = realloc(g->nonTerminals, g->ntCapacity * sizeof(NonTerminal));
                    if (!newNTs) { perror("Failed to realloc"); exit(1); }
                    g->nonTerminals = newNTs;
                }
                initNonTerminal(&g->nonTerminals[g->ntCount], name);
                g->ntCount++;
            }
            parseIdx = 0;
            memset(parse, 0, sizeof(parse));
            continue;
        }
        if (rule[i] == '|') {
            if (pastArrow && parseIdx > 0) {
                trimSpaces(parse);
                addRule(&g->nonTerminals[g->ntCount - 1], parse);
                parseIdx = 0;
                memset(parse, 0, sizeof(parse));
            }
            continue;
        }
        parse[parseIdx++] = rule[i];
    }
    if (pastArrow && parseIdx > 0) {
        trimSpaces(parse);
        addRule(&g->nonTerminals[g->ntCount - 1], parse);
    }
}

void inputData(Grammar* g, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }
    char line[MAX_RULE_LEN];
    while (fgets(line, sizeof(line), file)) {
        addRuleToGrammar(g, line);
    }
    fclose(file);
    printf("Initial Grammar:\n");
    printRules(g);
}

void solveNonImmediateLR(NonTerminal* A, NonTerminal* B) {
    char* nameA = A->name;
    char* nameB = B->name;
    char** newRules = malloc(A->ruleCount * B->ruleCount * sizeof(char*));
    int newRuleCount = 0;

    for (int i = 0; i < A->ruleCount; i++) {
        if (strncmp(A->rules[i], nameB, strlen(nameB)) == 0) {
            for (int j = 0; j < B->ruleCount; j++) {
                char temp[MAX_RULE_LEN];
                snprintf(temp, MAX_RULE_LEN, "%s%s", B->rules[j], A->rules[i] + strlen(nameB));
                newRules[newRuleCount++] = _strdup(temp);
            }
        } else {
            newRules[newRuleCount++] = _strdup(A->rules[i]);
        }
    }

    for (int i = 0; i < A->ruleCount; i++) free(A->rules[i]);
    free(A->rules);
    A->rules = newRules;
    A->ruleCount = newRuleCount;
    A->ruleCapacity = newRuleCount;
}

void solveImmediateLR(Grammar* g, NonTerminal* A) {
    char* name = A->name;
    char newName[MAX_RULE_LEN];
    snprintf(newName, MAX_RULE_LEN, "%s'", name);

    char** alphas = malloc(A->ruleCount * sizeof(char*));
    char** betas = malloc(A->ruleCount * sizeof(char*));
    int alphaCount = 0, betaCount = 0;

    for (int i = 0; i < A->ruleCount; i++) {
        if (strncmp(A->rules[i], name, strlen(name)) == 0) {
            alphas[alphaCount++] = _strdup(A->rules[i] + strlen(name));
        } else {
            betas[betaCount++] = _strdup(A->rules[i]);
        }
    }

    if (alphaCount == 0) {
        free(alphas);
        free(betas);
        return;
    }

    char** newRulesA = malloc((betaCount + 1) * sizeof(char*));
    int newRuleCountA = 0;
    if (betaCount == 0) {
        newRulesA[newRuleCountA++] = _strdup(newName);
    } else {
        for (int i = 0; i < betaCount; i++) {
            char temp[MAX_RULE_LEN];
            snprintf(temp, MAX_RULE_LEN, "%s%s", betas[i], newName);
            newRulesA[newRuleCountA++] = _strdup(temp);
        }
    }

    char** newRulesA1 = malloc((alphaCount + 1) * sizeof(char*));
    int newRuleCountA1 = 0;
    for (int i = 0; i < alphaCount; i++) {
        char temp[MAX_RULE_LEN];
        snprintf(temp, MAX_RULE_LEN, "%s%s", alphas[i], newName);
        newRulesA1[newRuleCountA1++] = _strdup(temp);
    }
    newRulesA1[newRuleCountA1++] = _strdup("\xce\xb5");

    for (int i = 0; i < A->ruleCount; i++) free(A->rules[i]);
    free(A->rules);
    A->rules = newRulesA;
    A->ruleCount = newRuleCountA;
    A->ruleCapacity = newRuleCountA;

    if (g->ntCount >= g->ntCapacity) {
        g->ntCapacity *= 2;
        NonTerminal* newNTs = realloc(g->nonTerminals, g->ntCapacity * sizeof(NonTerminal));
        if (!newNTs) { perror("Failed to realloc"); exit(1); }
        g->nonTerminals = newNTs;
    }
    initNonTerminal(&g->nonTerminals[g->ntCount], newName);
    g->nonTerminals[g->ntCount].rules = newRulesA1;
    g->nonTerminals[g->ntCount].ruleCount = newRuleCountA1;
    g->nonTerminals[g->ntCount].ruleCapacity = newRuleCountA1;
    g->ntCount++;

    for (int i = 0; i < alphaCount; i++) free(alphas[i]);
    for (int i = 0; i < betaCount; i++) free(betas[i]);
    free(alphas);
    free(betas);
}

void applyAlgorithm(Grammar* g) {
    int changed = 1;
    int iteration = 0;
    int* processed = calloc(g->ntCount, sizeof(int)); // Track processed NTs
    
    while (changed) {
        printf("Iteration %d:\n", iteration++);
        changed = 0;
        int size = g->ntCount;
        for (int i = 0; i < size; i++) {
            // Non-immediate LR
            for (int j = 0; j < i; j++) {
                int oldCount = g->nonTerminals[i].ruleCount;
                solveNonImmediateLR(&g->nonTerminals[i], &g->nonTerminals[j]);
                if (oldCount != g->nonTerminals[i].ruleCount) changed = 1;
            }
            // Immediate LR only if not processed
            if (!processed[i]) {
                int oldNtCount = g->ntCount;
                solveImmediateLR(g, &g->nonTerminals[i]);
                if (oldNtCount != g->ntCount) {
                    changed = 1;
                    processed = realloc(processed, g->ntCount * sizeof(int));
                    processed[i] = 1;
                }
            }
        }
    }
    free(processed);
}

void printRules(Grammar* g) {
    for (int i = 0; i < g->ntCount; i++) {
        printRule(&g->nonTerminals[i]);
    }
}

void printRuleToFile(FILE* file, NonTerminal* nt) {
    fprintf(file, "%s ->", nt->name);
    printf("%s ->", nt->name);

    for (int i = 0; i < nt->ruleCount; i++) {
        fprintf(file, " %s", nt->rules[i]);
        printf(" %s", nt->rules[i]);

        if (i < nt->ruleCount - 1) {
            fprintf(file, " |");
            printf(" |");
        }
    }
    
    fprintf(file, "\n");
    printf("\n");
}

void printRulesToFile(Grammar* g) {
    FILE* file = fopen("grammar_output.txt", "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < g->ntCount; i++) {
        printRuleToFile(file, &g->nonTerminals[i]);
    }
    fclose(file);
    printf("Grammar has been written to grammar_output.txt\n");

    FILE* out = fopen("output.txt", "a");
    if (!out) {
        perror("Error opening output.txt");
        exit(1);
    }
    fprintf(out, "\nAfter removing Left Recursion:\n");   
    for (int i = 0; i < g->ntCount; i++) {
        printRuleToFile(out, &g->nonTerminals[i]);
    }
    fclose(out);
}

void freeGrammar(Grammar* g) {
    for (int i = 0; i < g->ntCount; i++) {
        freeNonTerminal(&g->nonTerminals[i]);
    }
    free(g->nonTerminals);
}

#endif