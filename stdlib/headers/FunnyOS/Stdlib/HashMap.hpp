#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASHMAP_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASHMAP_HPP

#include "Functional.hpp"
#include "Hash.hpp"
#include "LinkedList.hpp"
#include "Vector.hpp"

namespace FunnyOS::Stdlib {

    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(HashMapKeyNotExists);
    F_TRIVIAL_EXCEPTION_WITH_MESSAGE(HashMapKeyAlreadyExists);

    template <typename K, typename V>
    struct HashMapEntry {
        K Key;
        V Value;

        HashMapEntry(K key, V value);
    };

    template <typename K, typename V>
    class HashMap {
       public:
#ifdef F_LL
        using load_factor_t = size_t;

        /**
         * The default load factor used by HashMap.
         */
        constexpr const static load_factor_t DEFAULT_LOAD_FACTOR = 1;

#else
        using load_factor_t = float;

        /**
         * The default load factor used by HashMap.
         */
        constexpr const static load_factor_t DEFAULT_LOAD_FACTOR = 0.75F;
#endif

        /**
         * The default inital capacity used by HashMap.
         */
        constexpr const static size_t DEFAULT_INITIAL_CAPACITY = 16;

        /**
         * A const type used to iterate over this hashmap
         */
        class ConstIterator {
           public:
            TRIVIALLY_COPYABLE(ConstIterator);
            TRIVIALLY_MOVEABLE(ConstIterator);

            /**
             * Increment this iterator by one.
             *
             * @return *this
             */
            ConstIterator operator++() noexcept;

            /**
             * Increment this iterator by one.
             *
             * @return *this
             */
            ConstIterator operator++(int) noexcept;

            /**
             * Destructs the iterator, this has no effect on the list.
             */
            ~ConstIterator() = default;

            /**
             * Gets the value that this iterator is pointing at the moment.
             *
             * @return value that this iterator is pointing at the moment.
             */
            const HashMapEntry<K, V>& operator*() const noexcept;

            /**
             * Gets a pointer to the value that this iterator is pointing at the moment.
             *
             * @return a pointer to the value that this iterator is pointing at the moment.
             */
            const HashMapEntry<K, V>* operator->() const noexcept;

            /**
             * Checks whether or not the [other] iterator is pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is pointing at the same element
             */
            bool operator==(const ConstIterator& other) const noexcept;

            /**
             * Checks whether or not the [other] iterator is not pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is not pointing at the same element
             */
            bool operator!=(const ConstIterator& other) const noexcept;

            /**
             * Checks whether or not the [other] iterator is pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is pointing at the same element
             */
            bool operator==(ConstIterator& other) noexcept;

            /**
             * Checks whether or not the [other] iterator is not pointing at the exact same element as this one.
             *
             * @param other other iterator to compare
             * @return whether or not the other iterator is not pointing at the same element
             */
            bool operator!=(ConstIterator& other) noexcept;

           protected:
            const HashMap& m_map;
            size_t m_bucketIndex;
            size_t m_keyIndex;

            ConstIterator(const HashMap& map, size_t bucketIndex, size_t keyIndex);

            void SkipInvalid();

            friend class HashMap;
        };

        /**
         * Type used to iterate over this list.
         */
        class Iterator : public ConstIterator {
           public:
            TRIVIALLY_COPYABLE(Iterator);
            TRIVIALLY_MOVEABLE(Iterator);

            /**
             * Destructs the iterator, this has no effect on the list.
             */
            ~Iterator() = default;

            /**
             * Gets the value that this iterator is pointing at the moment.
             *
             * @return value that this iterator is pointing at the moment.
             */
            HashMapEntry<K, V>& operator*() noexcept;

            /**
             * Gets a pointer to the value that this iterator is pointing at the moment.
             *
             * @return a pointer to the value that this iterator is pointing at the moment.
             */
            HashMapEntry<K, V>* operator->() noexcept;

           private:
            Iterator(HashMap& map, size_t bucketIndex, size_t keyIndex);

            friend class HashMap;
        };

       public:
        NON_COPYABLE(HashMap);
        TRIVIALLY_MOVEABLE(HashMap);

        /**
         * Constructs a new empty HashMap with the default load factor
         */
        HashMap();

        /**
         * Constructs a new empty HashMap with the given load factor
         */
        HashMap(load_factor_t loadFactor);

        /**
         * Constructs a new empty HashMap with the given initial capacity and load factor
         */
        HashMap(size_t initialCapacity, load_factor_t loadFactor);

        /**
         * Gets the size of the HashMap, the actual number of elements inside it.
         *
         * @return size of the hashmap
         */
        [[nodiscard]] size_t Size() const noexcept;

        /**
         * Gets the element associated with the given key
         *
         * @param index index
         * @return the element
         * @throws HashMapKeyNotExists if the key does not exist
         */
        [[nodiscard]] V& operator[](const K& key);

        /**
         * Gets the element associated with the given key
         *
         * @param index index
         * @return the element
         * @throws HashMapKeyNotExists if the key does not exist
         */
        [[nodiscard]] const V& operator[](const K& key) const;

        /**
         * Gets the element associated with the given key
         *
         * @param index index
         * @return the element or nullptr if the key does not exist
         */
        [[nodiscard]] V* GetOptional(const K& key);

        /**
         * Gets the element associated with the given key
         *
         * @param index index
         * @return the element or nullptr if the key does not exist
         */
        [[nodiscard]] const V* GetOptional(const K& key) const;

        /**
         * Inserts a value at the given key.
         *
         * @param key key to insert the element at
         * @param value value to append
         * @throws HashMapKeyAlreadyExists if the key already exists
         *
         * @return pointer to the inserted value
         */
        V* Insert(K key, V value);

        /**
         * Inserts a value at the given key if the key doesn't exist already.
         *
         * @param key key to insert the element at
         * @param value value to append
         *
         * @return pointer to the inserted value, or nullptr if nothing was inserted
         */
        V* InsertIfNotExist(K key, V value);

        /**
         * Inserts a value at the given key if the key and if the key already has value, replaces that value.
         *
         * @param key key to insert the element at
         * @param value value to append
         *
         * @return pointer to the inserted, or replaced value
         */
        V* InsertOrReplace(K key, V value);

        /**
         * Removes an element associated with the specified key.
         *
         * @return true if the element was removed, false if there was no element associated with the given key
         */
        bool Remove(const K& key);

        /**
         * Checks if the given key exists in the hashmap.
         *
         * @return true if there is an element associated with the given key, false if there is none
         */
        bool ContainsKey(const K& key);

        /**
         * Checks if the given value exists in the hashmap.
         *
         * @return true if there is an element associated with the given value, false if there is none
         */
        bool ContainsValue(const V& value);

        /**
         * Removes all elements from this hashmap.
         */
        void Clear();

        /**
         * Returns an iterator pointing at the beginning of the map.
         *
         * @return iterator pointing at the beginning of the map.
         */
        Iterator Begin() noexcept;

        /**
         * Returns an iterator pointing at the end of the map. (The element 1 after the last element of the map)
         *
         * @return iterator pointing at the end of the map.
         */
        Iterator End() noexcept;

        /**
         * Returns an iterator pointing at the beginning of the map.
         *
         * @return iterator pointing at the beginning of the map.
         */
        ConstIterator Begin() const noexcept;

        /**
         * Returns an iterator pointing at the end of the map. (The element 1 after the last element of the map)
         *
         * @return iterator pointing at the end of the map.
         */
        ConstIterator End() const noexcept;

        HAS_STANDARD_ITERATORS;

       private:
        struct Bucket {
            LinkedList<HashMapEntry<K, V>> Values;
        };

        const load_factor_t m_loadFactor;
        Vector<Bucket> m_buckets;
        size_t m_size;

       private:
        void RecalculateBuckets();

        Bucket& GetBucket(const K& key);

        const Bucket& GetBucket(const K& key) const;
    };

}  // namespace FunnyOS::Stdlib

#include "HashMap.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_HASHMAP_HPP
