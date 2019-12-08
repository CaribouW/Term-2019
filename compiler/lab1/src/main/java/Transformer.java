import graph.Enclosure;
import graph.FANode;

import java.util.*;

public class Transformer {
    private enum RE_TYPE {
        letter("letter"),
        digit("digit"),
        separator("separator"),
        reservedWord("reservedWord"),
        operator("operator"),
        epsilon("epsilon");
        private String value;

        RE_TYPE(String value) {
            this.value = value;
        }

        public String getValue() {
            return value;
        }
    }

    private static final String[] REs = {RE_TYPE.letter.value, RE_TYPE.digit.value,
            RE_TYPE.separator.value, RE_TYPE.reservedWord.value,
            RE_TYPE.operator.value};

    //Graph for the FA
    public static FANode root;
    //Transition table
    //Map from <enclosure_identifier>
    public static List<Map<String, Enclosure>> trasitionTable = new LinkedList<>();
    public static List<Enclosure> enclosures = new LinkedList<>();
    public static Set<Set<Enclosure>> optimizedEnclosures = new HashSet<>();

    //TODO: Re convert to NFA graph
    public static void RE2NFA(List<String> REs) {
        FANode S = new FANode("S", false);
        //n3 n4
        FANode n3 = new FANode("3", false);
        FANode n4 = new FANode("4", true);
        n3.insertNext(RE_TYPE.epsilon.value, n4);
        n3.insertNext(RE_TYPE.digit.value, n3);
        n3.insertNext(RE_TYPE.digit.value, n3);
        //n2
        FANode n2 = new FANode("2", false);
        n2.insertNext(RE_TYPE.epsilon.value, n3);
        //n1
        FANode n1 = new FANode("1", false);
        n1.insertNext(RE_TYPE.letter.value, n2);
        S.insertNext(RE_TYPE.epsilon.value, n1);
        root = S;
    }

    public static void NFA2DFA() {
        Enclosure I0 = new Enclosure(Collections.singleton(root));
        enclosures.add(I0);
        int index = 0;
        while (index < enclosures.size()) {
            Enclosure enclosure = enclosures.get(index);
            Map<String, Enclosure> map = new HashMap<>();
            for (String re : REs) {
                Set<FANode> set = enclosure.nextSet(re);
                //get the enclosure
                Enclosure tmp = new Enclosure(set);
                if (tmp.isEmpty()) continue;
                if (!tmp.equals(enclosure)) {
                    //add new Enclosure I
                    tmp.identifier = enclosures.size();
                    enclosures.add(tmp);
                }
                map.put(re, tmp);
            }
            ++index;
            trasitionTable.add(map);
        }
        printTable();
    }

    //TODO: optimize the DFA by DFS
    public static void DFAOptimize() {
        optimizedEnclosures.add((Set<Enclosure>) enclosures);

    }

    private static void printTable() {
        for (int i = 0; i < enclosures.size(); ++i) {

        }
    }
}
