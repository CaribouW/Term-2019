import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.Set;

import static org.junit.Assert.assertEquals;

public class EnclosureTest {
    private Enclosure enclosure;

    @Before
    public void init() throws IOException {
        Rule rule = new Rule();
        rule.setupRules("rules.txt");

    }
    //first 函数测试
    @Test
    public void firstTest() {
        Rule.rules.add(new Pair<>("S'", "S"));//S' -> S
        Set<String> set = Enclosure.first("+E", Rule.rules);
        assertEquals(1, set.size());
    }

    @Test
    public void parserTest() {
        Set<Enclosure> graph = Parser.rule2Enclosure(Rule.rules, "S");
        Set<Enclosure> target = Enclosure.enclosures;
        assertEquals(Enclosure.enclosures.size(), graph.size());
        ParsingTable table = Parser.DFA2Table(graph);
        Parser.parsingInput(table, "j-j;");
    }
}
