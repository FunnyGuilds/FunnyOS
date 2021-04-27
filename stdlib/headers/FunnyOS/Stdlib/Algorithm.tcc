#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_HPP
#error "Include Algorithm.hpp instead"
#endif
#ifndef FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC
#define FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC

namespace FunnyOS::Stdlib {

    template <typename T>
    constexpr T Min(T a, T b) noexcept(noexcept(a < b)) {
        return a < b ? a : b;
    }

    template <typename T>
    constexpr T Max(T a, T b) noexcept(noexcept(a > b)) {
        return a > b ? a : b;
    }

    template <typename T, typename... Args>
    [[nodiscard]] constexpr T Min(T value1, T value2, Args... args) noexcept(noexcept(Min(value1, value2))) {
        T min = Min(value1, value2);

        if (sizeof...(Args) > 0) {
            return Min(min, args...);
        }

        return min;
    }

    template <typename T, typename... Args>
    [[nodiscard]] constexpr T Max(T value1, T value2, Args... args) noexcept(noexcept(Max(value1, value2))) {
        T max = Max(value1, value2);

        if (sizeof...(Args) > 0) {
            return Max(max, args...);
        }

        return max;
    }

    namespace _Internal {
        template <typename Container, typename Iterator>
        struct DefaultEraser {
            constexpr DefaultEraser() = default;

            Iterator operator()(Container& container, Iterator iterator) {
                return container.Erase(iterator);
            }
        };
    }  // namespace _Internal

    template <typename Iterator, typename Matcher>
    Iterator Find(Iterator begin, Iterator end, Matcher matcher) {
        for (Iterator current = begin; current != end; current++) {
            if (matcher(*current)) {
                return current;
            }
        }

        return end;
    }

    template <typename Container, typename Iterator, typename Matcher>
    Iterator Find(const Container& container, Matcher matcher) {
        return Find<Iterator, Matcher>(Begin(container), End(container), matcher);
    }

    template <typename Iterator, typename Matcher, typename Eraser>
    size_t RemoveIf(Iterator begin, Iterator end, Matcher matcher, Eraser eraser) {
        size_t erased = 0;

        for (Iterator current = begin; current != end;) {
            if (matcher(*current)) {
                current = eraser(current);
                erased++;
            } else {
                current++;
            }
        }

        return erased;
    }

    template <typename Container, typename Iterator, typename Matcher, typename ContainerEraser>
    size_t RemoveIf(Container& container, Matcher matcher) {
        static ContainerEraser c_eraser;

        size_t erased = 0;

        for (Iterator current = Begin(container); current != End(container);) {
            if (matcher(*current)) {
                current = c_eraser(container, current);
                erased++;
            } else {
                current++;
            }
        }

        return erased;
    }
}  // namespace FunnyOS::Stdlib

#endif  // FUNNYOS_STDLIB_HEADERS_FUNNYOS_STDLIB_ALGORITHM_TCC
