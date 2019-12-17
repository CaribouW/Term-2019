import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class ParsingTable {
    private static class TableRow {
        Map<String, String> actions;
        Map<String, String> gotos;

        public TableRow(Enclosure enclosure) {
            actions = new HashMap<>();
            gotos = new HashMap<>();
            for (String edge : enclosure.outEdges.keySet()) {
                if (Rule.isV_N(edge)) {
                    gotos.put(edge, enclosure.outEdges.get(edge));
                } else {
                    //shift
                    actions.put(edge, "S" + enclosure.outEdges.get(edge));
                }
            }
            //对于可归约项
            for (LR1Item item : enclosure.items) {
                //可以规约
                if (item.isDotEnd()) {
                    for (String symbol : item.predictiveSymbols) {
                        int index = getRuleIndex(item.rule, Rule.rules);
                        assert index >= 0;
                        actions.put(symbol, Rule.zeroIndex == index ? symbols.accept.getValue() : "r" + index);
                    }
                }
            }
        }

        private static int getRuleIndex(Pair<String, String> rule, List<Pair<String, String>> rules) {
            int ans = 0;
            for (; ans < rules.size(); ++ans) {
                if (rule.equals(rules.get(ans)))
                    return ans;
            }
            return -1;
        }
    }

    private Map<String, TableRow> table = new HashMap<>();

    public ParsingTable(Set<Enclosure> graph) {
        for (Enclosure enclosure : graph) {
            table.put(enclosure.identifier, new TableRow(enclosure));
        }
    }

    public void printout() {
        for (String state : table.keySet()) {
            System.out.println(String.format("state:%s", state));
            TableRow row = table.get(state);
            System.out.println("action:");
            for (String actionK : row.actions.keySet()) {
                System.out.println(String.format("%s:%s\t", actionK, row.actions.get(actionK)));
            }
            System.out.println("goto");
            for (String gotoK : row.gotos.keySet()) {
                System.out.println(String.format("%s:%s\t", gotoK, row.gotos.get(gotoK)));
            }
        }
    }

    public int size() {
        return this.table.size();
    }

    /**
     * Query the table element
     *
     * @param state    : 状态
     * @param symbol   : 符号
     * @param isAction : 是否是action查询
     * @return : 如果查找的到就返回，不存在就返回 ""
     */
    public String query(String state, String symbol, boolean isAction) {
        if (!table.containsKey(state)) return "";
        return isAction ? table.get(state).actions.getOrDefault(symbol, "")
                : table.get(state).gotos.getOrDefault(symbol, "");
    }
}
