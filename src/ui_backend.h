#ifndef __UI_BACKEND_H__
#define __UI_BACKEND_H__

#include <memory>

#include "common_types.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#define _UI_BACKEND_TYPE IMGUI_UI
#define _UI_BACKEND_TYPE_STR _xstr(_UI_BACKEND_TYPE)

#define _IF_UI_BACKEND_EQUALS(_TYPE)\
    if(typeid(_UI_BACKEND_TYPE) == typeid(_TYPE))

#define _ENDIF_UI_BACKEND_EQUALS()

struct IMGUI_UI
{
    explicit IMGUI_UI(const _BACKEND_TYPE &sdl2, const char* fontFileName = nullptr)
    {
        game_window = ((_BACKEND_TYPE &)sdl2).get_game_window();
        renderer = ((_BACKEND_TYPE &)sdl2).get_renderer();


        if(*game_window != nullptr && *renderer != nullptr)
        {
            __INIT_IMGUI();
            if(fontFileName != nullptr && strlen(fontFileName) > 0) 
                __INIT_IMGUI_FONT(fontFileName);
        }
        else
        {
            std::cerr << "GAME_WINDOW or RENDERER ARE NULL ON THE PASSED SDL2_GAME OBJECT!" << std::endl;
        }
    }
    ~IMGUI_UI() = default;

    void begin_frame()
    {
        _IF_BACKEND_EQUALS(SDL2_GAME)
        {
            ImGui_ImplSDLRenderer_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
        }
        _ENDIF_BACKEND_EQUALS()

        ImGui::PushFont(/*_HighDPI ?*/ large_font /*: standard_font*/);
    }

    void UI_Render_Hook()
    {
        auto drawData = ImGui::GetDrawData();
        ImGui_ImplSDLRenderer_RenderDrawData(drawData);
    }

    void end_frame() 
    {
        ImGui::PopFont();

        ImGui::Render();
    }
private:
    struct ImFontDeleter
    {
        void operator()(ImFont* f)
        {
            if(f)
            {
                delete f;
            }
        }
    };

    inline void __INIT_IMGUI_FONT(const char* filePath)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();
        standard_font = io.Fonts->AddFontFromFileTTF(filePath, 16);
        large_font = io.Fonts->AddFontFromFileTTF(filePath, 22);
        io.Fonts->Build();

        ImGui_ImplSDLRenderer_CreateFontsTexture();

    }

    inline void __INIT_IMGUI()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        ImGui::StyleColorsDark();

        // ImGui_ImplSDL2_InitForOpenGL(*game_window, nullptr);
        ImGui_ImplSDL2_InitForOpenGL(*game_window, nullptr);
        ImGui_ImplSDLRenderer_Init(*renderer);
    }

    std::shared_ptr<SDL_Window*> game_window;
    std::shared_ptr<SDL_Renderer*> renderer;

    // TODO: Convert these to std::unique_ptr<ImFont> with customer deleter possibly needed?
public:
    ImFont *standard_font;
    ImFont *large_font;

    static constexpr float standard_font_size = 16.f;
    static constexpr float large_font_size = 22.f;

    static constexpr float standard_font_scale = 1.0f;
    static constexpr float large_font_scale = 22.f / 16.f;
};


#endif //__UI_BACKEND_H__