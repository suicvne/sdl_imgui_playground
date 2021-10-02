#import <Foundation/Foundation.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <stdio.h>

@interface AppleUtils:NSObject
+ (int) TESTAPPLEUTILS:(SDL_Window*)_game_win;
@end

@implementation AppleUtils
+ (int) TESTAPPLEUTILS:(SDL_Window*)_game_win
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(_game_win, &wmInfo);

    NSWindow *window = wmInfo.info.cocoa.window;
    // window->

    printf("Hello from Objective C! NSWindow Pointer: %p\n", window);

    return 0;
}
@end

int __AppleUtils_TESTAPPLEUTILS(SDL_Window *_game_win)
{
    return [AppleUtils TESTAPPLEUTILS:_game_win];
}