#ifndef CHANGING_CLASSES_HPP
#define CHANGING_CLASSES_HPP

#include <cassert>
#include <iostream>

class God
{
private:
    int a;
    int b;
    int c;
    int d;

public:
    God() noexcept {}
    God(int a, int b, int c, int d) noexcept : a{ a }, b{ b }, c{ c }, d{ d } {}
    virtual ~God() noexcept {}

    void print_data() noexcept
    {
        std::cout << "Data: " << a << ", " << b << ", " << c << ", " << d << '\n';
    }

    virtual void who_am_i() noexcept
    {
        std::cout << "I'm a generic god!\n";
        print_data();
    }

    void* operator new(size_t size, void* ptr)
    {
        return static_cast<void*>(ptr);

        // a happy compiler
        size;
    }

    void operator delete(void* address, void* ptr)
    {
        assert(0);

        // a happy compiler
        address;
        ptr;
    }
};

class Brahma : public God
{
public:
    Brahma() noexcept {}

    void who_am_i() noexcept override
    {
        std::cout << "I am Brahma!\n";
        print_data();
    }

    static void change(God* god)
    {
        new(god)Brahma{};
    }
};

class Rama : public God
{
public:
    Rama() noexcept {}

    void who_am_i() noexcept override
    {
        std::cout << "I am Rama!\n";
        print_data();
    }

    static void change(God* god)
    {
        new(god)Rama{};
    }
};

class Vishnu : public God
{
public:
    Vishnu() noexcept {}

    void who_am_i() noexcept override
    {
        std::cout << "Now I am become Death, the destroyer of worlds!\n";
        print_data();
    }

    static void change(God* god)
    {
        new(god)Vishnu{};
    }
};

int run()
{
    God* god{ ::new God(1, 2, 3, 4) };

    god->who_am_i();
    Brahma::change(god);
    god->who_am_i();
    Rama::change(god);
    god->who_am_i();
    Vishnu::change(god);
    god->who_am_i();

    ::delete god;

    return 0;
}

#endif