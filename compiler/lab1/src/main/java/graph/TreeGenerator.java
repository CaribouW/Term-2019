package graph;

import java.util.*;

public class TreeGenerator {
    private static Map<Character, Integer> outPriority = new HashMap<>();
    private static Set<Character> operators = new HashSet<>(Arrays.asList('(', ')', '|', '*', '.'));
    private static int NodeCounter = 0;
    private static final String epsilon = "epsilon";
    private static final char dot = '.';
    private static final char loop = '*';
    private static final char or = '|';
    private static Set<FANode> dfsVisited;

    static {
        //out stack priority
        outPriority.put('(', -3);
        outPriority.put('|', 1);
        outPriority.put('*', 2);
        outPriority.put('.', 0);
        outPriority.put(')', -1);
    }

    public static FANode RE2NFATree(Map<String, String> REs) {
        List<FANode> nodes = new LinkedList<>();
        FANode root = new FANode(String.valueOf(NodeCounter++), false);
        for (String identifier : REs.keySet()) {
            String input = REs.get(identifier);
            String postFix = infix2PostFix(input);
            Stack<FANode> stack = new Stack<>();
            int index = 0;
            do {
                char ch = postFix.charAt(index);
                //simple node
                if (!operators.contains(ch)) {
                    FANode f = singleNode(String.valueOf(ch));
                    stack.push(f);
                } else {
                    FANode f2 = null, f1 = null;
                    switch (ch) {
                        case dot:
                            f2 = stack.pop();
                            f1 = stack.pop();
                            stack.push(connectNode(f1, f2));
                            break;
                        case loop:
                            FANode f = stack.pop();
                            stack.push(loopNode(f));
                            break;
                        case or:
                            f2 = stack.pop();
                            f1 = stack.pop();
                            stack.push(orNode(f1, f2));
                            break;
                    }
                }
                ++index;
            } while (index < postFix.length());
            if (!stack.empty()) {
                dfsVisited = new HashSet<>();
                FANode node = stack.peek();
                //dfs to insert identifier
                DFS(node, identifier);
                nodes.add(node);
            }
        }
        for (FANode node : nodes) {
            root.insertNext(epsilon, node);
        }
        //merge the node list to a single graph
        return root;
    }

    /**
     * Insert the identifier to set to end state
     *
     * @return
     */
    private static void DFS(FANode root, String identifier) {
        if (root.edgeFANodeMap.size() == 0) {
            root.identifier = identifier;
            root.isEnd = true;
            return;
        }
        for (String nodeKey : root.edgeFANodeMap.keySet()) {
            for (FANode cur : root.edgeFANodeMap.get(nodeKey)) {
                if (!dfsVisited.contains(cur)) {
                    dfsVisited.add(cur);
                    DFS(cur, identifier);
                }
            }
        }
    }

    //infix to postfix
    //从左到右遍历中缀表达式的每个数字和符号，若是数字就输出，即成为后缀表达式的一部分；
    // 若是符号，则判断其与栈顶符号的优先级，
    // 是右括号或优先级低于栈顶符号（乘除优先加减）则栈顶元素依次出找并输出，并将当前符号进栈，
    // 一直到最终输出后缀表达式为止
    public static String infix2PostFix(String infixInput) {
        // (a|b)* | a*
        StringBuilder sb = new StringBuilder();
        Stack<Character> stack = new Stack<>();
        int index = 0;
        while (index < infixInput.length()) {
            char ch = infixInput.charAt(index);
            if (!operators.contains(ch)) {
                sb.append(ch);
                ++index;
                continue;
            }
            if (stack.empty() || ch == '(') {
                stack.push(ch);
            } else {
                char top = stack.peek();
                if (outPriority.get(top) == outPriority.get(ch)) {
                    sb.append(top).append(ch);
                } else if (top == '(' && ch == ')') {
                } else if (outPriority.get(top) < outPriority.get(ch)) {
                    stack.push(ch);
                } else if (outPriority.get(top) > outPriority.get(ch)) {
                    stack.pop();
                    sb.append(top);
                    continue;
                }
            }
            ++index;
        }
        while (!stack.empty()) {
            char ch = stack.pop();
            sb.append(ch);
        }

        return sb.toString();
    }

    //f1->f2
    private static FANode singleNode(String edgeName) {
        FANode f1 = new FANode(String.valueOf(NodeCounter++), false);
        FANode f2 = new FANode(String.valueOf(NodeCounter++), false);
        f1.insertNext(edgeName, f2);
        return f1;
    }

    //a.b
    private static FANode connectNode(FANode f1, FANode f2) {
        for (String key : f1.edgeFANodeMap.keySet()) {
            List<FANode> faNodes = f1.edgeFANodeMap.get(key);
            if (1 == faNodes.size()) {
                faNodes.get(0).insertNext(epsilon, f2);
            }
            //.insertNext(epsilon, f2)
            break;
        }
        return f1;
    }

    //a|b
    private static FANode orNode(FANode f1, FANode f2) {
        FANode begin = new FANode(String.valueOf(NodeCounter++), false);
        FANode end = new FANode(String.valueOf(NodeCounter++), false);
        //begin
        begin.insertNext(epsilon, f1);
        begin.insertNext(epsilon, f2);
        //end
        for (String key : f1.edgeFANodeMap.keySet()) {
            List<FANode> faNodes = f1.edgeFANodeMap.get(key);
            if (1 == faNodes.size()) {
                faNodes.get(0).insertNext(epsilon, end);
            }
            break;
        }
        for (String key : f2.edgeFANodeMap.keySet()) {
            List<FANode> faNodes = f2.edgeFANodeMap.get(key);
            if (1 == faNodes.size()) {
                faNodes.get(0).insertNext(epsilon, end);
            }
            break;
        }
        return begin;
    }

    //a*
    private static FANode loopNode(FANode inner) {
        FANode two = null;
        for (String key : inner.edgeFANodeMap.keySet()) {
            List<FANode> faNodes = inner.edgeFANodeMap.get(key);
            if (1 == faNodes.size()) {
                two = faNodes.get(0);
            }
            break;
        }
        assert two != null;
        FANode begin = new FANode(String.valueOf(NodeCounter++), false);
        FANode end = new FANode(String.valueOf(NodeCounter++), false);
        //begin
        begin.insertNext(epsilon, inner);
        begin.insertNext(epsilon, end);
        //two
        two.insertNext(epsilon, inner);
        two.insertNext(epsilon, end);
        return begin;
    }
}
