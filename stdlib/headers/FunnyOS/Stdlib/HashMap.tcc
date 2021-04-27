#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASHMAP_HPP
#error "Include HashMap.hpp instead"
#endif

#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASHMAP_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASHMAP_TCC
#include "Algorithm.hpp"

namespace FunnyOS::Stdlib {

    template <typename K, typename V>
    HashMapEntry<K, V>::HashMapEntry(K key, V value) : Key(Move(key)), Value(Move(value)) {}

    template <typename K, typename V>
    typename HashMap<K, V>::ConstIterator HashMap<K, V>::ConstIterator::operator++() noexcept {
        auto snapshot = *this;

        m_keyIndex++;
        SkipInvalid();

        return snapshot;
    }

    template <typename K, typename V>
    typename HashMap<K, V>::ConstIterator HashMap<K, V>::ConstIterator::operator++(int) noexcept {
        return operator++();
    }

    template <typename K, typename V>
    const HashMapEntry<K, V>& HashMap<K, V>::ConstIterator::operator*() const noexcept {
        return m_map.m_buckets[m_bucketIndex].Values[m_keyIndex];
    }

    template <typename K, typename V>
    const HashMapEntry<K, V>* HashMap<K, V>::ConstIterator::operator->() const noexcept {
        return &m_map.m_buckets[m_bucketIndex].Values[m_keyIndex];
    }

    template <typename K, typename V>
    bool HashMap<K, V>::ConstIterator::operator==(const HashMap::ConstIterator& other) const noexcept {
        return &m_map == &other.m_map && m_bucketIndex == other.m_bucketIndex && m_keyIndex == other.m_keyIndex;
    }

    template <typename K, typename V>
    bool HashMap<K, V>::ConstIterator::operator!=(const HashMap::ConstIterator& other) const noexcept {
        return !(*this == other);
    }

    template <typename K, typename V>
    bool HashMap<K, V>::ConstIterator::operator==(HashMap::ConstIterator& other) noexcept {
        return &m_map == &other.m_map && m_bucketIndex == other.m_bucketIndex && m_keyIndex == other.m_keyIndex;
    }

    template <typename K, typename V>
    bool HashMap<K, V>::ConstIterator::operator!=(HashMap::ConstIterator& other) noexcept {
        return !(*this == other);
    }

    template <typename K, typename V>
    HashMap<K, V>::ConstIterator::ConstIterator(const HashMap<K, V>& map, size_t bucketIndex, size_t keyIndex)
        : m_map{map}, m_bucketIndex{bucketIndex}, m_keyIndex{keyIndex} {
        SkipInvalid();
    }

    template <typename K, typename V>
    void HashMap<K, V>::ConstIterator::SkipInvalid() {
        if (m_bucketIndex >= m_map.m_buckets.Size()) {
            return;
        }

        while (m_keyIndex >= m_map.m_buckets[m_bucketIndex].Values.Size()) {
            m_bucketIndex++;
            m_keyIndex = 0;

            if (m_bucketIndex >= m_map.m_buckets.Size()) {
                break;
            }
        }
    }

    template <typename K, typename V>
    HashMapEntry<K, V>& HashMap<K, V>::Iterator::operator*() noexcept {
        return this->m_map.m_buckets[this->m_bucketIndex].Values[this->m_keyIndex];
    }

    template <typename K, typename V>
    HashMapEntry<K, V>* HashMap<K, V>::Iterator::operator->() noexcept {
        return &this->m_map.m_buckets[this->m_bucketIndex].Values[this->m_keyIndex];
    }

    template <typename K, typename V>
    HashMap<K, V>::Iterator::Iterator(HashMap<K, V>& map, size_t bucketIndex, size_t keyIndex)
        : ConstIterator(map, bucketIndex, keyIndex) {}

    template <typename K, typename V>
    HashMap<K, V>::HashMap() : HashMap(DEFAULT_LOAD_FACTOR) {}

    template <typename K, typename V>
    HashMap<K, V>::HashMap(load_factor_t loadFactor) : HashMap(DEFAULT_INITIAL_CAPACITY, loadFactor) {}

    template <typename K, typename V>
    HashMap<K, V>::HashMap(size_t initialCapacity, load_factor_t loadFactor)
        : m_loadFactor{loadFactor}, m_buckets{initialCapacity, 2}, m_size{0} {
        RecalculateBuckets();
    }

    template <typename K, typename V>
    size_t HashMap<K, V>::Size() const noexcept {
        return m_size;
    }

    template <typename K, typename V>
    V& HashMap<K, V>::operator[](const K& key) {
        V* value = GetOptional(key);

        if (value == nullptr) {
            F_ERROR_WITH_MESSAGE(HashMapKeyNotExists, "Key does not exist");
        }

        return *value;
    }

    template <typename K, typename V>
    const V& HashMap<K, V>::operator[](const K& key) const {
        V* value = GetOptional(key);

        if (value == nullptr) {
            F_ERROR_WITH_MESSAGE(HashMapKeyNotExists, "Key does not exist");
        }

        return *value;
    }

    template <typename K, typename V>
    V* HashMap<K, V>::GetOptional(const K& key) {
        Bucket& bucket = GetBucket(key);

        for (HashMapEntry<K, V>& entry : bucket.Values) {
            if (entry.Key == key) {
                return &entry.Value;
            }
        }

        return nullptr;
    }

    template <typename K, typename V>
    const V* HashMap<K, V>::GetOptional(const K& key) const {
        const Bucket& bucket = GetBucket(key);

        for (const HashMapEntry<K, V>& entry : bucket.Values) {
            if (entry.Key == key) {
                return &entry.Value;
            }
        }

        return nullptr;
    }

    template <typename K, typename V>
    V* HashMap<K, V>::Insert(K key, V value) {
        V* ptr = InsertIfNotExist(Move(key), Move(value));

        if (ptr == nullptr) {
            F_ERROR_WITH_MESSAGE(HashMapKeyAlreadyExists, "Key already exists");
        }

        return ptr;
    }

    template <typename K, typename V>
    V* HashMap<K, V>::InsertIfNotExist(K key, V value) {
        Bucket& bucket = GetBucket(key);

        for (HashMapEntry<K, V>& entry : bucket.Values) {
            if (entry.Key == key) {
                return nullptr;
            }
        }

        m_size++;
        auto& entry = bucket.Values.AppendInPlace(Move(key), Move(value));
        RecalculateBuckets();
        return &entry.Value;
    }

    template <typename K, typename V>
    V* HashMap<K, V>::InsertOrReplace(K key, V value) {
        Bucket& bucket = GetBucket(key);

        for (HashMapEntry<K, V>& entry : bucket.Values) {
            if (entry.Key == key) {
                entry.Value = Move(value);
                return &entry.Value;
            }
        }

        m_size++;
        auto& entry = bucket.Values.AppendInPlace(Move(key), Move(value));
        RecalculateBuckets();
        return &entry.Value;
    }

    template <typename K, typename V>
    bool HashMap<K, V>::Remove(const K& key) {
        Bucket& bucket = GetBucket(key);

        const size_t removed =
            Stdlib::RemoveIf(bucket.Values, [&](const HashMapEntry<K, V>& entry) { return entry.Key == key; });

        m_size -= removed;
        return removed > 0;
    }

    template <typename K, typename V>
    bool HashMap<K, V>::ContainsKey(const K& key) {
        Bucket& bucket = GetBucket(key);

        for (const HashMapEntry<K, V>& entry : bucket.Values) {
            if (entry.Key == key) {
                return true;
            }
        }

        return false;
    }

    template <typename K, typename V>
    bool HashMap<K, V>::ContainsValue(const V& value) {
        for (Bucket& bucket : m_buckets) {
            for (const HashMapEntry<K, V>& entry : bucket.Values) {
                if (entry.Value == value) {
                    return true;
                }
            }
        }

        return false;
    }

    template <typename K, typename V>
    void HashMap<K, V>::Clear() {
        for (Bucket& bucket : m_buckets) {
            bucket.Values.Clear();
        }

        m_size = 0;
    }

    template <typename K, typename V>
    typename HashMap<K, V>::Iterator HashMap<K, V>::Begin() noexcept {
        return Size() == 0 ? End() : HashMap::Iterator(*this, 0, 0);
    }

    template <typename K, typename V>
    typename HashMap<K, V>::Iterator HashMap<K, V>::End() noexcept {
        return HashMap::Iterator(*this, m_buckets.Size(), 0);
    }

    template <typename K, typename V>
    typename HashMap<K, V>::ConstIterator HashMap<K, V>::Begin() const noexcept {
        return Size() == 0 ? End() : HashMap::ConstIterator(*this, 0, 0);
    }

    template <typename K, typename V>
    typename HashMap<K, V>::ConstIterator HashMap<K, V>::End() const noexcept {
        return HashMap::ConstIterator(*this, m_buckets.Size(), 0);
    }

    template <typename K, typename V>
    void HashMap<K, V>::RecalculateBuckets() {
        if (m_size > m_buckets.Capacity() * m_loadFactor) {
            // double the bucket count
            m_buckets.EnsureCapacity(m_buckets.Capacity() * 2);
        }

        if (m_buckets.Size() >= m_buckets.Capacity()) {
            return;
        }

        const size_t previousCapacity = m_buckets.Size();

        while (m_buckets.Size() < m_buckets.Capacity()) {
            m_buckets.AppendInPlace();
        }

        // re-hash
        for (size_t currentBucket = 0; currentBucket < previousCapacity; currentBucket++) {
            Bucket& bucket = m_buckets[currentBucket];

            for (auto it = bucket.Values.Begin(); it != bucket.Values.End();) {
                const size_t computedBucket = Hash<K>{}((*it).Key) % m_buckets.Size();

                if (computedBucket != currentBucket) {
                    m_buckets[computedBucket].Values.Append(Move(*it));
                    it = bucket.Values.Erase(it);
                } else {
                    it++;
                }
            }
        }
    }

    template <typename K, typename V>
    typename HashMap<K, V>::Bucket& HashMap<K, V>::GetBucket(const K& key) {
        return m_buckets[Hash<K>{}(key) % m_buckets.Size()];
    }

    template <typename K, typename V>
    const typename HashMap<K, V>::Bucket& HashMap<K, V>::GetBucket(const K& key) const {
        return m_buckets[Hash<K>{}(key) % m_buckets.Size()];
    }

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASHMAP_TCC
