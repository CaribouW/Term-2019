import java.util.*;

public class ParsingTree {
    private static Node root;       //根节点

    public static class Node {
        final String identifier;
        List<Node> next;

        /**
         * @param identifier : 根节点id
         * @param offspring  : 后代链表
         */
        Node(String identifier, List<Node> offspring) {
            this.identifier = identifier;
            this.next = new LinkedList<>(offspring);
        }

        Node(String identifier) {
            this.identifier = identifier;
            this.next = new LinkedList<>();
        }

        Node() {
            this.identifier = "";
        }

        public boolean isNull() {
            return identifier.isEmpty();
        }

    }

    /**
     * 根据归约序列, 生成语法分析树
     *
     * @param reductionSeq: 归约序列
     * @return : 语法分析树
     */
    public static Node generateTree(List<Pair<String, String>> reductionSeq) {
        Stack<Node> stack = new Stack<>();
        for (Pair<String, String> reduction : reductionSeq) {
            String rootId = reduction.first, reductionContent = reduction.second;
            List<Node> offspring = new LinkedList<>();
            for (int i = reductionContent.length() - 1; i >= 0; --i) {
                String target = String.valueOf(reductionContent.charAt(i));
                if (Rule.isV_N(target)) {
                    if (stack.empty() || !stack.peek().identifier.equals(target)) assert false;
                    else {
                        offspring.add(stack.pop());
                    }
                } else {
                    offspring.add(new Node(target));
                }
            }
            //push current root
            stack.push(new Node(rootId, offspring));
        }
        assert stack.size() >= 1;
        return stack.pop();
    }

    /**
     * BFS 层次序遍历parsing tree
     *
     * @param root : 根节点
     * @return : 一个二维数组，每一维都是语法分析树对应层次的内容
     */
    public static List<List<Node>> levelTraversalTree(final Node root) {
        Queue<Node> queue = new ArrayDeque<>();
        List<List<Node>> ans = new LinkedList<>();
        List<Node> singleLine = new LinkedList<>();
        queue.offer(root);
        Node nullNode = new Node();
        queue.offer(nullNode);
        while (!queue.isEmpty()) {
            Node ptr = queue.poll();
            if (ptr.isNull()) {
                if (queue.isEmpty()) break;
                //来到下一行
                queue.offer(ptr);
                ans.add(singleLine);
                singleLine = new LinkedList<>();
            } else {
                singleLine.add(ptr);
                for (Node node : ptr.next) {
                    queue.offer(node);
                }
            }
        }
        return ans;
    }

    public static void displaytree(Node root, int level) {       //递归显示树
        StringBuilder preStr = new StringBuilder();
        preStr.append("\t".repeat(Math.max(0, level)));

        for (int i = 0; i < root.next.size(); i++) {
            Node son = root.next.get(i);
            System.out.println(String.format("%s---【%s】", preStr, son.identifier));
            if (!son.next.isEmpty()) {
                displaytree(son, level + 1);
            }
        }
    }

    /**
     * 打印多叉树
     * 打印不好看,我太难了
     */
    private static void printTree(final List<List<Node>> tree) {
        assert tree.size() > 0;
        Queue<Integer> sizeQueue = new LinkedList<>();
        for (Node n : tree.get(0)) {
            System.out.print(String.format("%s\t\t", n.identifier));
            sizeQueue.offer(n.next.size());
        }
        System.out.println();
        for (int i = 1; i < tree.size(); ++i) {
            Queue<Integer> subQueue = new LinkedList<>();
            int index = 0;
            while (index < tree.get(i).size()) {
                int end = sizeQueue.poll();
                for (int j = 0; j < end; ++j, ++index) {
                    System.out.print(String.format("%s\t", tree.get(i).get(j).identifier));
                    subQueue.offer(tree.get(i).get(j).next.size());
                }
                //不同的
                System.out.print('\t');
            }
            System.out.println();
            assert sizeQueue.size() == 0;
            sizeQueue.addAll(subQueue);
        }
    }
}
