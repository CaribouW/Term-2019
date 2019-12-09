import graph.Enclosure;
import graph.FANode;

import java.util.Arrays;
import java.util.HashSet;

public class Analyser {
    //reserved syntax map
    private static final String syntaxErr = "Syntax error";

    private static boolean isReservedSyntax(String target) {
        return new HashSet<>(Arrays.asList(REs.reservedSet)).contains(target);
    }

    private static boolean isOperator(String target) {
        return new HashSet<>(Arrays.asList(REs.operatorSet)).contains(target);
    }

    private static boolean isDelimiter(char target) {
        return new HashSet<>(Arrays.asList(REs.separatorSet)).contains(target);
    }


    public static void analyseEntry(String input) {
        int index = 0;
        int len = input.length();
        int entry = index;
        FANode endNode = null;
        StringBuilder sb = new StringBuilder();
        while (index < len) {
            char ch = input.charAt(index);
            if (endNode != null && ch == ' ') {
                printOut(endNode.identifier, sb.toString());
                sb = new StringBuilder();
                ++index;
                entry = 0;
                continue;
            }
            sb.append(ch);
            //jump to the next state according to the transition table
            Enclosure state = Transformer.trasitionTable
                    .get(entry)
                    .getOrDefault(String.valueOf(ch), null);
            if (null == state) {
                //Error condition due to the empty element in the transition table
                System.out.println(syntaxErr + " on the content < " + sb.append(" >").toString());
                sb = new StringBuilder();
                //jump to next blank
                entry = 0;
                while (index < input.length() &&
                        input.charAt(index++) != ' ') {
                }
                continue;
            } else {
                entry = state.identifier;
                FANode node = state.fetchEnd();
                //come to end
                if (null != node) {
                    endNode = node;
                }
            }
            ++index;
        }
        if (null != endNode) {
            printOut(endNode.identifier, sb.toString());
            entry = 0;
        }
    }

    private static void printOut(String identifier, String content) {
        if (isReservedSyntax(content)) {
            System.out.println(
                    "<" + Transformer.RE_TYPE.reservedWord.getValue() + "," + content + ">"
            );
        } else if (isOperator(content)) {
            System.out.println(
                    "<" + Transformer.RE_TYPE.operator.getValue() + "," + content + ">"
            );
        } else if (!content.isEmpty())
            System.out.println(
                    "<" + identifier + "," + content + ">"
            );
    }
}
