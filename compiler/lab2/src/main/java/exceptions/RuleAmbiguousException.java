package exceptions;

/**
 * 二义性文法异常
 */
public class RuleAmbiguousException extends RuntimeException {
    private final String msg;

    public RuleAmbiguousException(String msg) {
        this.msg = msg;
    }

    public String getMsg() {
        return msg;
    }
}
