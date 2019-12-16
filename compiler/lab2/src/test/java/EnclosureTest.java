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
        assertEquals(13, enclosure.items.size());
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
        assertEquals(3, nextEnclosures.size());
    }
}
