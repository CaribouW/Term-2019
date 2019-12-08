package graph;

import java.util.HashMap;
import java.util.Map;

public class FANode {
    public boolean isEnd;
    public String identifier;
    public Map<String, FANode> edgeFANodeMap = new HashMap<>();

    public FANode(String identifier, boolean isEnd) {
        this.identifier = identifier;
        this.isEnd = isEnd;
    }

    /**
     * Fetch the next node according to the current identifier
     */
    public FANode getNext(String identifier) {
        return edgeFANodeMap.getOrDefault(identifier
                , null);
    }

    public void insertNext(String edge, FANode node) {
        this.edgeFANodeMap.put(edge, node);
    }
}
