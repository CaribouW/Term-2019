import graph.FANode;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

public class Entry {
    private static void read(String filename) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(new File(filename)));
        String input;
        while ((input = br.readLine()) != null) {
            Analyser.analyseEntry(input);
        }
    }

    public static void main(String[] args) {
        try {
            REs.initRe("REs.txt");
        } catch (IOException e) {
            e.printStackTrace();
        }
        FANode node = TreeGenerator.RE2NFATree(REs.idSMap);
        //transform the RE to the final optimized DFA
        Transformer.RE2NFA(node, REs.edgeSet);
        Transformer.NFA2DFA();
        Transformer.DFAOptimize();
        //Conduct reading and analyse the syntax
        try {
            read("input.txt");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
