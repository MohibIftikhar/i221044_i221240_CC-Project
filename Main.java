import java.util.*;
import java.util.regex.*;

class Preprocessor {
    public static String removeComments(String sourceCode) {
        // Remove single-line comments
        String noSingleLine = sourceCode.replaceAll("//.*", "");
        // Remove multi-line comments using DOTALL flag to match across lines
        Pattern multiLinePattern = Pattern.compile("/\\*.*?\\*/", Pattern.DOTALL);
        Matcher matcher = multiLinePattern.matcher(noSingleLine);
        String preprocessed = matcher.replaceAll("");
        return preprocessed;
    }
}

enum TokenType {
    KEYWORD("^(int|decimal|char|bool|global|local)"),
    BOOLEAN_LITERAL("^(true|false)"),
    INPUT("^(input)\\s+([a-z]+)"),
    PRINT("^(print\\(\"(.?)\"\\)|print\\(([a-z][a-z])\\))"),
    IDENTIFIER("^[a-z]+"),
    ASSIGN("^="),
    DECIMAL("^(-?[0-9]+\\.[0-9]{1,5})"),
    INTEGER("^(-?[0-9]+)"),
    CHAR_LITERAL("^'([^'])'"),
    STRING_LITERAL("^\"([^\"]*)\""),
    OPERATOR("^[+\\-*/%^]"),
    SEPARATOR("^[;{}]"),
    COMMENT_SINGLE("^//.*"),
    COMMENT_MULTI("^/\\.?\\*/", Pattern.DOTALL);

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
                    String value = matcher.group(matcher.groupCount());
                    tokens.add(new Token(type, value));

                    remaining = remaining.substring(matcher.end()).stripLeading();
                    matched = true;
                    break;
                }
            }

            if (!matched) {
                System.out.println("Lexical error: Unrecognized token near -> " + remaining);
                break;
            }
        }
        for (Token token : tokens) {
            System.out.println(token.toString());
        }
        return tokens;
    }
}


// SymbolEntry to track variable details
class SymbolEntry {
    String type;
    String value;
    boolean isConstant;
    String scope;

    public SymbolEntry(String type, String value, boolean isConstant, String scope) {
        this.type = type;
        this.value = value;
        this.isConstant = isConstant;
        this.scope = scope;
    }

    @Override
    public String toString() {
        return String.format("{Type: %s, Value: %s, Constant: %s, Scope: %s}",
                type, value, isConstant, scope);
    }
}

// Stack-based SymbolTable for scopes
class SymbolTable {
    private Stack<Map<String, SymbolEntry>> scopes = new Stack<>();

    public SymbolTable() {
        enterScope(); // Global scope
    }

    public void enterScope() {
        scopes.push(new HashMap<>());
    }

    public void exitScope() {
        if (scopes.size() > 1) {
            scopes.pop();
        }
    }

    public void addSymbol(String name, SymbolEntry entry) {
        scopes.peek().put(name, entry);
    }

    public SymbolEntry lookup(String name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes.get(i).containsKey(name)) {
                return scopes.get(i).get(name);
            }
        }
        return null;
    }

    public void printTable() {
        System.out.println("Symbol Table:");
        for (int i = 0; i < scopes.size(); i++) {
            System.out.println("Scope " + i + ": " + scopes.get(i));
        }
    }
}

// Updated Parser with scope and constant handling
class Parser {
    private List<Token> tokens;
    private SymbolTable symbolTable = new SymbolTable();
    private int currentTokenIndex = 0;

    public Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    public void parse() {
        while (currentTokenIndex < tokens.size()) {
            Token token = tokens.get(currentTokenIndex);
            switch (token.type) {
                case KEYWORD:
                    handleDeclaration();
                    break;
                case SEPARATOR:
                    if (token.value.equals("{")) {
                        symbolTable.enterScope();
                    } else if (token.value.equals("}")) {
                        symbolTable.exitScope();
                    }
                    currentTokenIndex++;
                    break;
                // Handle other cases (INPUT, PRINT, etc.)
                default:
                    currentTokenIndex++;
            }
        }
        symbolTable.printTable();
    }

    private void handleDeclaration() {
        // Example: global int x = 5;
        boolean isGlobal = tokens.get(currentTokenIndex).value.equals("global");
        boolean isLocal = tokens.get(currentTokenIndex).value.equals("local");
        if (isGlobal || isLocal) {
            currentTokenIndex++; // Skip 'global'/'local'
        }
        String type = tokens.get(currentTokenIndex).value;
        currentTokenIndex++;
        String varName = tokens.get(currentTokenIndex).value;
        currentTokenIndex += 2; // Skip '=' and get value
        String value = tokens.get(currentTokenIndex).value;
        currentTokenIndex++;

        SymbolEntry entry = new SymbolEntry(type, value, true, isGlobal ? "global" : "local");
        symbolTable.addSymbol(varName, entry);
    }
}

// Example NFA and DFA for IDENTIFIER token
public class Main {
    public static void main(String[] args) {
        String src = """               
                // Comment 
        
                /* Comment Multi
                 Line */
                global int xz = ;
                char a = 'l';
                decimal b;
                bool c; // Comment
                int y;
                y = 2+3;
                print("You");
                """;
        Parser parser = new Parser(LexicalAnalyzer.tokenize(Preprocessor.removeComments(src)));
        parser.parse();
    }
}