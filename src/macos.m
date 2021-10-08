#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/NSColor.h>


#include <SDL.h>
#include <SDL_syswm.h>
#include <stdio.h>

@interface AppleUtils:NSObject
+ (NSWindow *) TESTAPPLEUTILS:(SDL_Window*)_game_win;
+ (void) SetCocoaTitle:(NSWindow *)_window;
+ (void) SetTitlebarAppearsTransparent:(NSWindow *)_window :(bool)_beTransparent;
@end

@implementation AppleUtils
+ (NSWindow *) TESTAPPLEUTILS:(SDL_Window*)_game_win
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(_game_win, &wmInfo);

    NSWindow *window = wmInfo.info.cocoa.window;
    // window->

    printf("Hello from Objective C! NSWindow Pointer: %p\n", window);

    return window;
}

+ (void) SetCocoaTitle:(NSWindow *)_window
{
    // [_window ]
}

+ (void) SetTitlebarAppearsTransparent:(NSWindow *)_window :(bool)_beTransparent
{
    // [_window setTitlebarAppearsTransparent: _beTransparent];
    _window.styleMask |= NSFullSizeContentViewWindowMask;
    _window.titlebarAppearsTransparent = _beTransparent;
    _window.titleVisibility = NSWindowTitleHidden;

    NSColor *blackColor = [NSColor colorWithCalibratedRed:0.f green:0.f blue:0.f alpha:1.f];
    _window.backgroundColor = blackColor;
}
@end

void* __AppleUtils_TESTAPPLEUTILS(SDL_Window *_game_win)
{
    return [AppleUtils TESTAPPLEUTILS:_game_win];
}

void __AppleUtils_SetTitlebarAppearsTransparent(void *_win, int _beTransparent)
{
    return [AppleUtils SetTitlebarAppearsTransparent:_win:(_beTransparent == 0 ? false : true)];
}

