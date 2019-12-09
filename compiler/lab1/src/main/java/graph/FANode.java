package graph;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class FANode {
    public boolean isEnd;
    public String identifier;
    public Map<String, List<FANode>> edgeFANodeMap = new HashMap<>();

    public FANode(String identifier, boolean isEnd) {
        this.identifier = identifier;
        this.isEnd = isEnd;
    }

    /**
     * Fetch the next node according to the current identifier
     */
    public List<FANode> getNext(String identifier) {
        return edgeFANodeMap.getOrDefault(identifier
                , null);
    }

    public void insertNext(String edge, FANode node) {
        List<FANode> nodeList = edgeFANodeMap.getOrDefault(edge, new LinkedList<>());
        nodeList.add(node);
        this.edgeFANodeMap.put(edge, nodeList);
    }
}
