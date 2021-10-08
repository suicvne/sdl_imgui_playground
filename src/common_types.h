#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__

#include <iostream>

// Declares a type called "_fn_ptr" that is a function pointer (returning void, no args)
typedef void (*_fn_ptr)(); 
template<typename T, typename R>
_fn_ptr void_cast(R(T::*f)())
{
    // By utilizing a union here, we get low-level
    // enough to allow Clang to cast our member function 
    // to our _fn_ptr type
    union
    {
        R(T::*pf)();
        _fn_ptr p;
    };

    pf = f;
    return p;
}

#define CLAMP(a, min, max) ((a) < (min) ? (min) : (a) > (max) ? (max) : (a))

#define _xstr(s) str(s)
#define str(s) #s

struct Color
{
    uint8_t red, green, blue, alpha;

    constexpr Color(const uint8_t &r, const uint8_t &g, const uint8_t &b, const uint8_t &a)
        : red(r), green(g), blue(b), alpha(a)
    {
    }
    constexpr Color() : red(0), green(0), blue(0), alpha(255)
    {
    }
};

static inline void _print_color(const struct Color &col)
{
    std::cout << "Color(" << col.red << ", " << col.green << ", " << col.blue << ", " << col.alpha << ")" << std::endl;
}

template<typename T>
struct timeinfo_t
{
    T delay_time;
    T frame_time;

    timeinfo_t(T& dt, T& ft)
    : delay_time(dt), frame_time(ft)
    {}

    timeinfo_t() : delay_time(0), frame_time(0)
    {}

    void set(T& dt, T& ft)
    {
        delay_time = dt;
        frame_time = ft;   
    }
};

#endif //__COMMON_TYPES_H__