import graph.Enclosure;
import graph.FANode;

import java.util.*;

public class Transformer {
    public enum RE_TYPE {
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

    private static List<String> REs = new LinkedList<>();

    //Graph for the FA
    public static FANode root;
    //Transition table
    //Map from <enclosure_identifier>
    public static List<Map<String, Enclosure>> transitionTable = new LinkedList<>();
    public static List<Enclosure> enclosures = new LinkedList<>();
    public static Set<Set<Enclosure>> optimizedEnclosures = new HashSet<>();

    //TODO: Re convert to NFA graph
    public static void RE2NFA(List<String> REs) {
        FANode S = new FANode("S", false);
        //n3 n4
        FANode n3 = new FANode("3", false);
        //Set to ID
        FANode n4 = new FANode("ID", true);
        n3.insertNext(RE_TYPE.epsilon.value, n4);
        n3.insertNext(RE_TYPE.digit.value, n3);
        n3.insertNext(RE_TYPE.letter.value, n3);
        //n2
        FANode n2 = new FANode("2", false);
        n2.insertNext(RE_TYPE.epsilon.value, n3);
        //n1
        FANode n1 = new FANode("1", false);
        n1.insertNext(RE_TYPE.letter.value, n2);
        S.insertNext(RE_TYPE.epsilon.value, n1);
        root = S;
    }

    public static void RE2NFA(FANode node, Set<String> idSMap) {
        root = node;
        REs.addAll(idSMap);
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
                if (!enclosures.contains(tmp)) {
                    //add new Enclosure I
                    tmp.identifier = enclosures.size();
                    enclosures.add(tmp);
                } else {
                    for (Enclosure en : enclosures) {
                        if (en.equals(tmp)) {
                            tmp = en;
                            break;
                        }
                    }
                }
                map.put(re, tmp);
            }
            ++index;
            transitionTable.add(map);
        } }

    //TODO: optimize the DFA by DFS
    public static void DFAOptimize() {
        optimizedEnclosures = new Optimizer().optimize(transitionTable, enclosures);
        List<Map<String, Enclosure>> ans = new LinkedList<>();
        for (Set<Enclosure> I : optimizedEnclosures) {
            Enclosure e = I.stream()
                    .findAny()
                    .get();

        }
    }

    private static boolean isEquivalent(Enclosure e1, Enclosure e2) {
        for (String re : REs) {
            Enclosure map1 = transitionTable.get(e1.identifier).get(re);
            Enclosure map2 = transitionTable.get(e2.identifier).get(re);
            for (Set<Enclosure> set : optimizedEnclosures) {
                if (set.contains(map1) && !set.contains(map2)) return false;
            }
        }
        return true;
    }
}
