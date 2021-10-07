#ifndef __VIDEO_BACKEND_H__

#ifndef DEFAULT_BACKEND
#define DEFAULT_BACKEND SDL2

#include "common_types.h"
#include <SDL.h>

#include <memory>

#define _BACKEND_TYPE SDL2_GAME
#define _BACKEND_TYPE_STR _xstr(_BACKEND_TYPE)
#define _IF_BACKEND_EQUALS(_TYPE)\
    if(typeid(_BACKEND_TYPE) == typeid(_TYPE))

#define _ENDIF_BACKEND_EQUALS()

struct SDL2_GAME
{
    SDL2_GAME()
    {
        constexpr auto SDL_INIT_FLAGS = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER;
        if(SDL_Init(SDL_INIT_FLAGS) != 0)
        {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            // TODO: Fix macOS High DPI support
            // https://github.com/ocornut/imgui/issues/3757#issuecomment-800921198
            auto window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_ALLOW_HIGHDPI*/;
            game_window = std::make_shared<SDL_Window*>(SDL_CreateWindow("Just a test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags));
            
            renderer = std::make_shared<SDL_Renderer*>(SDL_CreateRenderer(*game_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_WINDOW_OPENGL));
        }
    }

    ~SDL2_GAME()
    {
        if(renderer != nullptr)
            SDL_DestroyRenderer(*renderer);
        if(game_window != nullptr)
            SDL_DestroyWindow(*game_window);

        std::cout << "SDL2 Cleaned Up." << std::endl;
    }

    inline std::tuple<int, int> get_window_size()
    {
        int w, h;
        SDL_GetWindowSize(*game_window, &w, &h);
        return std::tuple<int, int>(w, h);
    }

    void begin_frame()
    {}

    void end_frame()
    {
        SDL_SetRenderDrawColor(*renderer, clear_color.red, clear_color.green, clear_color.blue, clear_color.alpha);
        SDL_RenderClear(*renderer);

        if(_UI_Hook_Render != nullptr)
            _UI_Hook_Render();
        /*
        // TODO: Get Draw Data from ImGui?
        auto drawData = ImGui::GetDrawData();
        ImGui_ImplSDLRenderer_RenderDrawData(drawData);
        
        */

       SDL_RenderPresent(*renderer);
    }
private:
    bool is_valid = false;
    void (*_UI_Hook_Render)() = nullptr;
    std::shared_ptr<SDL_Window*> game_window;
    std::shared_ptr<SDL_Renderer*> renderer;
    static constexpr struct Color clear_color = Color(25, 25, 25, 255);
public:
#define GETTER(_TYPE, _OBJ_NAME)\
    _TYPE get_ ## _OBJ_NAME(){return (_OBJ_NAME);}
#define SETTER(_TYPE, _OBJ_NAME)\
    void set_ ## _OBJ_NAME((_TYPE) newVal) { _OBJ_NAME = newVal;}
#define SETTER_FN(_TYPE, _OBJ_NAME)\
    void set_ ## _OBJ_NAME(_TYPE (newVal)()) { _OBJ_NAME = newVal; }

    GETTER(std::shared_ptr<SDL_Window*>, game_window);
    GETTER(std::shared_ptr<SDL_Renderer*>, renderer);

    SETTER_FN(void, _UI_Hook_Render);

#undef SETTER
#undef GETTER
};
#endif

#endif // __VIDEO_BACKEND_H__