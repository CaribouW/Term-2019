import graph.Enclosure;

import java.util.*;
import java.util.stream.Collectors;

public class Optimizer {
    private List<Map<String, Enclosure>> table;
    private Set<Set<Enclosure>> ans = new HashSet<>();

    /**
     * @param transitionTable : The DFA table without optimizing
     * @param enclosures      : enclosure full set
     * @return : The new enclosure sets
     */
    public Set<Set<Enclosure>> optimize(List<Map<String, Enclosure>> transitionTable, List<Enclosure> enclosures) {
        ans.add(enclosures
                .stream()
                .filter(enclosure -> enclosure.fetchEnd() != null)
                .collect(Collectors.toSet()));
        ans.add(enclosures
                .stream()
                .filter(enclosure -> enclosure.fetchEnd() == null)
                .collect(Collectors.toSet()));
        table = transitionTable;
        //for each leaf in the full set
        for (Set<Enclosure> leaf : ans) {
            Set<Enclosure> eq = new HashSet<>();
            Enclosure ueqEn;
            List<Enclosure> tmpList = new LinkedList<>(leaf);
            for (int i = 0; i < tmpList.size(); ++i) {
                Enclosure e1 = tmpList.get(i);
                for (int j = i + 1; j < tmpList.size(); ++j) {
                    Enclosure e2 = tmpList.get(j);
                    if (isEquivalent(e1, e2)) {
                        eq.addAll(Arrays.asList(e1, e2));
                    } else {
                        ueqEn = eq.contains(e1) ? e2 : e1;
                        break;
                    }
                }
            }
        }
        return ans;
    }


    /**
     * @return : return the enclosure which is not the equivalent class among all of the other enclosures in the target
     * Set. And return #null if all of the element are equivalent
     */
    private boolean isEquivalent(Enclosure e1, Enclosure e2) {
        Map<String, Enclosure> row1 = table.get(e1.identifier), row2 = table.get(e2.identifier);
        for (String key : row1.keySet()) {
            Enclosure v1 = row1.getOrDefault(key, null),
                    v2 = row2.getOrDefault(key, null);
            if (v1 != null) {
                Set<Enclosure> enclosureSet1 = ans.stream()
                        .filter(s -> s.contains(v1))
                        .findAny()
                        .get();
                if (enclosureSet1.contains(v2)) return false;
            } else {
                if (v2 != null) return false;
            }
        }
        return true;
    }
}
