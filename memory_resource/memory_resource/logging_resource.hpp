#ifndef LOGGING_RESOURCE_HPP
#define LOGGING_RESOURCE_HPP

#include <memory_resource>
#include <iostream>

class logging_resource : public std::pmr::memory_resource
{
private:
    std::pmr::memory_resource* m_underlying_resource;

    void* do_allocate(size_t bytes, size_t align) override
    {
        std::cout << "Allocating " << bytes << " bytes\n";

        return m_underlying_resource->allocate(bytes, align);
    }

    void do_deallocate(void* ptr, size_t bytes, size_t align) override
    {
        return m_underlying_resource->deallocate(ptr, bytes, align);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        return m_underlying_resource->is_equal(other);
    }
    
public:
    logging_resource(std::pmr::memory_resource* underlying_resource)
        : m_underlying_resource{ underlying_resource } {}
};

#endif