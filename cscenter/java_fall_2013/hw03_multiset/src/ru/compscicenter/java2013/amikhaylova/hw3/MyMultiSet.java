package ru.compscicenter.java2013.amikhaylova.hw3;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import ru.compscicenter.java2013.collections.MultiSet;

/**
 * This implementation of MultiSet interface allows null elements.
 *
 * @author Alexandra Mikhaylova mikhaylova.alexandra.a@gmail.com
 */
public class MyMultiSet<E> implements MultiSet<E> {

    private HashMap<E, Integer> elements;
    private int size;

    public MyMultiSet() {
        elements = new HashMap<>();
        size = 0;
    }

    public MyMultiSet(Collection<? extends E> collection) {
        elements = new HashMap<>();
        size = 0;
        addAll(collection);
    }

    @Override
    public int size() {
        return size;
    }

    @Override
    public Iterator<E> iterator() {
        return toList().iterator();
    }

    public boolean add(E e) {
        add(e, 1);
        return true;
    }

    public int add(E e, int occurrences) {
        Integer value = elements.get(e);
        value = value == null ? 0 : value;
        elements.put(e, value + occurrences);
        size += occurrences;
        return value;
    }

    @Override
    public boolean remove(Object e) {
        return remove(e, 1) > 0;
    }

    @Override
    public int remove(Object e, int occurrences) {
        if (occurrences < 0) {
            throw new IllegalArgumentException("Occurrence count is negative");
        }

        if (occurrences == 0) {
            return 0;
        }

        Integer value = elements.get(e);
        if (value == null) {
            return 0;
        }
        if (value - occurrences <= 0) {
            elements.remove(e);
            size -= value;
        } else {
            elements.put((E) e, value - occurrences);
            size -= occurrences;
        }
        return value;
    }

    @Override
    public int count(Object e) {
        Integer value = elements.get(e);
        return value == null ? 0 : value;
    }

    @Override
    public boolean contains(Object o) {
        return elements.containsKey(o);
    }

    @Override
    public boolean containsAll(Collection<?> c) {
        if (c == null) {
            return false;
        }

        for (Object o : c) {
            if (!contains(o)) {
                return false;
            }
        }
        return true;
    }

    @Override
    public Object[] toArray() {
        return toList().toArray(new Object[size]);
    }

    @Override
    public <T> T[] toArray(T[] a) {
        return toList().toArray(a);
    }

    @Override
    public boolean removeAll(Collection<?> c) {
        if (c == null) {
            return false;
        }

        boolean result = false;
        int occurrences;
        for (Object o : c) {
            occurrences = removeElement(o);
            result = occurrences > 0;
        }
        return result;
    }

    @Override
    public boolean retainAll(Collection<?> c) {
        if (c == null) {
            return false;
        }

        boolean result = false;
        for (E e : getElementsMap().keySet()) {
            if (!c.contains(e)) {
                removeElement(e);
                result = true;
            }
        }
        return result;
    }

    @Override
    public void clear() {
        elements.clear();
        size = 0;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof MultiSet)) {
            return false;
        }

        Map<E, Integer> oMap = ((MyMultiSet) o).getElementsMap();
        for (Map.Entry<E, Integer> entry : oMap.entrySet()) {
            if (count(entry.getKey()) != entry.getValue()) {
                return false;
            }
        }
        return true;
    }

    @Override
    public boolean isEmpty() {
        return size == 0;
    }

    @Override
    public boolean addAll(Collection<? extends E> c) {
        if (c == null) {
            throw new NullPointerException("The specified collection is null");
        }

        for (E e : c) {
            add(e);
        }

        return true;
    }

    private Map<E, Integer> getElementsMap() {
        return new HashMap<>(elements);
    }

    private List<E> toList() {
        List<E> list = new ArrayList<>(size);
        for (Map.Entry<E, Integer> entry : elements.entrySet()) {
            for (int i = 0; i < entry.getValue(); i++) {
                list.add(entry.getKey());
            }
        }
        return list;
    }

    private int removeElement(Object e) {
        return remove(e, Integer.MAX_VALUE);
    }

    /**
     *  A few usage examples.
     */
    public static void main(String[] args) {
        MyMultiSet<String> myMultiSet1 = new MyMultiSet<>();
        myMultiSet1.add("Winter");
        myMultiSet1.add("Summer", 10);
        myMultiSet1.add(null, 3);
        System.out.println(myMultiSet1.contains(null));
        System.out.println(myMultiSet1.remove(null, 1));
        System.out.println(myMultiSet1.remove(null, 1));

        MyMultiSet<String> myMultiSet2 = new MyMultiSet<>();
        myMultiSet2.add("Winter");
        myMultiSet2.add("Summer", 10);
        myMultiSet2.add(null, 1);
        System.out.println(myMultiSet1.equals(myMultiSet2));
        System.out.println(myMultiSet2.toArray().length);

        List<String> list1 = new ArrayList<>();
        list1.add("Winter");
        List<String> list2 = new ArrayList<>();
        list2.add("Winter");
        MyMultiSet<String> myMultiSet3 = new MyMultiSet<>(list1);
        MyMultiSet<String> myMultiSet4 = new MyMultiSet<>(list2);
        System.out.println(myMultiSet3.equals(myMultiSet4) + "\n");

        /*
        myMultiSet1.remove("Winter", 7);
        for (String s : myMultiSet1) {
            System.out.println(s);
        }
        */

        myMultiSet2.retainAll(list1);
        for (String s : myMultiSet2) {
            System.out.println(s);
        }

        MyMultiSet<Object> myMultiSet5 = new MyMultiSet<>();
        myMultiSet5.add(null);
        myMultiSet5.add(null);
        myMultiSet5.add(null, 0);
        myMultiSet5.add(null);
        System.out.println(myMultiSet5.count(null));
    }
}
