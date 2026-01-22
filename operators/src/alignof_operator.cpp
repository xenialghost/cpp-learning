#include <iostream>
#include <memory>
#include <ostream>

#include "operators_examples.h"

#include <vector>

namespace operators_examples {

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
        std::tuple<Values...> data;
        static constexpr std::size_t align = alignof(std::tuple<Values...>);


        constexpr explicit VariadicClass(Values... args) : data(args...) {}
    };

    class alignas(32) SIMD256 {
        float positions[8] = {};
    };

    class CPUCacheLine {
        /*alignas(64)*/ alignas(std::hardware_destructive_interference_size) std::atomic<int> countA;
        /*alignas(64)*/ alignas(std::hardware_destructive_interference_size) std::atomic<int> countB;
    };

    class alignas(4096) MemoryPageBasedDMABuffer {
        char data[4096] = {};
    };

    auto run_alignof_operator() -> void {
        constexpr std::size_t example_length = 4;
        int raw_array[example_length];
        static_assert(alignof(int) == sizeof(int));
        static_assert(alignof(int[]) == sizeof(int));
        static_assert(alignof(int[example_length]) == sizeof(int));

        for (std::size_t i = 0; i < example_length; ++i){
            std::println("alignof example: int array elements address [{}] = {}", i, reinterpret_cast<std::size_t>(raw_array + i));
        }

        static_assert(alignof(std::vector<int>) == sizeof(void*));

        static_assert(alignof(Empty) == 1);

        static_assert(alignof(Big) == sizeof(void*));

        VariadicClass obj(10, 3.14, "Hello", 'a');
        static_assert(obj.align == sizeof(void*));
        static_assert(alignof(VariadicClass<int>) == sizeof(int));
        static_assert(alignof(VariadicClass<>) == 1);
        static_assert(alignof(VariadicClass<int, double, const char *, char>) == sizeof(void*));

        static_assert(alignof(SIMD256) == 32);
        static_assert(alignof(CPUCacheLine) == 64);
        static_assert(alignof(MemoryPageBasedDMABuffer) == 4096);
        static_assert(sizeof(MemoryPageBasedDMABuffer) == 4096);

        //-----------

        // malloc/new default alignment
        static_assert(alignof(std::max_align_t) == sizeof(void*));
        void* ptr1 = malloc(1); // alignment 8
        void* ptr2 = aligned_alloc(4096, 1); // alignment 4096

        Empty *empty_placement = new (ptr1) Empty;
        Empty *empty_placement_4096 = new (ptr2) Empty;

        delete empty_placement;
        delete empty_placement_4096;

        //-----------

        /* C-Style (C11 and newer): _Alignof(char) */

        //-----------

        char buffer[100];
        void* ptr = &buffer[1];
        size_t space = 99;

        size_t need_size = sizeof(double);
        size_t need_align = alignof(double);

        std::cout << "alignof example: Pointer address: " << ptr << "\n";
        if (std::align(need_align, need_size, ptr, space)) {
            std::cout << "alignof example: Pointer aligned. New address: " << ptr << "\n";

            double* d_ptr = new (ptr) double(3.14);
        } else {
            std::cout << "Not enough space to align!\n";
        }

        //-----------

        // Modern std::aligned_storage replacement
        alignas(Empty) char storage[sizeof(Empty)]; // stack allocation
        Empty* ptr5 = new (&storage) Empty;

        /**
        template <typename T>
        class Wrapper {
            alignas(T) unsigned char storage[sizeof(T)]; or C++17 alignas(T) std::byte storage[sizeof(T)];
        };
         */
    }
} // namespace operators_examples

// 1. Реализация Wrapper
//
//  #include <iostream>
//  #include <new> // Обязательно для placement new
//
//  template <typename T>
//  class Wrapper {
// private:
//     // 1. Место под объект. Используем std::byte (C++17) или unsigned char
//     alignas(T) std::byte storage[sizeof(T)];
//     bool initialized = false;
//
// public:
//     // 2. Конструктор (инициализируем объект в нашем буфере)
//     template <typename... Args>
//     void construct(Args&&... args) {
//         if (initialized) return;
//
//         // Магия Placement New: new (адрес) Тип(аргументы)
//         new (storage) T(std::forward<Args>(args)...);
//         initialized = true;
//     }
//
//     // 3. Доступ к объекту (кастим буфер к нужному типу)
//     T& get() {
//         return *reinterpret_cast<T*>(storage);
//     }
//
//     // 4. Деструктор (ВАЖНО!)
//     // Так как мы создали объект вручную через placement new,
//     // мы ОБЯЗАНЫ вызвать деструктор вручную.
//     ~Wrapper() {
//         if (initialized) {
//             get().~T(); // Ручной вызов деструктора
//         }
//     }
// };
//
// 2. Пример использования
//
//  struct MyData {
//     int id;
//     MyData(int i) : id(i) { std::cout << "Born: " << id << "\n"; }
//     ~MyData() { std::cout << "Died: " << id << "\n"; }
// };
//
// int main() {
//     {
//         // Wrapper на стеке
//         Wrapper<MyData> w;
//
//         // В этот момент память под MyData есть, но конструктор не вызывался
//
//         w.construct(42); // Теперь объект создан
//
//         std::cout << "Data ID: " << w.get().id << "\n";
//
//     } // Конец скоупа: вызовется ~Wrapper, который внутри вызовет ~MyData
// }
