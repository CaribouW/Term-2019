import java.io.Serializable;
import java.util.LinkedList;
import java.util.List;

/**
 * LR(1) item
 */
enum symbols {
    epsilon("epsilon"),
    zeroGen("S'"),
    end("$"),
    accept("accept");
    private String value;

    symbols(String symbol) {
        this.value = symbol;
    }

    public String getValue() {
        return value;
    }
}

public class LR1Item implements Serializable {
    public Pair<String, String> rule;   //文法映射
    public int dotIndex;                //点的位置
    public List<String> predictiveSymbols;//预测符

    public LR1Item(Pair<String, String> rule, int dotIndex, List<String> predictiveSymbols) {
        this.rule = new Pair<>(rule.first, rule.second);
        this.dotIndex = dotIndex;
        this.predictiveSymbols = new LinkedList<>(predictiveSymbols);
    }

    //dot是否在末尾
    public boolean isDotEnd() {
        return dotIndex == rule.second.length();
    }

    @Override
    public boolean equals(Object obj) {
        LR1Item item = (LR1Item) obj;
        boolean result = dotIndex == item.dotIndex &&
                rule.equals(item.rule) &&
                predictiveSymbols.size() == item.predictiveSymbols.size();
        if (!result) return false;
        else {
            for (String s : predictiveSymbols) {
                if (!item.predictiveSymbols.contains(s)) return false;
            }
        }
        return true;
    }

    @Override
    public int hashCode() {
        return rule.hashCode() + dotIndex + predictiveSymbols
                .stream()
                .map(String::hashCode)
                .reduce(Integer::sum)
                .get();
    }

    public String getPointValue() {
        if (isDotEnd()) return null;
        return String.valueOf(this.rule.second.charAt(dotIndex));
    }

    public String restPart() {
        if (dotIndex == rule.second.length() - 1) return "";
        return String.valueOf(rule.second.charAt(dotIndex + 1));
    }
}
