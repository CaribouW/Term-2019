public class Entry {
    public static void main(String[] args) {
        Transformer.RE2NFA(REs.genS);
        Transformer.NFA2DFA();
        Transformer.DFAOptimize();
    }
}
