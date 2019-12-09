import graph.Enclosure;
import graph.FANode;

import java.util.Arrays;
import java.util.HashSet;

public class Analyser {
    //reserved syntax map
    private static final String syntaxErr = "Syntax error!";

    private static boolean isReservedSyntax(String target) {
        return new HashSet<>(Arrays.asList(REs.reservedSet)).contains(target);
    }

    private static boolean isOperator(String target) {
        return new HashSet<>(Arrays.asList(REs.operatorSet)).contains(target);
    }

    private static boolean isDelimiter(char target) {
        return new HashSet<>(Arrays.asList(REs.separatorSet)).contains(target);
    }

    //DFA states
    private enum STATES {
        I0, I1, I2, I3, I4
    }

    //point out the current state
    private static STATES curState = STATES.I0;

    private static String analyseCh(char ch) {
        if (Character.isDigit(ch))
            return Transformer.RE_TYPE.digit.getValue();
        else if (Character.isLetter(ch))
            return Transformer.RE_TYPE.letter.getValue();
        else if (isDelimiter(ch))
            return Transformer.RE_TYPE.separator.getValue();
        return "";
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
                System.out.println(
                        "<" + endNode.identifier + "," + sb.toString() + ">"
                );
                sb = new StringBuilder();
                ++index;
                continue;
            }
            sb.append(ch);
            Enclosure state = Transformer.trasitionTable
                    .get(entry)
                    .getOrDefault(analyseCh(ch), null);
            if (null == state) {
                //TODO:error
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
            System.out.println(
                    "<" + endNode.identifier + "," + sb.toString() + ">"
            );
        }
    }



}
