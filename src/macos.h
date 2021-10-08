/**
 * 
 * A collection of useful macOS Specific Functions.
 * Enables special macOS functionality through an ImGui app.
 * 
 */

#ifdef __APPLE__
#ifndef __APPLE_H__

struct SDL_Window;
extern "C" {
    extern void *__AppleUtils_TESTAPPLEUTILS(struct SDL_Window *_game_win);
    extern void __AppleUtils_SetTitlebarAppearsTransparent(void *_win, int _beTransparent);
}

struct AppleUtils
{
    static inline void *TESTAPPLEUTILS(struct SDL_Window *_game_win)
    {
        return __AppleUtils_TESTAPPLEUTILS(_game_win);
    }
    
    static inline void SetTitlebarAppearsTransparent(void *_win, int _beTransparet)
    {
        __AppleUtils_SetTitlebarAppearsTransparent(_win, _beTransparet);
    }
};
// struct AppleUtils
// {
//     constexpr AppleUtils()
//     {
//     };
    
//     ~AppleUtils(){}

//     static inline void Test1(const std::shared_ptr<SDL_Window*> &sdl_win)
//     {
//         // SDL_SysWMinfo wmInfo;
//         // SDL_VERSION(&wmInfo.version);
//         // SDL_GetWindowWMInfo(*sdl_win, &wmInfo);

                

          
//     }
// };

#endif // __APPLE_H__
#endif // __APPLE__