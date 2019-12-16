public class Pair<K, V> {
    public K first;
    public V second;

    public Pair(K first, V second) {
        this.first = first;
        this.second = second;
    }

    @Override
    public boolean equals(Object obj) {
        Pair<K, V> pair = (Pair<K, V>) obj;
        return pair.first.equals(first) &&
                pair.second.equals(second);
    }

    @Override
    public int hashCode() {
        return first.hashCode() + second.hashCode();
    }
}
