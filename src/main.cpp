#include <iostream>
#include <thread>
#include <cassert>

#include "video_backend.h"
#include "ui_backend.h"
#include "logging.h"

#ifdef __APPLE__
#include "macos.h"

#define _TRY_FANCY

#endif

// Our globals & constants.
namespace 
{
    static constexpr std::chrono::duration<int64_t, std::milli> ideal_frame_time(16);
    static constexpr std::chrono::duration<int64_t, std::milli> zero(0);

    static bool _DemoWindowShown = false;
    static bool _HighDPI = false;

    static const ImVec2 ImVec2_Zero(0.f, 0.f);
    static constexpr bool show_main_window = true;
}

static struct timeinfo_t<std::chrono::duration<int64_t, std::milli>> last_frame_time;

static inline void perform_clamp_test()
{
    int value = 64, min = 0, max = 32;

    int clamped = CLAMP(value, min, max);
    std::cout << "Value: " << value << "; Clamped: " << clamped << std::endl;
    
    value = 12;
    clamped = CLAMP(value, min, max);
    std::cout << "Value: " << value << "; Clamped: " << clamped << std::endl;

    std::cout << "Passed! Value: " << clamped << std::endl;
}

static inline void render_imgui(const int &w, const int &h, const ImFont* standard_font)
{
    #ifdef __APPLE__
    // ImGui::SetNextWindowPos(ImVec2(0, 32), ImGuiCond_Always);
    #else
    ImGui::SetNextWindowPos(ImVec2_Zero, ImGuiCond_Always);
    #endif
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);

    ImGui::Begin("Hello World", ((bool*)&show_main_window), 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

    if(ImGui::BeginMenuBar())
    {
        #ifdef __APPLE__
        #ifdef _TRY_FANCY
        if(ImGui::BeginMenu("          ", false)) ImGui::EndMenu();
        #endif
        #endif

        if(ImGui::BeginMenu("File", true))
        {
            if(ImGui::MenuItem("Test Menu Item.", nullptr)){}
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Edit", true))
        {
            if(ImGui::MenuItem("Paste", nullptr))
            {}
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("View"))
        {
            if(ImGui::MenuItem("Fullscreen", nullptr)){}
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help"))
        {
            if(ImGui::MenuItem("About", nullptr)){}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("Test text....");
    ImGui::Text("My Window Size (from SDL!): %d x %d", w, h);

    if(ImGui::Button("Show the Test Window"))
    {
        _DemoWindowShown = !_DemoWindowShown;
    }

    if(ImGui::Button("Toggle High-DPI Mode"))
    {
        _HighDPI = !_HighDPI;

        ImGuiStyle& curStyle = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();

        if(_HighDPI)
        { 
            curStyle.ScaleAllSizes(2.0f);
            io.FontGlobalScale = IMGUI_UI::large_font_scale;
            std::cout << "Setting to scale " << IMGUI_UI::large_font_scale << "x" << std::endl;
        }
        else 
        {
            curStyle.ScaleAllSizes(.5f);
            io.FontGlobalScale = IMGUI_UI::standard_font_scale;
        }
    }

    if(_DemoWindowShown)
        ImGui::ShowDemoWindow(&_DemoWindowShown);
    

    ImGui::End();
}

static inline void render_time_debug_imgui()
{
    ImGui::Begin("Time Debug");
    ImGui::Text("Frame Time: %ld", last_frame_time.frame_time.count());
    ImGui::Text("Delay Time (to hit 16ms): %ld", last_frame_time.delay_time.count());
    ImGui::End();
}


template<typename VIDEO_BACKEND>
static inline void SimpEventHandler(VIDEO_BACKEND& game, bool& stayAlive)
{
    _IF_BACKEND_EQUALS(VIDEO_BACKEND)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) stayAlive = false;
            if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) stayAlive = false;
        }
    }
    _ENDIF_BACKEND_EQUALS()
}

template<typename VIDEO_BACKEND, typename UI_BACKEND>
static inline void SimpLoop(VIDEO_BACKEND& game, UI_BACKEND& imgui)
{
    // TODO: Change this?
    bool stayAlive = true;

    std::chrono::time_point<std::chrono::system_clock> t_begin = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> t_end = std::chrono::system_clock::now();
    while(stayAlive)
    {
        // Event/Updating
        SimpEventHandler(game, stayAlive);

        // Render
        imgui.begin_frame();

        auto window_size = game.get_window_size();
        render_imgui(std::get<0>(window_size), std::get<1>(window_size), imgui.standard_font);
        // render_time_debug_imgui();
        imgui.end_frame();
        game.end_frame();

        t_end = std::chrono::system_clock::now();

        auto frameTime = 
            std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_begin);
        auto delayTime = 
            std::chrono::duration_cast<std::chrono::milliseconds>(ideal_frame_time - frameTime);
        
        std::chrono::duration<int64_t, std::milli> diff = CLAMP(delayTime, zero, ideal_frame_time);
        
        last_frame_time.set(delayTime, frameTime);
        // std::cout << "Frame Time: " << frameTime.count() << "; Delay Time: " << diff.count() << std::endl;
        
        // TODO: STANDARD WAY TO DELAY! usleep?

        _IF_BACKEND_EQUALS(VIDEO_BACKEND)
            SDL_Delay(diff.count());
        _ENDIF_BACKEND_EQUALS()
    }
}

/*

TODO FOR FUN:
Mess around with a circular buffer for graphing frame times.
Because why not?

*/

int main(int argc, const char **argv)
{
    ms::cout << "Hello World!" << "\n";
    ms::cout << "Video Backend: " << _BACKEND_TYPE_STR << "\n";
    ms::cout << "UI Backend: " << _UI_BACKEND_TYPE_STR << "\n";
    

    _BACKEND_TYPE game;

    _IF_BACKEND_EQUALS(SDL2_GAME)
    {
        const char *curRenderDriver = SDL_GetHint(SDL_HINT_RENDER_DRIVER);
        if(curRenderDriver != NULL)
            std::cout << "Cur Render Driver: " << curRenderDriver << std::endl;
    }
    _ENDIF_BACKEND_EQUALS()
    

    IMGUI_UI imgui = IMGUI_UI(game, "Fonts/Roboto-Medium.ttf");
    
    // ((void(*)())&IMGUI_UI::UI_Render_Hook)
    game.set__UI_Hook_Render((void(*)())void_cast(&IMGUI_UI::UI_Render_Hook));

    #ifdef __APPLE__
    #ifdef _TRY_FANCY
    auto nsw = AppleUtils::TESTAPPLEUTILS(*(game.get_game_window()));
    AppleUtils::SetTitlebarAppearsTransparent(nsw, true);
    #endif
    #endif

    // std::chrono::

    SimpLoop(game, imgui);

    return 0;
}