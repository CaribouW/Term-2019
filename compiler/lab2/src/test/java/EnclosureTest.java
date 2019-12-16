import org.junit.Before;
import org.junit.Test;

import java.io.IOException;
import java.util.Set;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class EnclosureTest {
    private Enclosure enclosure;

    @Before
    public void init() throws IOException {
        Rule rule = new Rule();
        rule.setupRules("rules.txt");

    }

    //初始化, 判定第一个闭包的正确性测试
    @Test
    public void enclosureTest1() {
        Rule.rules.add(new Pair<>("S'", "S"));//S' -> S
        enclosure = new Enclosure(Rule.rules);
        //第一个闭包
        int size = 13;
        assertEquals(size, enclosure.items.size());
    }

    //first 函数测试
    @Test
    public void firstTest() {
        Rule.rules.add(new Pair<>("S'", "S"));//S' -> S
        Set<String> set = Enclosure.first("+E", Rule.rules);
        assertEquals(1, set.size());
        assertTrue(set.contains("+"));
    }

    @Test
    public void parserTest() {
        Rule.rules.add(new Pair<>("S'", "S"));//S' -> S
        enclosure = new Enclosure(Rule.rules);
        Set<Enclosure> nextEnclosures = enclosure.getNext();
        assertEquals(1, nextEnclosures.size());
        assertEquals(1, enclosure.outEdges.size());
        Set<Enclosure> graph = Parser.rule2Enclosure(Rule.rules, "S");
        assertEquals(4, graph.size());
//        ParsingTable table = new ParsingTable(graph);
//        assertEquals(graph.size(),table.size());
//        table.printout();
    }
}
