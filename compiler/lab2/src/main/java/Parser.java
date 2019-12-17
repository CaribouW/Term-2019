import java.util.*;
import java.util.stream.Collectors;

/**
 * The parser to generate the final parser table
 */
public class Parser {
    /**
     * 根据文法定义，生成DFA闭包
     *
     * @param rules       : 文法
     * @param beginSymbol : 初始符号
     * @return : 返回最终的闭包
     */
    public static Set<Enclosure> rule2Enclosure(List<Pair<String, String>> rules, String beginSymbol) {
        //首先给出0号产生式
        rules.add(new Pair<>("S'", beginSymbol));
        Rule.zeroIndex = rules.size() - 1;
        //加入全部的内容
        Enclosure e = new Enclosure(rules);
        //获取闭包内容
        return BFS(e.items);
    }

    /**
     * 从DFA 闭包转换为相对应的parsing table
     *
     * @return : 返回的parsing table
     */
    public static ParsingTable DFA2Table(Set<Enclosure> graph) {
        return new ParsingTable(graph);
    }

    /**
     * 进行正式的语法分析阶段
     *
     * @param input : 输入文本内容
     */
    public static void parsingInput(ParsingTable table, String input) {
        List<Pair<String, String>> reductionSeq = new LinkedList<>();
        input += symbols.end.getValue();            //添加 $ 终止符号
        Stack<String> stateStack = new Stack<>();   //状态栈
        Stack<String> symbolStack = new Stack<>();  //符号栈
        stateStack.push("0");
        symbolStack.push(symbols.end.getValue());
        int index = 0;
        String readPoint = String.valueOf(input.charAt(index));
        String tableCell;
        while (!(tableCell = table.query(stateStack.peek(), readPoint, true))
                .equals(symbols.accept.getValue())) {
            //Shift
            if (tableCell.startsWith("S")) {
                symbolStack.push(readPoint);
                stateStack.push(tableCell.substring(1));
                readPoint = String.valueOf(input.charAt(++index));
            }
            //reduction
            else if (tableCell.startsWith("r")) {
                String topState = stateStack.peek();
                int reducIndex = Integer.parseInt(tableCell.substring(1));
                Pair<String, String> pair = Rule.rules.get(reducIndex);
                reductionSeq.add(pair);
                for (int i = 0; i < pair.second.length(); ++i) {
                    symbolStack.pop();
                    stateStack.pop();
                }
                assert !stateStack.empty();
                symbolStack.push(pair.first);

                stateStack.push(table.query(stateStack.peek(), symbolStack.peek(), false));
            } else {
                //error
                assert false;
            }
        }

        for (Pair<String, String> pair : reductionSeq) {
            System.out.println(String.format("%s->%s", pair.first, pair.second));
        }

    }

    private static Set<Enclosure> BFS(Set<LR1Item> start) {
        Queue<Set<LR1Item>> queue = new LinkedList<>();
        List<Set<LR1Item>> visited = new LinkedList<>();
        queue.offer(start);
        //BFS获取全部的闭包内容
        while (!queue.isEmpty()) {
            Set<LR1Item> top = queue.poll();
            //首先求闭包
            top = Enclosure.encloseOf(top);
            if (isVisited(visited, top)) {
                continue;
            }
            //进行状态间扩展
            Set<Set<LR1Item>> itemSets = betStateTransition(top);
            for (Set<LR1Item> set : itemSets)
                queue.offer(set);
            Enclosure.enclosures.add(new Enclosure(top));
            visited.add(top);
        }
        //连边
        for (Enclosure enclosure : Enclosure.enclosures) {
            enclosure.getNext();
        }
        Enclosure.enclosures = Enclosure.enclosures
                .stream()
                .sorted(Comparator.comparingInt(o -> Integer.parseInt(o.identifier)))
                .collect(Collectors.toCollection(LinkedHashSet::new));
        return Enclosure.enclosures;
    }

    /**
     * 状态内部转换，其实就是根据core来求整个闭包
     *
     * @param core : 闭包核
     * @return : 整个闭包内容
     */
    public static Enclosure inStateTransition(Enclosure core) {
        return new Enclosure(core.items);
    }

    /**
     * 状态间转换
     */
    public static Set<Set<LR1Item>> betStateTransition(Set<LR1Item> items) {
        return Enclosure.getNext(items);
    }

    private static boolean isVisited(List<Set<LR1Item>> visited, Set<LR1Item> set) {
        for (Set<LR1Item> s : visited) {
            if (Enclosure.isSame(s, set)) return true;
        }
        return false;
    }
}
