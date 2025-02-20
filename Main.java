import java.util.*;
import java.util.regex.*;

class ErrorHandler {
    private static Stack<String> errors = new Stack<>();

    public static void logError(String errorMessage) {
        errors.push(errorMessage);
    }

    public static List<String> getErrors() {
        return new ArrayList<>(errors);
    }

    public static boolean hasErrors() {
        return !errors.isEmpty();
    }

    public static void clearErrors() {
        errors.clear();
    }
}

class Preprocessor {
    public static String removeComments(String sourceCode) {
        String noSingleLine = sourceCode.replaceAll("//.*", "");
        Pattern multiLinePattern = Pattern.compile("/\\*.*?\\*/", Pattern.DOTALL);
        Matcher matcher = multiLinePattern.matcher(noSingleLine);
        return matcher.replaceAll("");
    }
}

enum TokenType {
    KEYWORD("^(int|decimal|char|bool|global|local)"),
    BOOLEAN_LITERAL("^(true|false)"),
    INPUT("^(input)\\s+([a-z][a-z]*)"),
    PRINT("^(print\\(\"(.*?)\"\\)|print\\(([a-z][a-z]*)\\))"),
    IDENTIFIER("^[a-z][a-z]*"),
    ASSIGN("^="),
    DECIMAL("^(-?[0-9]+\\.[0-9]{1,5})"),
    INTEGER("^(-?[0-9]+)"),
    CHAR_LITERAL("^'([^'])'"),
    STRING_LITERAL("^\"([^\"]*)\""),
    OPERATOR("^[+\\-*/%^]"),
    SEPARATOR("^[;{}]"),
    COMMENT_SINGLE("^//.*"),
    COMMENT_MULTI("^/\\*.*?\\*/", Pattern.DOTALL);

    public final Pattern pattern;

    TokenType(String regex) {
        this.pattern = Pattern.compile(regex);
    }

    TokenType(String regex, int flags) {
        this.pattern = Pattern.compile(regex, flags);
    }
}

class Token {
    TokenType type;
    String value;

    Token(TokenType type, String value) {
        this.type = type;
        this.value = value;
    }

    @Override
    public String toString() {
        return "<" + type.name().toLowerCase() + "," + value + ">";
    }
}

class LexicalAnalyzer {
    public static List<Token> tokenize(String sourceCode) {
        List<Token> tokens = new ArrayList<>();
        String remaining = sourceCode.strip();

        while (!remaining.isEmpty()) {
            boolean matched = false;

            for (TokenType type : TokenType.values()) {
                Matcher matcher = type.pattern.matcher(remaining);

                if (matcher.find()) {
                    String value = matcher.group(0);
                    int groupIndex = 0;

                    switch (type) {
                        case INPUT:
                            value = matcher.group(2);
                            break;
                        case PRINT:
                            if (matcher.group(2) != null) {
                                value = matcher.group(2);
                            } else {
                                value = matcher.group(3);
                            }
                            break;
                        case CHAR_LITERAL:
                        case STRING_LITERAL:
                            groupIndex = matcher.groupCount() > 0 ? 1 : 0;
                            value = matcher.group(groupIndex);
                            break;
                        default:
                            if (matcher.groupCount() > 0) {
                                value = matcher.group(1);
                            }
                            break;
                    }

                    tokens.add(new Token(type, value != null ? value : matcher.group(0)));

                    remaining = remaining.substring(matcher.end()).stripLeading();
                    matched = true;
                    break;
                }
            }

            if (!matched) {
                String errorChar = remaining.substring(0, 1);
                ErrorHandler.logError("Lexical error: Unrecognized character '" + errorChar + "'");
                remaining = remaining.substring(1).stripLeading();
            }
        }
        return tokens;
    }
}

class SymbolTableEntry {
    private String name;
    private String type;
    private String scope;

    public SymbolTableEntry(String name, String type, String scope) {
        this.name = name;
        this.type = type;
        this.scope = scope;
    }

    public String getName() {
        return name;
    }

    public String getType() {
        return type;
    }

    public String getScope() {
        return scope;
    }

    @Override
    public String toString() {
        return "Name: " + name + ", Type: " + type + ", Scope: " + scope;
    }
}

class SymbolTable {
    private List<SymbolTableEntry> entries = new ArrayList<>();

    public void addEntry(String name, String type, String scope) {
        for (SymbolTableEntry entry : entries) {
            if (entry.getName().equals(name) && entry.getScope().equals(scope)) {
                ErrorHandler.logError("Redefinition of variable '" + name + "' in " + scope + " scope");
                return;
            }
        }
        entries.add(new SymbolTableEntry(name, type, scope));
    }

    public static SymbolTable buildSymbolTable(List<Token> tokens) {
        SymbolTable symbolTable = new SymbolTable();
        int i = 0;
        while (i < tokens.size()) {
            Token currentToken = tokens.get(i);
            if (currentToken.type == TokenType.KEYWORD) {
                String keyword = currentToken.value;
                if (keyword.equals("global") || keyword.equals("local")) {
                    if (i + 1 < tokens.size()) {
                        Token nextToken = tokens.get(i + 1);
                        if (nextToken.type == TokenType.KEYWORD && isTypeKeyword(nextToken.value)) {
                            if (i + 2 < tokens.size()) {
                                Token idToken = tokens.get(i + 2);
                                if (idToken.type == TokenType.IDENTIFIER) {
                                    String scopeVal = keyword;
                                    String typeVal = nextToken.value;
                                    String nameVal = idToken.value;
                                    symbolTable.addEntry(nameVal, typeVal, scopeVal);
                                    i += 3;
                                    continue;
                                } else {
                                    ErrorHandler.logError("Syntax error: Expected identifier after " + keyword + " " + nextToken.value);
                                }
                            }
                        }
                    }
                } else if (isTypeKeyword(keyword)) {
                    if (i + 1 < tokens.size()) {
                        Token idToken = tokens.get(i + 1);
                        if (idToken.type == TokenType.IDENTIFIER) {
                            symbolTable.addEntry(idToken.value, keyword, "local");
                            i += 2;
                            continue;
                        } else {
                            ErrorHandler.logError("Syntax error: Expected identifier after " + keyword);
                        }
                    }
                }
            }
            i++;
        }
        return symbolTable;
    }

    private static boolean isTypeKeyword(String keyword) {
        return keyword.equals("int") || keyword.equals("decimal") || keyword.equals("char") || keyword.equals("bool");
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (SymbolTableEntry entry : entries) {
            sb.append(entry.toString()).append("\n");
        }
        return sb.toString();
    }
}

public class Main {
    public static void main(String[] args) {
        ErrorHandler.clearErrors();
        String src = """
                // Comment
                
                /* Comment Multi
                 Line */
                
                global int xz = -3;
                char a = 'l';
                decimal b = -3.142;
                global int xz;  // Redefinition in global scope
                bool c; // Comment
                int y;
                y = 2+3;
                input xz;
                print(xz);
                """;
        String preprocessed = Preprocessor.removeComments(src);
        List<Token> tokens = LexicalAnalyzer.tokenize(preprocessed);
        System.out.println("Tokens:");
        for (Token token : tokens) {
            System.out.println(token);
        }
        SymbolTable symbolTable = SymbolTable.buildSymbolTable(tokens);
        System.out.println("\nSymbol Table:");
        System.out.println(symbolTable);

        List<String> errors = ErrorHandler.getErrors();
        if (!errors.isEmpty()) {
            System.out.println("\nErrors:");
            for (String error : errors) {
                System.out.println(error);
            }
        }
    }
}