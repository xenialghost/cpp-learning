#include <ostream>

#include "operators_examples.h"

#include <vector>

namespace operators_examples {
    auto pointer_decay(const int* input_ptr) -> const int* {
        return input_ptr;
    }

    class Empty {};

    class Big {
    public:
        int a = 0;
        int b = 0;
        char c = 0;
        virtual ~Big() = default;
        virtual auto test() -> int = 0;
    };

    template <typename... Values>
    class VariadicClass final {
        public:
        static constexpr std::size_t size = sizeof...(Values);
        std::tuple<Values...> data;

        constexpr explicit VariadicClass(Values... args) : data(args...) {}
    };

    auto run_sizeof_operator() -> void {
        constexpr std::size_t example_length = 4;
        int raw_array[example_length];
        static_assert(sizeof(raw_array) == sizeof(int) * example_length);
        static_assert(sizeof(pointer_decay(raw_array)) == sizeof(void*));

        std::vector<int> vec(example_length);
        static_assert(sizeof(&vec) == sizeof(void*));

        static_assert(sizeof(Empty) == 1);
        constexpr std::size_t big_class_raw_size = sizeof(int) * 2 + sizeof(char) + sizeof(void*);
        constexpr std::size_t big_class_padded_size = (alignof(void*) + big_class_raw_size - 1) & ~(alignof(void*) - 1);

        static_assert(sizeof(Big) == big_class_padded_size); // class members + vptr padded
        static_assert(alignof(Big) == sizeof(void*));

        VariadicClass obj(10, 3.14, "Hello", 'a');
        static_assert(VariadicClass<int, double, const char *, char>::size == 4);
        static_assert(obj.size == 4);
        static_assert(sizeof(VariadicClass<int>) == sizeof(int));
        static_assert(sizeof(VariadicClass<>) == 1);
        static_assert(sizeof(VariadicClass<int, double, const char *, char>::size) == sizeof(void*));
        constexpr std::size_t variadic_class_raw_size = sizeof(int) + sizeof(double) + sizeof(char) + sizeof(void*) + sizeof(VariadicClass<int, double, const char *, char>::size);
        constexpr std::size_t variadic_class_padded_size = (alignof(void*) + variadic_class_raw_size - 1) & ~(alignof(void*) - 1);
        static_assert(sizeof(obj) == variadic_class_padded_size);
    }
} // namespace operators_examples
