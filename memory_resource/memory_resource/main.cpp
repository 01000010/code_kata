#include <iostream>
#include "logging_resource.hpp"

int main()
{
    static logging_resource memory_resource{ std::pmr::new_delete_resource() };

    std::pmr::set_default_resource(&memory_resource);
    std::cout << "vector<int> testing...\n";

    std::pmr::vector<int> ints;

    ints.push_back(42);
    ints.push_back(1729);

    return 0;
}