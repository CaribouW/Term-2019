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
                        tmpSet.addAll(node.getNext(edge));
                    }
                }
            }
            enclosure.addAll(tmpSet);
            finish = true;
            for (FANode node : enclosure) {
                if (node.edgeFANodeMap.containsKey(epsilon) &&
                        !isContain(enclosure, new HashSet<>(
                                node.getNext(epsilon)
                        ))) {
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
                ans.addAll(node.getNext(edge));
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

    /**
     * To get whether the enclosure is the end
     *
     * @return : return the end node if it exists , otherwise return NULL.
     */
    public FANode fetchEnd() {
        for (FANode node : enclosure) {
            if (node.isEnd) return node;
        }
        return null;
    }

    public static boolean isContain(Set<FANode> enclosure, Set<FANode> set) {
        for (FANode node : set) {
            if (!enclosure.contains(node)) {
                return false;
            }
        }
        return true;
    }
}
