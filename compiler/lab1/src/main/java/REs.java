import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

public class REs {
    private static final String orSep = "|";
    private static final String loopSep = "*";
    //reserved syntax map
    private static final String syntaxErr = "Syntax error!";
    public static final String[] reservedSet = {
            "function", "abstract", "arguments", "boolean",
            "break", "const", "byte", "boolean", "catch",
            "if", "else", "default", "import", "in", "static",
            "this", "try", "throw", "final", "finally", "do",
            "delete", "let", "var", "typeof", "void", "volatile",
            "while", "with", "yield"
    };


    //operator appear in the js
    public static final String[] operatorSet = {
            "+", "-", "/", "%", ">", "<",
            "=", "&", "|", "~", ">=", "<=", "==",
            "!=", "===", "!==", "&&", "||", "++", "--", "+=",
            "-=", "(", ")", "[", "]", ".", "\""};

    public static final char[] separatorSet = {',', ';', '{', '}'};
    //All of the valid REs defined by myself
    public static String digit = "";
    public static String character = "";
    //S generator
    public static String operator = "";
    public static String reservedWord = "";
    public static String separator = "";    //all of the sep
    public static String ID = "";           //<letter> (letter | digit) *
    public static String number = "";       //consist of digits

    public static List<String> genS;

    public static Map<String, String> idSMap = new HashMap<>();
    public static Set<String> edgeSet = new HashSet<>();
    //construct the REs from the given details
    //TODO: RE to NFA
    // a | b
    static {
        //basic digit and character
        for (int i = 0; i < 9; ++i) {
            digit += String.valueOf(i) + orSep;
        }
        digit += String.valueOf(9);

        for (int i = 0; i < 25; ++i) {
            character += (char) ('a' + i) + orSep;
            character += (char) ('a' + i - 32) + orSep;
        }
        character += "z|Z";
        //operator
        for (int i = 0; i < operatorSet.length; ++i) {
            operator += operatorSet[i];
            if (i < operatorSet.length - 1) operator += orSep;
        }
        //reserved
        for (int i = 0; i < reservedSet.length; ++i) {
            reservedWord += reservedSet[i];
            if (i < reservedSet.length - 1) reservedWord += orSep;
        }
        //separator
        for (int i = 0; i < separatorSet.length; ++i) {
            separator += separatorSet[i];
            if (i < separatorSet.length - 1) separator += orSep;
        }
        //ID
        ID = "(" + character + ")((" + character + ")(" + digit + "))" + loopSep;
        //Number
        number = "(" + character + ")" + "(" + digit + ")" + loopSep;
        //save as a RE list
        genS = Arrays.asList(operator, reservedWord, separator, ID, number);
    }

    public static void initRe(String filepath) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(new File(filepath)));
        String input;
        while ((input = br.readLine()) != null) {
            String[] strings = input.split("->");
            if (strings.length != 2) {
                System.out.println(String.format("RE %s definition Error", input));
                idSMap = null;
                break;
            }
            idSMap.put(strings[0], strings[1]);
        }
        if (null == idSMap) return;
        //infix to postfix
        idSMap.replaceAll((k, v) -> TreeGenerator.infix2PostFix(idSMap.get(k)));
    }

}
