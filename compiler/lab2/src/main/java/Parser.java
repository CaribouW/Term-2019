import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Set;

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
        //加入全部的内容
        Enclosure e = new Enclosure(rules);
        //获取闭包内容
        return BFS(e.items);
    }

    /**
     * 从DFA 闭包转换为相对应的parsing table
     */
    public static void DFA2Table(Set<Enclosure> graph) {

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
