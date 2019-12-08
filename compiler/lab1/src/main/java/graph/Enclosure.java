package graph;

import java.util.HashSet;
import java.util.Set;

public class Enclosure {
    private Set<FANode> enclosure = new HashSet<>();
    private static final String epsilon = "epsilon";
    public int identifier;


    /**
     * Construct the epsilon enclosure by a set of nodes in graph
     * Adopting the BFS
     *
     * @param nodes : Nodes in the graph
     */
    public Enclosure(Set<FANode> nodes) {
        enclosure.addAll(nodes);
        boolean finish = false;
        while (!finish) {
            Set<FANode> tmpSet = new HashSet<>();
            for (FANode node : enclosure) {
                for (String edge : node.edgeFANodeMap.keySet()) {
                    if ("epsilon".equals(edge)
                            && null != node.getNext(edge)) {
                        tmpSet.add(node.getNext(edge));
                    }
                }
            }
            enclosure.addAll(tmpSet);
            finish = true;
            for (FANode node : enclosure) {
                if (node.edgeFANodeMap.containsKey(epsilon) &&
                        !enclosure.contains(node.getNext(epsilon))) {
                    finish = false;
                    break;
                }
            }
        }
    }

    /**
     * Find next set according to the edge send from the enclosure
     *
     * @param edge : The target edge identifier
     */
    public Set<FANode> nextSet(String edge) {
        Set<FANode> ans = new HashSet<>();
        for (FANode node : enclosure) {
            if (node.edgeFANodeMap.containsKey(edge)) {
                ans.add(node.getNext(edge));
            }
        }
        return ans;
    }

    @Override
    public boolean equals(Object obj) {
        Enclosure e = (Enclosure) obj;
        if (e.enclosure.size() != this.enclosure.size()) return false;
        for (FANode node : e.enclosure) {
            if (!this.enclosure.contains(node)) return false;
        }
        return true;
    }

    public boolean isEmpty() {
        return this.enclosure.size() == 0;
    }
}
