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
        return pair.first == first &&
                pair.second == second;
    }
}
