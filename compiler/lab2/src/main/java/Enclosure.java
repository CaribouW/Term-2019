import java.util.*;
import java.util.stream.Collectors;

public class Enclosure {
    public static Set<Enclosure> enclosures = new HashSet<>();
    public static int counter = 0;
    public Set<LR1Item> items;
    public String identifier;
    public Map<String, String> outEdges = new HashMap<>();     //边名称->闭包标识符的映射


    /**
     * 初始闭包生成, 不包含dot
     *
     * @param core : 闭包的core
     */
    public Enclosure(List<Pair<String, String>> core) {
        this.identifier = String.valueOf(counter);
        Set<LR1Item> lr = new HashSet<>();
        for (Pair<String, String> pair : core) {
            if (pair.first.equals(symbols.zeroGen.getValue())) {//S'
                //添加新的
                lr.add(new LR1Item(pair,
                        0,
                        Collections.singletonList(symbols.end.getValue() + "|0"))
                );
                break;
            }
        }
        this.items = lr;
    }

    /**
     * @param core : 闭包核, 需要返回整个闭包
     */
    public Enclosure(Set<LR1Item> core) {
        this.items = encloseOf(core);
        this.identifier = String.valueOf(counter++);
    }


    public void getNext() {
        Map<String, Set<LR1Item>> edge2RuleMap = new HashMap<>();        //一条边引出的LR(1)项映射
        for (LR1Item item : items) {
            //对于dot合法的LR(1)项, 求出下一个闭包的core
            if (!item.isDotEnd()) {
                //获取边
                String edgeName = item.getPointValue();
                //产生下一个
                LR1Item single = new LR1Item(item.rule,
                        item.dotIndex + 1,
                        item.predictiveSymbols);
                if (!edge2RuleMap.containsKey(edgeName)) {
                    edge2RuleMap.put(edgeName, Collections.singleton(single));
                } else {
                    Set<LR1Item> rs = new HashSet<>(edge2RuleMap.get(edgeName));
                    rs.add(single);
                    edge2RuleMap.put(edgeName, rs);
                }
            }
        }
        //对于每一条边发出的 LR (1) 项目集合
        for (String edgeName : edge2RuleMap.keySet()) {
            Set<LR1Item> lr1Items = edge2RuleMap.get(edgeName);
            lr1Items = encloseOf(lr1Items);
            Set<LR1Item> finalLr1Items = lr1Items;
            assert enclosures.size() > 0;
            Enclosure nextE = enclosures.stream()
                    .filter(enclosure -> isSame(finalLr1Items, enclosure.items)).findAny().get();
            //添加出边
            outEdges.put(edgeName, nextE.identifier);
        }
    }

    /**
     * 当前闭包已经是epsilon闭包, 只是缺少出边
     * 当前闭包,填充edges出边
     *
     * @return : 出边的所有闭包集合
     */
    public static Set<Set<LR1Item>> getNext(Set<LR1Item> items) {
        Set<Set<LR1Item>> ans = new HashSet<>();
        Map<String, Set<LR1Item>> edge2RuleMap = new HashMap<>();        //一条边引出的LR(1)项映射
        for (LR1Item item : items) {
            //对于dot合法的LR(1)项, 求出下一个闭包的core
            if (!item.isDotEnd()) {
                //获取边
                String edgeName = item.getPointValue();
                //产生下一个
                LR1Item single = new LR1Item(item.rule,
                        item.dotIndex + 1,
                        item.predictiveSymbols);
                if (!edge2RuleMap.containsKey(edgeName)) {
                    edge2RuleMap.put(edgeName, Collections.singleton(single));
                } else {
                    Set<LR1Item> rs = new HashSet<>(edge2RuleMap.get(edgeName));
                    rs.add(single);
                    edge2RuleMap.put(edgeName, rs);
                }
            }
        }
        //对于每一条边发出的 LR (1) 项目集合
        for (String edgeName : edge2RuleMap.keySet()) {
            Set<LR1Item> lr1Items = edge2RuleMap.get(edgeName);
            //添加出边
            ans.add(lr1Items);
        }
        return ans;
    }

    /**
     * 获取指定非终结符的 first
     *
     * @param V_N:  非终结符
     * @param rules : 原文法内容
     * @return : 包含了终结符和相对应的文法
     */
    public static Set<String> first(String V_N, List<Pair<String, String>> rules) {
        Set<String> ans = new HashSet<>();
        String first = V_N.substring(0, 1);
        //ignore V_N
        if (Rule.isV_N(first)) {
            assert false;
        } else {
            ans.add(first);
        }
        return ans.stream()
                .map(str -> {
                    if (str.equals(symbols.end.getValue())) {
                        return str + "|0";
                    }
                    for (int index = 0; index < rules.size(); ++index) {
                        String right = rules.get(index).second;
                        if (right.contains(first)) {
                            str += "|" + index;
                            break;
                        }
                    }
                    return str;
                }).collect(Collectors.toSet());
    }

    /**
     * 获取指定非终结符的 follow
     *
     * @param V_N   : 非终结符
     * @param rules : 原文法内容
     */
    public static Set<String> follow(String V_N, List<Pair<String, String>> rules) {
        return new HashSet<>();
    }

    /**
     * 根据core获取闭包
     *
     * @param core : 核
     * @return : epsilon 闭包
     */
    public static Set<LR1Item> encloseOf(Set<LR1Item> core) {
        while (true) {
            Set<LR1Item> newItems = new HashSet<>();
            for (LR1Item item : core) {
                if (item.isDotEnd()) continue;
                String nextVal = item.getPointValue();
                assert nextVal != null;
                //是一个非终结符
                if (Rule.isV_N(nextVal)) {
                    for (Pair<String, String> rule : Rule.rules) {
                        if (rule.first.equals(nextVal)) {
                            //获取first, 同时需要得到相应规约的是哪一个文法
                            for (String symbol : item.predictiveSymbols) {
                                String V_N = item.restPart() + symbol; // first()
                                List<String> predictiveSymbols =
                                        new LinkedList<>(first(V_N, Rule.rules));
                                LR1Item cur = new LR1Item(rule, 0, predictiveSymbols);
                                if (!core.contains(cur))
                                    newItems.add(cur);
                            }
                        }
                    }
                }
            }
            Set<LR1Item> tmp = new HashSet<>(core);
            core.addAll(newItems);
            if (isSame(core, tmp))
                break;
        }
        return core;
    }


    public static void input(Set<Enclosure> en, Enclosure e) {
        Enclosure tmp = null;
        for (Enclosure target : en) {
            if (target.equals(e)) {
                tmp = target;
                break;
            }
        }
        tmp.outEdges = new HashMap<>(e.outEdges);
        tmp.items = new HashSet<>(e.items);
        en.add(tmp);
    }

    public static boolean isSame(Set<LR1Item> i1, Set<LR1Item> i2) {
        if (i1.size() != i2.size()) return false;
        for (LR1Item item : i1) {
            if (!i2.contains(item)) return false;
        }
        return true;
    }


    @Override
    public boolean equals(Object obj) {
        Enclosure enclosure = (Enclosure) obj;
        if (items.size() != enclosure.items.size())
            return false;
        else {
            for (LR1Item item : items) {
                if (!enclosure.items.contains(item)) return false;
            }
        }
        return true;
    }

    @Override
    public int hashCode() {
        return this.items.stream()
                .map(LR1Item::hashCode)
                .reduce(Integer::sum)
                .get();
    }
}
