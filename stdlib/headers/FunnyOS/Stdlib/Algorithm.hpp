#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP

namespace FunnyOS::Stdlib {

    /**
     * Returns a lower value from the two supplied.
     *
     * @tparam T type of the values
     * @param[in] a first value to test
     * @param[in] b second value to test
     * @return lowest of those values
     */
    template <typename T>
    [[nodiscard]] constexpr T Min(T a, T b) noexcept(noexcept(a < b));

    /**
     * Returns a higher value from the two supplied.
     *
     * @tparam T type of the values
     * @param[in] a first value to test
     * @param[in] b second value to test
     * @return higher of those values
     */
    template <typename T>
    [[nodiscard]] constexpr T Max(T a, T b) noexcept(noexcept(a > b));

    /**
     * Returns the lowest value from all of the supplied.
     *
     * @tparam T type of the values
     * @tparam Args type of the values
     * @param value1 first value to compare
     * @param value2 second value to compare
     * @param args rest of the values to compare
     * @return the lowest value
     */
    template <typename T, typename... Args>
    [[nodiscard]] constexpr T Min(T value1, T value2, Args... args) noexcept(noexcept(Min(value1, value2)));

    /**
     * Returns the highest value from all of the supplied.
     *
     * @tparam T type of the values
     * @tparam Args type of the values
     * @param value1 first value to compare
     * @param value2 second value to compare
     * @param args rest of the values to compare
     * @return the highest value
     */
    template <typename T, typename... Args>
    [[nodiscard]] constexpr T Max(T value1, T value2, Args... args) noexcept(noexcept(Max(value1, value2)));

    namespace _Internal {
        template <typename T, typename Iterator>
        struct DefaultEraser;
    }

    /**
     * Finds the first element between [begin] (inclusive) and [end] (exclusive) that matches the predicate [matcher].
     * If nothing matches [end] is returned.
     *
     * @tparam Iterator type of the iterators
     * @tparam Matcher type of the matcher
     * @param begin iterator pointing to the start of the range (inclusive).
     * @param end iterator pointing to the end of the range (exclusive).
     * @param matcher matcher to test elements
     * @return iterator pointing at the matched element, or [end] if nothing matched]
     */
    template <typename Iterator, typename Matcher>
    Iterator Find(Iterator begin, Iterator end, Matcher matcher);

    /**
     * Finds the first element in the given container that matches the predicate [matcher].
     * If nothing matches [container.End()] is returned.
     *
     * @tparam Container type of the container
     * @tparam Iterator type of the iterators used by the Container
     * @tparam Matcher type of the matcher
     * @param container container to search in
     * @param matcher matcher to test elements
     * @return iterator pointing at the matched element, or [container.End()] if nothing matched]
     */
    template <typename Container, typename Iterator = typename Container::ConstIterator, typename Matcher>
    Iterator Find(const Container& container, Matcher matcher);

    /**
     * Removes every element between [begin] (inclusive) and [end] (exclusive) that matches the predicate [matcher].
     *
     * @tparam Iterator type of the iterators
     * @tparam Matcher type of the matcher
     * @tparam Eraser type of the eraser
     * @param begin iterator pointing to the start of the range (inclusive).
     * @param end iterator pointing to the end of the range (exclusive).
     * @param matcher matcher to test elements
     * @param eraser eraser, functor used to erase elements (see _Internal::DefaultEraser for example)
     */
    template <typename Iterator, typename Matcher, typename Eraser>
    void RemoveIf(Iterator begin, Iterator end, Matcher matcher, Eraser eraser);

    /**
     * Removes every element on the given container that matches the predicate [matcher].
     *
     * @tparam Container type of the container
     * @tparam Iterator type of the iterators used by the Container
     * @tparam Matcher type of the matcher
     * @tparam ContainerEraser type of the eraser
     * @param container container to remove the elements from
     * @param matcher matcher to test elements
     */
    template <typename Container, typename Iterator = typename Container::ConstIterator, typename Matcher,
              typename ContainerEraser = _Internal::DefaultEraser<Container, Iterator>>
    void RemoveIf(Container& container, Matcher matcher);

}  // namespace FunnyOS::Stdlib

#include "Algorithm.tcc"
#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
