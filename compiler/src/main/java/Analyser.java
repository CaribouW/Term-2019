import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashSet;

public class Analyser {
    //reserved syntax map
    private static final String syntaxErr = "Syntax error!";
    private static final String[] reservedSet = {
            "function", "abstract", "arguments", "boolean",
            "break", "const", "byte", "boolean", "catch",
            "if", "else", "default", "import", "in", "static",
            "this", "try", "throw", "final", "finally", "do",
            "delete", "let", "var", "typeof", "void", "volatile",
            "while", "with", "yield"
    };

    //operator appear in the js
    public static final String[] operatorSet = {
            "+", "-", "*", "/", "%", ">", "<",
            "=", "&", "|", "~", ">=", "<=", "==",
            "!=", "===", "!==", "&&", "||", "++", "--", "+=",
            "-=", "(", ")", "[", "]", ".", "\""};

    public static final char[] delimiterSet = {',', ';', '{', '}'};

    private static boolean isReservedSyntax(String target) {
        return new HashSet<>(Arrays.asList(reservedSet)).contains(target);
    }

    private static boolean isOperator(String target) {
        return new HashSet<>(Arrays.asList(operatorSet)).contains(target);
    }

    private static boolean isDelimiter(char target) {
        return new HashSet<>(Arrays.asList(delimiterSet)).contains(target);
    }

    //DFA states
    private enum STATES {
        I0, I1, I2, I3, I4
    }

    //point out the current state
    private static STATES curState = STATES.I0;

    public static void run() throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(new File("README.md")));
        String input;
        while ((input = br.readLine()) != null) {
            analyseEntry(input);
        }
    }

    private static void analyseEntry(String input) {
        int index = 0;
        int len = input.length();
        StringBuilder target = new StringBuilder();
        while (index <= len) {
            char ch = 0;
            if (index < len)
                ch = input.charAt(index);
            switch (curState) {
                case I0:
                    if (Character.isLetter(ch)) {
                        curState = STATES.I1;
                        target.append(ch);
                    } else if (Character.isDigit(ch)) {
                        curState = STATES.I2;
                        target.append(ch);
                    } else if (isDelimiter(ch)) {
                        curState = STATES.I4;
                        target.append(ch);
                    } else if (isOperator(target.append(ch).toString())) {
                        curState = STATES.I3;
                    } else if (ch == ' ') {
                        target = new StringBuilder();
                    }
                    break;
                case I1:
                    if (Character.isDigit(ch) || Character.isLetter(ch)) {
                        target.append(ch);
                    } else {
                        curState = STATES.I0;
                        if (isReservedSyntax(target.toString())) {
                            String output = '(' +
                                    "reserve," +
                                    target +
                                    ')';
                            System.out.println(output);
                        } else {
                            String output = '(' +
                                    "ID," +
                                    target +
                                    ')';
                            System.out.println(output);
                        }
                        target = new StringBuilder();
                        index--;
                    }
                    break;
                case I2:
                    if (Character.isDigit(ch) || ch == '.') {
                        target.append(ch);
                    } else {
                        curState = STATES.I0;
                        //number
                        String output = '(' +
                                "Number," +
                                target +
                                ')';
                        System.out.println(output);
                        target = new StringBuilder();
                        --index;
                    }
                    break;
                case I3:
                    StringBuilder temp = target.append(ch);
                    if (isOperator(temp.toString())) {
                        target = temp;
                    } else {
                        curState = STATES.I0;
                        String output = '(' +
                                "Operator," +
                                target +
                                ')';
                        System.out.println(output);
                        target = new StringBuilder();
                        --index;
                    }
                    break;
                case I4:
                    curState = STATES.I0;
                    //sep
                    target = new StringBuilder();
                    --index;
                    break;
                default:
                    System.out.println(syntaxErr);
            }

            ++index;
        }
    }

    public static void main(String[] args) {
        try {
            run();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
