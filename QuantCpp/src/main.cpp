
#include <iostream>
#include <ranges>

int main() {
    // C++23 ranges 예제
    for (int i : std::views::iota(1, 10)) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
   
    return 0;
}