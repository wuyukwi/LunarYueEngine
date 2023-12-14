#include <iostream>
#include <span>
#include <vector>

int main()
{
    // C++20 range-based for loop
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    for (int number : numbers)
    {
        std::cout << number << " ";
    }
    std::cout << std::endl;

    // C++20 std::span
    int            arr[] = {10, 20, 30, 40, 50};
    std::span<int> spanArr(arr, 5);

    // Print elements using range-based for loop with std::span
    for (int element : spanArr)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;
#if __has_cpp_attribute(is_constant_evaluated) || defined(__cpp_lib_is_constant_evaluated)
    std::cout << "C++20 supported!" << std::endl;
#else
    std::cout << "C++20 not supported!" << std::endl;
#endif

    std::cout << __cplusplus << std::endl;
    return 0;
}