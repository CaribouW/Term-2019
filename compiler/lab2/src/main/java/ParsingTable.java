import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class ParsingTable {
    private class TableRow {
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
                        //conflict!
                        String[] content = symbol.split("\\|");
                        if (content[0].equals(symbols.end.getValue()) &&
                                item.rule.first.equals(symbols.zeroGen.getValue())) {
                            actions.put(content[0], symbols.accept.getValue());
                        } else actions.put(content[0], "r" + content[1]);
                    }
                }
            }
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
}
