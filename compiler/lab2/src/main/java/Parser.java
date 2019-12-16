import java.util.HashSet;
import java.util.List;
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
        Enclosure.enclosures.add(new Enclosure(rules));
        //作为访问限制，保证每一个闭包只做一次分析
        Set<Enclosure> visited = new HashSet<>();
        while (!Enclosure.enclosures.isEmpty()) {
            Set<Enclosure> newAddList = new HashSet<>();
            for (Enclosure e : Enclosure.enclosures) {
                //对于没有进行转换过的闭包
                if (!visited.contains(e)) {
                    //状态内转换,更新e
                    Enclosure I = inStateTransition(e);
                    //基于 I 这个状态，进行状态间转换
                    Set<Enclosure> nextEnclosure = betStateTransition(I);
                    //添加新的状态 ; 如果存在的话，就是刷新这个闭包
                    newAddList.add(I);
                    assert nextEnclosure != null;
                    //把所有新的闭包(还只是core的状态)加入
                    for (Enclosure next : nextEnclosure) {
                        if (!Enclosure.enclosures.contains(next))
                            newAddList.add(next);
                    }
                    visited.add(I);
                }
            }
            if (0 == newAddList.size())
                break;
            Enclosure.enclosures.addAll(newAddList);
//            for (Enclosure e : newAddList) {
//                if (Enclosure.enclosures.contains(e)) {
//                    Enclosure.input(Enclosure.enclosures, e);
//                } else {
//                    Enclosure.enclosures.add(e);
//                }
//            }
        }

        return Enclosure.enclosures;
    }

    /**
     * 从DFA 闭包转换为相对应的parsing table
     */
    public static void DFA2Table(Set<Enclosure> graph) {

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
    public static Set<Enclosure> betStateTransition(Enclosure enclosure) {
        return enclosure.getNext();
    }

}
