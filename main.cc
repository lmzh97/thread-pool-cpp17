#include <iostream>

#include "ThreadPool.h"

int main() {
    auto pool = ThreadPool(4);
    auto add_func = [](int a, int b) { return a + b; };
    auto ret = pool.Enqueue(add_func, 1, 2);
    if (ret.valid()) {
        std::cout << "1 + 2 = " << ret.get() << std::endl;
    } else {
        std::cout << "Invalid" << std::endl;
    }
    return 0;
}
