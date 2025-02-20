public class Automatas {

    public static void main(String[] args) {
        printKeywordStateTable();
        System.out.println();
        printBooleanLiteralStateTable();
        System.out.println();
        printInputStateTable();
        System.out.println();
        printPrintStateTable();
        System.out.println();
        printIdentifierStateTable();
        System.out.println();
        printAssignStateTable();
        System.out.println();
        printDecimalStateTable();
        System.out.println();
        printIntegerStateTable();
        System.out.println();
        printCharLiteralStateTable();
        System.out.println();
        printStringLiteralStateTable();
        System.out.println();
        printOperatorStateTable();
        System.out.println();
        printSeparatorStateTable();
        System.out.println();
        printCommentSingleStateTable();
        System.out.println();
        printCommentMultiStateTable();
    }

    // 1. KEYWORD: "^(int|decimal|char|bool|global|local)"
    static void printKeywordStateTable() {
        System.out.println("KEYWORD: Regex: \"^(int|decimal|char|bool|global|local)\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- ε --> q1      [branch \"int\"]");
        System.out.println("      q1 -- 'i' --> q2");
        System.out.println("      q2 -- 'n' --> q3");
        System.out.println("      q3 -- 't' --> q4 (accept)");
        System.out.println("  q0 -- ε --> q5      [branch \"decimal\"]");
        System.out.println("      q5 -- 'd' --> q6");
        System.out.println("      q6 -- 'e' --> q7");
        System.out.println("      q7 -- 'c' --> q8");
        System.out.println("      q8 -- 'i' --> q9");
        System.out.println("      q9 -- 'm' --> q10");
        System.out.println("      q10 -- 'a' --> q11");
        System.out.println("      q11 -- 'l' --> q12 (accept)");
        System.out.println("  q0 -- ε --> q13     [branch \"char\"]");
        System.out.println("      q13 -- 'c' --> q14");
        System.out.println("      q14 -- 'h' --> q15");
        System.out.println("      q15 -- 'a' --> q16");
        System.out.println("      q16 -- 'r' --> q17 (accept)");
        System.out.println("  q0 -- ε --> q18     [branch \"bool\"]");
        System.out.println("      q18 -- 'b' --> q19");
        System.out.println("      q19 -- 'o' --> q20");
        System.out.println("      q20 -- 'o' --> q21");
        System.out.println("      q21 -- 'l' --> q22 (accept)");
        System.out.println("  q0 -- ε --> q23     [branch \"global\"]");
        System.out.println("      q23 -- 'g' --> q24");
        System.out.println("      q24 -- 'l' --> q25");
        System.out.println("      q25 -- 'o' --> q26");
        System.out.println("      q26 -- 'b' --> q27");
        System.out.println("      q27 -- 'a' --> q28");
        System.out.println("      q28 -- 'l' --> q29 (accept)");
        System.out.println("  q0 -- ε --> q30     [branch \"local\"]");
        System.out.println("      q30 -- 'l' --> q31");
        System.out.println("      q31 -- 'o' --> q32");
        System.out.println("      q32 -- 'c' --> q33");
        System.out.println("      q33 -- 'a' --> q34");
        System.out.println("      q34 -- 'l' --> q35 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0 (start):");
        System.out.println("    on 'i' -> (int branch) S1: 'n' -> S2, S2: 't' -> S3 (accept)");
        System.out.println("    on 'd' -> (decimal branch) S4: 'e' -> S5, S5: 'c' -> S6, S6: 'i' -> S7,");
        System.out.println("          S7: 'm' -> S8, S8: 'a' -> S9, S9: 'l' -> S10 (accept)");
        System.out.println("    on 'c' -> (char branch) S11: 'h' -> S12, S12: 'a' -> S13, S13: 'r' -> S14 (accept)");
        System.out.println("    on 'b' -> (bool branch) S15: 'o' -> S16, S16: 'o' -> S17, S17: 'l' -> S18 (accept)");
        System.out.println("    on 'g' -> (global branch) S19: 'l' -> S20, S20: 'o' -> S21, S21: 'b' -> S22,");
        System.out.println("          S22: 'a' -> S23, S23: 'l' -> S24 (accept)");
        System.out.println("    on 'l' -> (local branch) S25: 'o' -> S26, S26: 'c' -> S27, S27: 'a' -> S28,");
        System.out.println("          S28: 'l' -> S29 (accept)");
    }

    // 2. BOOLEAN_LITERAL: "^(true|false)"
    static void printBooleanLiteralStateTable() {
        System.out.println("BOOLEAN_LITERAL: Regex: \"^(true|false)\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- 't' --> q1    [branch \"true\"]");
        System.out.println("      q1 -- 'r' --> q2");
        System.out.println("      q2 -- 'u' --> q3");
        System.out.println("      q3 -- 'e' --> q4 (accept)");
        System.out.println("  q0 -- 'f' --> q5    [branch \"false\"]");
        System.out.println("      q5 -- 'a' --> q6");
        System.out.println("      q6 -- 'l' --> q7");
        System.out.println("      q7 -- 's' --> q8");
        System.out.println("      q8 -- 'e' --> q9 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on 't' -> S1, S1: 'r' -> S2, S2: 'u' -> S3, S3: 'e' -> S4 (accept)");
        System.out.println("  S0: on 'f' -> S5, S5: 'a' -> S6, S6: 'l' -> S7, S7: 's' -> S8, S8: 'e' -> S9 (accept)");
    }

    // 3. INPUT: "^(input)\\s+([a-z]+)"
    static void printInputStateTable() {
        System.out.println("INPUT: Regex: \"^(input)\\s+([a-z]+)\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- 'i' --> q1");
        System.out.println("  q1 -- 'n' --> q2");
        System.out.println("  q2 -- 'p' --> q3");
        System.out.println("  q3 -- 'u' --> q4");
        System.out.println("  q4 -- 't' --> q5");
        System.out.println("  q5 -- (whitespace)+ --> q6");
        System.out.println("  q6 -- [a-z] --> q7");
        System.out.println("  q7 -- loop on [a-z] --> q7 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: 'i' -> S1, S1: 'n' -> S2, S2: 'p' -> S3, S3: 'u' -> S4, S4: 't' -> S5,");
        System.out.println("  S5: whitespace -> S6,");
        System.out.println("  S6: on [a-z] -> S7 (accept), S7: loop on [a-z]");
    }

    // 4. PRINT: "^(print\\(\"(.?)\"\\)|print\\(([a-z][a-z])\\))"
    static void printPrintStateTable() {
        System.out.println("PRINT: Regex: \"^(print\\(\"(.?)\"\\)|print\\(([a-z][a-z])\\))\"");
        System.out.println("NFA:");
        System.out.println("  [Branch A: print with string literal]");
        System.out.println("    q0 -- 'p' --> q1, q1 -- 'r' --> q2, q2 -- 'i' --> q3, q3 -- 'n' --> q4,");
        System.out.println("    q4 -- 't' --> q5, q5 -- '(' --> q6,");
        System.out.println("    q6 -- '\"' --> q7,");
        System.out.println("    q7 -- (any char, 0 or 1 time) --> q8,");
        System.out.println("    q8 -- '\"' --> q9,");
        System.out.println("    q9 -- ')' --> q10 (accept)");
        System.out.println("  [Branch B: print with two-letter identifier]");
        System.out.println("    q0 -- 'p' --> r1, r1 -- 'r' --> r2, r2 -- 'i' --> r3, r3 -- 'n' --> r4,");
        System.out.println("    r4 -- 't' --> r5, r5 -- '(' --> r6,");
        System.out.println("    r6 -- [a-z] --> r7,");
        System.out.println("    r7 -- [a-z] --> r8,");
        System.out.println("    r8 -- ')' --> r9 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: 'p' -> S1, S1: 'r' -> S2, S2: 'i' -> S3, S3: 'n' -> S4, S4: 't' -> S5, S5: '(' -> S6,");
        System.out.println("  At S6:");
        System.out.println("    if next is '\"': then S6: '\"' -> S7, S7: (optional any char) -> S8, S8: '\"' -> S9, S9: ')' -> S10 (accept);");
        System.out.println("    if next is letter: then S6: [a-z] -> S11, S11: [a-z] -> S12, S12: ')' -> S13 (accept)");
    }

    // 5. IDENTIFIER: "^[a-z]+"
    static void printIdentifierStateTable() {
        System.out.println("IDENTIFIER: Regex: \"^[a-z]+\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- [a-z] --> q1,");
        System.out.println("  q1 -- loop on [a-z] --> q1 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on [a-z] -> S1 (accept), S1: loop on [a-z]");
    }

    // 6. ASSIGN: "^="
    static void printAssignStateTable() {
        System.out.println("ASSIGN: Regex: \"^=\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- '=' --> q1 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on '=' -> S1 (accept)");
    }

    // 7. DECIMAL: "^(-?[0-9]+\\.[0-9]{1,5})"
    static void printDecimalStateTable() {
        System.out.println("DECIMAL: Regex: \"^(-?[0-9]+\\.[0-9]{1,5})\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- optional '-' --> q1,");
        System.out.println("  q1 -- [0-9]+ --> q2,");
        System.out.println("  q2 -- '.' --> q3,");
        System.out.println("  q3 -- [0-9] --> q4,");
        System.out.println("      then loop on [0-9] up to 4 additional times --> q4 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: if '-' then S1, else (digit) branch from S0 -> S2,");
        System.out.println("  S1/S2: loop on [0-9] -> S2,");
        System.out.println("  S2: on '.' -> S3,");
        System.out.println("  S3: on [0-9] -> S4 (accept),");
        System.out.println("  S4: on additional [0-9] (up to 4 times) remain in S4 (accept)");
    }

    // 8. INTEGER: "^(-?[0-9]+)"
    static void printIntegerStateTable() {
        System.out.println("INTEGER: Regex: \"^(-?[0-9]+)\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- optional '-' --> q1,");
        System.out.println("  q1 -- [0-9]+ --> q2 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: if '-' then S1, S1: on digit -> S2 (accept, loop on digit);");
        System.out.println("  Or S0: on digit -> S2 (accept, loop on digit)");
    }

    // 9. CHAR_LITERAL: "^'([^'])'"
    static void printCharLiteralStateTable() {
        System.out.println("CHAR_LITERAL: Regex: \"^'([^'])'\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- '\\'' --> q1,");
        System.out.println("  q1 -- (any char except '\\'') --> q2,");
        System.out.println("  q2 -- '\\'' --> q3 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on '\\'' -> S1,");
        System.out.println("  S1: on (char ≠ '\\'') -> S2,");
        System.out.println("  S2: on '\\'' -> S3 (accept)");
    }

    // 10. STRING_LITERAL: "^\"([^\"]*)\""
    static void printStringLiteralStateTable() {
        System.out.println("STRING_LITERAL: Regex: \"^\\\"([^\\\"]*)\\\"\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- '\"' --> q1,");
        System.out.println("  q1 -- loop on (any char except '\"') --> q1,");
        System.out.println("  q1 -- '\"' --> q2 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on '\"' -> S1,");
        System.out.println("  S1: while char ≠ '\"' remain in S1,");
        System.out.println("  S1: on '\"' -> S2 (accept)");
    }

    // 11. OPERATOR: "^[+\\-*/%^]"
    static void printOperatorStateTable() {
        System.out.println("OPERATOR: Regex: \"^[+\\-*/%^]\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- on one of {+, -, *, /, %, ^} --> q1 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on any of {+, -, *, /, %, ^} -> S1 (accept)");
    }

    // 12. SEPARATOR: "^[;{}]"
    static void printSeparatorStateTable() {
        System.out.println("SEPARATOR: Regex: \"^[;{}]\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- on one of { ;, {, } } --> q1 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on { ;, {, } } -> S1 (accept)");
    }

    // 13. COMMENT_SINGLE: "^//.*"
    static void printCommentSingleStateTable() {
        System.out.println("COMMENT_SINGLE: Regex: \"^//.*\"");
        System.out.println("NFA:");
        System.out.println("  q0 -- '/' --> q1,");
        System.out.println("  q1 -- '/' --> q2,");
        System.out.println("  q2 -- (any char)* --> q3 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on '/' -> S1,");
        System.out.println("  S1: on '/' -> S2,");
        System.out.println("  S2: on any char -> remain in S2 (accept)");
    }

    // 14. COMMENT_MULTI: "^/\\.?\\*/"  [DOTALL mode]
    static void printCommentMultiStateTable() {
        System.out.println("COMMENT_MULTI: Regex: \"^/\\.?\\*/\"  [DOTALL]");
        System.out.println("NFA:");
        System.out.println("  q0 -- '/' --> q1,");
        System.out.println("  q1 -- optional '.' --> q2,");
        System.out.println("  q2 -- '*' --> q3,");
        System.out.println("  q3 -- '/' --> q4 (accept)");
        System.out.println("DFA:");
        System.out.println("  S0: on '/' -> S1,");
        System.out.println("  S1: on '.' (if present) -> S2, else ε -> S2,");
        System.out.println("  S2: on '*' -> S3,");
        System.out.println("  S3: on '/' -> S4 (accept)");
    }
}
