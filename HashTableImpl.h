#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

template <int MAX_SIZE>
int HashTable<MAX_SIZE>::PRIMES[3] = {102523, 100907, 104659};

template <int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintLine(int tableIndex) const
{
    const HashData &data = table[tableIndex];

    if (data.sentinel == SENTINEL_MARK)
    {
        printf("[%03d]         : SENTINEL\n", tableIndex);
    }
    else if (data.sentinel == EMPTY_MARK)
    {
        printf("[%03d]         : EMPTY\n", tableIndex);
    }
    else
    {
        printf("[%03d] - [%03d] : ", tableIndex, data.lruCounter);
        printf("(%-5s) ", data.isCostWeighted ? "True" : "False");
        size_t sz = data.intArray.size();
        for (size_t i = 0; i < sz; i++)
        {
            if (i % 2 == 0)
                printf("[%03d]", data.intArray[i]);
            else
                printf("/%03d/", data.intArray[i]);

            if (i != sz - 1)
                printf("-->");
        }
        printf("\n");
    }
}

template <int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintTable() const
{
    printf("____________________\n");
    printf("Elements %d\n", elementCount);
    printf("[IDX] - [LRU] | DATA\n");
    printf("____________________\n");
    for (int i = 0; i < MAX_SIZE; i++)
    {
        PrintLine(i);
    }
}

template <int MAX_SIZE>
int HashTable<MAX_SIZE>::Hash(int startInt, int endInt, bool isCostWeighted)
{
    return startInt * PRIMES[0] + endInt * PRIMES[1] + (int)isCostWeighted * PRIMES[2];
}

template <int MAX_SIZE>
HashTable<MAX_SIZE>::HashTable()
{
    elementCount = 0;
    for (int i = 0; i < MAX_SIZE; i++)
    {
        table[i].lruCounter = 0;
        table[i].sentinel = EMPTY_MARK;
    }
}

template <int MAX_SIZE>
int HashTable<MAX_SIZE>::Insert(const std::vector<int> &intArray, bool isCostWeighted)
{
    if (intArray.size() < 1)
        throw InvalidTableArgException();

    if (elementCount > MAX_SIZE / CAPACITY_THRESHOLD)
        throw TableCapFullException(elementCount);

    int index = Hash(intArray[0], intArray[intArray.size() - 1], isCostWeighted);

    for (int i = 0;; i++)
    {

        int new_index = (index + i * i) % MAX_SIZE; 
        if (table[new_index].sentinel == EMPTY_MARK || table[new_index].sentinel == SENTINEL_MARK)
        {

            for (int k = 0; k < intArray.size(); k++)
            {
                table[new_index].intArray.push_back(intArray[k]);
            } // vector deep copy

            table[new_index].lruCounter++;

            table[new_index].startInt = intArray[0];
            table[new_index].endInt = intArray[intArray.size() - 1];
            table[new_index].sentinel = OCCUPIED_MARK;
            table[new_index].isCostWeighted = isCostWeighted;

            elementCount++;
            return 0;

        } // it is not present in the hashtable->reaches an empty spot.

        if (table[new_index].sentinel == OCCUPIED_MARK && table[new_index].startInt == intArray[0] && table[new_index].endInt == intArray[intArray.size() - 1] && table[new_index].isCostWeighted == isCostWeighted)
        {
            table[new_index].lruCounter++;
            return table[new_index].lruCounter - 1;
        }
    }

    return -1;
}

template <int MAX_SIZE>
bool HashTable<MAX_SIZE>::Find(std::vector<int> &intArray,
                               int startInt, int endInt, bool isCostWeighted,
                               bool incLRU)
{
    int index = Hash(startInt, endInt, isCostWeighted);

    for (int i = 0;; i++)
    {

        int new_index = (index + i * i) % MAX_SIZE;

        if (table[new_index].sentinel == EMPTY_MARK)
            return false;

        if (table[new_index].sentinel == OCCUPIED_MARK && table[new_index].startInt == startInt && table[new_index].endInt == endInt && table[new_index].isCostWeighted == isCostWeighted)
        {
            if (incLRU)
            {
                table[new_index].lruCounter++;
            }

            intArray.clear();

            for (int k = 0; k < table[new_index].intArray.size(); k++)
            {
                intArray.push_back(table[new_index].intArray[k]);
            }
            return true;
        }
    }

    return false;
}

template <int MAX_SIZE>
void HashTable<MAX_SIZE>::InvalidateTable()
{
    for (int i = 0; i < MAX_SIZE; i++)
    {
        table[i].lruCounter = 0;
        table[i].sentinel = EMPTY_MARK;
    }

    elementCount = 0;
}

template <int MAX_SIZE>
void HashTable<MAX_SIZE>::GetMostInserted(std::vector<int> &intArray) const
{
    int maximum = -1;

    for (int i = 0; i < MAX_SIZE; i++)
    {

        if (table[i].lruCounter > maximum)
        {

            maximum = table[i].lruCounter;
            intArray.clear();

            for (int k = 0; k < table[i].intArray.size(); k++)
            {
                intArray.push_back(table[i].intArray[k]);
            }
        }
    }
}

template <int MAX_SIZE>
void HashTable<MAX_SIZE>::Remove(std::vector<int> &intArray,
                                 int startInt, int endInt, bool isCostWeighted)
{
    int index = Hash(startInt, endInt, isCostWeighted);

    for (int i = 0;; i++)
    {

        int new_index = (index + i * i) % MAX_SIZE;

        if (table[new_index].sentinel == EMPTY_MARK)
            return;

        if (table[new_index].sentinel == OCCUPIED_MARK && table[new_index].startInt == startInt && table[new_index].endInt == endInt && table[new_index].isCostWeighted == isCostWeighted)
        {

            table[new_index].lruCounter = 0;
            table[new_index].startInt = -1;
            table[new_index].endInt = -1;
            table[new_index].sentinel = SENTINEL_MARK;
            elementCount--;

            intArray.clear();
            for (int k = 0; k < table[new_index].intArray.size(); k++)
            {
                intArray.push_back(table[new_index].intArray[k]);
            }
            return;
        }
    }
}

template <int MAX_SIZE>
void HashTable<MAX_SIZE>::RemoveLRU(int lruElementCount)
{
    std::vector<int> v;

    HashData x;

    for (int i = 0; i < lruElementCount; i++)
    {
        // remove 1 element in 1 loop
        int minimum = 50000;
        for (int k = 0; k < MAX_SIZE; k++)
        {
            if (table[k].sentinel == OCCUPIED_MARK && table[k].lruCounter < minimum)
            {
                minimum = table[k].lruCounter;
                x = table[k];
            }
        }

        Remove(v, x.startInt, x.endInt, x.isCostWeighted);
    }
}


template <int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintSortedLRUEntries() const
{
    MaxPairHeap<int, int> name;

    for (int i = 0; i < MAX_SIZE; i++)
    {
        if (table[i].lruCounter > 0)
        {
            Pair<int, int> p;
            p.key = table[i].lruCounter;
            p.value = i;
            name.push(p);
        }
    }

    while (!name.empty())
    {
        PrintLine(name.top().value);
        name.pop();
    }
}

#endif // HASH_TABLE_HPP