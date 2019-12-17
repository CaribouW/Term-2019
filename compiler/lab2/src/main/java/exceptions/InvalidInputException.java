package exceptions;

/**
 * 不合法的输入
 */
public class InvalidInputException extends RuntimeException {
    private final String msg;

    public InvalidInputException(String message) {
        this.msg = message;
    }

    public String getMsg() {
        return msg;
    }
}
