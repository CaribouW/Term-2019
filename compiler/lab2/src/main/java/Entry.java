import java.io.IOException;

/**
 * The whole entry for the programme
 */
public class Entry {

    public static void main(String[] args) {
        Rule rule = new Rule();
        try {
            rule.setupRules("rules.txt");
            Rule.rules.forEach(pair -> System.out.println(String.format("rule is %s->%s", pair.first, pair.second)));
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
