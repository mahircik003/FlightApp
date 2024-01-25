#ifndef INT_PAIR_H
#define INT_PAIR_H

#include <queue>

template <class K, class V>
struct Pair
{
    K key;
    V value;
};

template <class PairType>
struct LessComparator
{
    inline bool operator()(PairType left, PairType right)
    {
        return left.key < right.key;
    }
};

template <class PairType>
struct GreaterComparator
{
    inline bool operator()(PairType left, PairType right)
    {
        return left.key > right.key;
    }
};

template <class K, class V>
using MaxPairHeap = std::priority_queue<Pair<K, V>,
                                        std::vector<Pair<K, V>>,
                                        LessComparator<Pair<K, V>>>;

template <class K, class V>
using MinPairHeap = std::priority_queue<Pair<K, V>,
                                        std::vector<Pair<K, V>>,
                                        GreaterComparator<Pair<K, V>>>;

typedef std::priority_queue<Pair<float, int>, std::vector<Pair<float, int>>,
                            LessComparator<Pair<float, int>>>
    MinPairHeapFloatInt;


#endif // INT_PAIR_H