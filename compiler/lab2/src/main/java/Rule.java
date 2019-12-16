import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

public class Rule {
    private static List<SingleNode> rawRules = new LinkedList<>();
    public static List<Pair<String, String>> rules = new LinkedList<>();
    //非终结符
    private static Set<String> V_NSet = new HashSet<>();

    static class SingleNode {
        String identifier;
        List<String> derivations;

        SingleNode(String identifier, String deri) {
            this.identifier = identifier;
            String[] strings = deri.split("\\|");
            this.derivations = Arrays.asList(strings);
        }
    }

    void setupRules(String filepath) throws IOException {
        BufferedReader br = new BufferedReader(new FileReader(new File(filepath)));
        String input;
        while ((input = br.readLine()) != null) {
            String[] strings = input.split("->");
            if (strings.length != 2) {
                System.out.println(String.format("rule %s definition Error", input));
                break;
            }
            SingleNode r = new SingleNode(strings[0], strings[1]);
            rawRules.add(r);
        }
        rawRules.forEach(raw -> {
            V_NSet.add(raw.identifier);
            raw.derivations.forEach(deri -> rules.add(new Pair<>(raw.identifier, deri)));
        });
    }

    public static boolean isV_N(String target) {
        return V_NSet.contains(target);
    }
}
