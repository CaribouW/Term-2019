import exceptions.InvalidInputException;
import exceptions.RuleAmbiguousException;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.Set;

/**
 * The whole entry for the programme
 */
public class Entry {

    public static void main(String[] args) {
        Rule rule = new Rule();
        try {
            rule.setupRules("rules.txt");
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            //Rule to DFA transition graph
            Set<Enclosure> graph = Parser.rule2Enclosure(Rule.rules, "S");
            ParsingTable table = Parser.DFA2Table(graph);
            //read the input
            String[] inputs = readInput("input.txt").split("\n");
            for (String input : inputs) {
                try {
                    Parser.parsingInput(table, input);
                } catch (RuleAmbiguousException e) {
                    System.out.println(e.getMsg());
                } catch (InvalidInputException e) {
                    System.out.println(e.getMsg());
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    private static String readInput(String filepath) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(new File(filepath)));
        StringBuilder sb = new StringBuilder();
        String input;
        while ((input = br.readLine()) != null) {
            sb.append(input + "\n");
        }
        return sb.toString();
    }
}
