#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LINKEDLIST_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LINKEDLIST_TCC
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LINKEDLIST_HPP
#error "Include LinkedList.hpp instaed"
#endif

#include "Memory.hpp"
#include "New.hpp"
#include "Functional.hpp"
#include "LinkedList.hpp"
#include "String.hpp"

namespace FunnyOS::Stdlib {

    template <typename T>
    typename LinkedList<T>::ConstIterator LinkedList<T>::ConstIterator::operator++() noexcept {
        const auto snapshot = *this;
        m_element = m_element->Next;
        return snapshot;
    }

    template <typename T>
    typename LinkedList<T>::ConstIterator LinkedList<T>::ConstIterator::operator++(int) noexcept {
        const auto snapshot = *this;
        m_element = m_element->Next;
        return snapshot;
    }

    template <typename T>
    const T& LinkedList<T>::ConstIterator::operator*() const noexcept {
        return m_element->Data.GetObject();
    }

    template <typename T>
    const T* LinkedList<T>::ConstIterator::operator->() const noexcept {
        return m_element->Data.GetObject();
    }

    template <typename T>
    bool LinkedList<T>::ConstIterator::operator==(const LinkedList::ConstIterator& other) noexcept {
        return m_element == other.m_element;
    }

    template <typename T>
    bool LinkedList<T>::ConstIterator::operator!=(const LinkedList::ConstIterator& other) noexcept {
        return m_element != other.m_element;
    }

    template <typename T>
    const LinkedList<T>& LinkedList<T>::ConstIterator::GetList() const {
        return m_list;
    }

    template <typename T>
    typename LinkedList<T>::Element* LinkedList<T>::ConstIterator::GetElement() const {
        return m_element;
    }

    template <typename T>
    LinkedList<T>::ConstIterator::ConstIterator(const LinkedList& list, LinkedList::Element* element)
        : m_list(list), m_element(element) {}

    template <typename T>
    T& LinkedList<T>::Iterator::operator*() noexcept {
        return this->m_element->Data.GetObject();
    }

    template <typename T>
    T* LinkedList<T>::Iterator::operator->() noexcept {
        return this->m_element->Data.GetObject();
    }

    template <typename T>
    LinkedList<T>::LinkedList(const LinkedList& other) : LinkedList() {
        for (auto const& ref : other) {
            Append(ref);
        }
    }

    template <typename T>
    LinkedList<T>& LinkedList<T>::operator=(const LinkedList& other) {
        if (&other == this) {
            return *this;
        }

        Clear();

        for (auto const& ref : other) {
            Append(ref);
        }

        return *this;
    }

    template <typename T>
    LinkedList<T>::LinkedList(LinkedList&& other) noexcept
        : m_size(other.m_size), m_head(other.m_head), m_tail(other.m_tail) {
        other.m_head = nullptr;
        other.m_tail = nullptr;
    }

    template <typename T>
    LinkedList<T>& LinkedList<T>::operator=(LinkedList&& other) noexcept {
        Clear();
        delete m_head;

        m_size = other.m_size;
        m_tail = other.m_tail;
        m_head = other.m_head;

        other.m_head = nullptr;
        other.m_tail = nullptr;

        return *this;
    }

    template <typename T>
    LinkedList<T>::Iterator::Iterator(LinkedList& list, LinkedList::Element* element) : ConstIterator(list, element) {}

    template <typename T>
    LinkedList<T>::LinkedList() : m_head(new Element{Storage<T>(), nullptr}), m_tail(m_head) {}

    template <typename T>
    LinkedList<T>::LinkedList(InitializerList<T> list) : LinkedList() {
        for (const T& ref : list) {
            Append(ref);
        }
    }

    template <typename T>
    LinkedList<T>::~LinkedList() {
        Clear();
        delete m_head;
    }

    template <typename T>
    size_t LinkedList<T>::Size() const noexcept {
        return m_size;
    }

    template <typename T>
    T& LinkedList<T>::operator[](size_t index) {
        CheckBounds(index);
        return (ElementAt(index)->Data).GetObject();
    }

    template <typename T>
    const T& LinkedList<T>::operator[](size_t index) const {
        CheckBounds(index);
        return (ElementAt(index)->Data).GetObject();
    }

    template <typename T>
    T& LinkedList<T>::Head() {
        CheckBounds(0);
        return m_head->Data.GetObject();
    }

    template <typename T>
    T& LinkedList<T>::Tail() {
        CheckBounds(0);
        return m_tail->Data.GetObject();
    }

    template <typename T>
    const T& LinkedList<T>::Head() const {
        CheckBounds(0);
        return m_head->Data.GetObject();
    }

    template <typename T>
    const T& LinkedList<T>::Tail() const {
        CheckBounds(0);
        return m_tail->Data.GetObject();
    }

    template <typename T>
    void LinkedList<T>::Append(const T& value) {
        Insert(m_size, value);
    }

    template <typename T>
    void LinkedList<T>::Append(T&& value) {
        Insert(m_size, Forward<T>(value));
    }

    template <typename T>
    template <typename... Args>
    void LinkedList<T>::AppendInPlace(Args&&... args) {
        InsertInPlace(m_size, Forward<Args>(args)...);
    }

    template <typename T>
    void LinkedList<T>::Remove(size_t index) {
        CheckBounds(index);

        Element* before;
        Element* toRemove = ElementAt(index, &before);
        before->Next = toRemove->Next;
        if (toRemove == m_tail) {
            m_tail = before;
        }

        delete toRemove;
        m_size--;
    }

    template <typename T>
    void LinkedList<T>::RemoveRange(size_t from, size_t to) {
        CheckBounds(from);
        CheckBounds(to);

        if (from > to) {
            throw LinkedListIndexOutOfBounds("from > to");
        }

        Element* before;
        Element* current = ElementAt(from, &before);
        Element* toRemoveLast = ElementAt(to);

        before->Next = toRemoveLast->Next;
        if (toRemoveLast == m_tail) {
            m_tail = before;
        }

        for (;;) {
            Element* next = current->Next;
            delete current;

            m_size--;
            if (current == toRemoveLast) {
                break;
            }

            current = next;
        }
    }

    template <typename T>
    void LinkedList<T>::Insert(size_t index, const T& value) {
        InsertElement(index, new Element{Storage<T>(value), nullptr});
    }

    template <typename T>
    void LinkedList<T>::Insert(size_t index, T&& value) {
        InsertElement(index, new Element{Storage<T>(value), nullptr});
    }

    template <typename T>
    template <typename... Args>
    void LinkedList<T>::InsertInPlace(size_t index, Args&&... args) {
        InsertElement(index, new Element{Storage<T>(InPlaceConstructor::Value, Forward<Args>(args)...), nullptr});
    }

    template <typename T>
    template <typename OtherIterator>
    void LinkedList<T>::Insert(size_t index, OtherIterator value, size_t size) {
        if (size == 0) {
            return;
        }

        if (index == m_size) {
            for (size_t i = 0; i < size; i++) {
                m_tail->Next = new Element{Storage<T>(*value), nullptr};
                m_tail = m_tail->Next;
                m_size++;
                value++;
            }

            return;
        }

        Element* before;
        Element* insertAfter = ElementAt(index, &before);

        Element* current = before;
        for (size_t i = 0; i < size; i++) {
            current->Next = new Element{Storage<T>(*value), nullptr};
            current = current->Next;

            m_size++;
            value++;
        }

        current->Next = insertAfter;
    }

    template <typename T>
    void LinkedList<T>::Clear() {
        Element* element = m_head->Next;

        while (element != nullptr) {
            Element* next = element->Next;
            delete element;
            element = next;
        }

        m_head->Next = nullptr;
        m_size = 0;
    }

    template <typename T>
    typename LinkedList<T>::Iterator LinkedList<T>::Begin() noexcept {
        return Iterator(*this, m_head->Next);
    }

    template <typename T>
    typename LinkedList<T>::Iterator LinkedList<T>::End() noexcept {
        return Iterator(*this, nullptr);
    }

    template <typename T>
    typename LinkedList<T>::ConstIterator LinkedList<T>::Begin() const noexcept {
        return ConstIterator(*this, m_head->Next);
    }

    template <typename T>
    typename LinkedList<T>::ConstIterator LinkedList<T>::End() const noexcept {
        return ConstIterator(*this, nullptr);
    }

    template <typename T>
    void LinkedList<T>::CheckBounds(size_t num) const {
        if (num >= m_size) {
            String::StringBuffer errorBuffer = Memory::AllocateBuffer<char>(32);
            String::Format(errorBuffer, "%zu > %zu", num, m_size);
            throw LinkedListIndexOutOfBounds(errorBuffer.Data);
        }
    }

    template <typename T>
    typename LinkedList<T>::Element* LinkedList<T>::ElementAt(size_t index, Element** preceding) const {
        Element* current = m_head;

        for (size_t i = 0; i < index + 1; i++) {
            *preceding = current;
            current = current->Next;
        }

        return current;
    }

    template <typename T>
    typename LinkedList<T>::Element* LinkedList<T>::ElementAt(size_t index) const {
        Element* ignored;
        return ElementAt(index, &ignored);
    }

    template <typename T>
    void LinkedList<T>::InsertElement(size_t index, Element* element) {
        if (index == m_size) {
            m_tail->Next = element;
            m_tail = element;
            m_size++;
            return;
        }

        Element* before;
        Element* current = ElementAt(index, &before);
        before->Next = element;
        element->Next = current;
        m_size++;
    }

}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_LINKEDLIST_TCC
