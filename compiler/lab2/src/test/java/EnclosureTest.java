import org.junit.Before;
import org.junit.Test;

import java.io.IOException;

import static org.junit.Assert.assertEquals;

public class EnclosureTest {
    private Enclosure enclosure;

    @Before
    public void init() throws IOException {
        Rule rule = new Rule();
        rule.setupRules("rules.txt");
        Rule.rules.add(new Pair<>("S'", "S"));//S' -> S

    }

    //初始化, 判定第一个闭包的正确性测试
    @Test
    public void enclosureTest1() {
        enclosure = new Enclosure(Rule.rules);
        //第一个闭包
        assertEquals(3, enclosure.items.size());
    }
}
