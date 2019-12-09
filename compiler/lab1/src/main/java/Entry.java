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
        Transformer.RE2NFA(REs.genS);
        Transformer.NFA2DFA();
        Transformer.DFAOptimize();
        try {
            read("README.md");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
