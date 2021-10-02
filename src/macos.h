/**
 * 
 * A collection of useful macOS Specific Functions.
 * Enables special macOS functionality through an ImGui app.
 * 
 */

#ifdef __APPLE__
#ifndef __APPLE_H__

struct SDL_Window;
extern int __AppleUtils_TESTAPPLEUTILS(struct SDL_Window *_game_win);

struct AppleUtils
{
    static inline int TESTAPPLEUTILS(struct SDL_Window *_game_win)
    {
        return __AppleUtils_TESTAPPLEUTILS(_game_win);
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