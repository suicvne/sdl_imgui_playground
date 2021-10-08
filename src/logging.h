#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <iostream>


#ifdef VITA

#ifndef DBG_IP
#define DBG_IP "192.168.0.45"
#endif

#ifndef DBG_PORT
#define DBG_PORT 18194
#endif

#define PRINTF(args...) debugNetPrintf(DEBUG, (args))
#define PRINTF_INIT debugNetInit(DBG_IP, DBG_PORT, DEBUG)
#define PRINTF_FINISH debugNetFinish()

#else // All Other Platforms

#include <cstdio>

#define PRINTF(args...) printf(args)
#define PRINTF_INIT 
#define PRINTF_FINISH

#endif

// I'm lazy
#define _FR_OPOV friend CCout& operator<<
#define _OPOV CCout& operator<<

struct CCout
{
    CCout()
    {
        PRINTF_INIT;
    }

    ~CCout()
    {
        PRINTF_FINISH;
    }

    _FR_OPOV(CCout& out, const int &msg);
    _FR_OPOV(CCout& out, const std::string &msg);
    _FR_OPOV(CCout& out, const char* input);
};

namespace ms {
    CCout cout;
};

_OPOV(CCout& out, const int &msg)
{
    PRINTF("%d", msg);
    return out;
}


_OPOV(CCout& out, const std::string &msg)
{
    PRINTF("%s", msg.c_str());
    return out;
}

_OPOV(CCout& out, const char *input)
{
    PRINTF("%s", input);
    return out;
}

// Hey, I undefined it after.
#undef _OPOV
#undef _FR_OPOV

#endif // __LOGGING_H__