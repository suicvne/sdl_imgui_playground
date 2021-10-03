#include <iostream>
#include <thread>
#include <cassert>

#include <SDL.h>
#include "macos.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_sdlrenderer.h"

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

struct str_const
{
private:
    const char* const _p;
    const std::size_t _sz;
public:
    template<std::size_t N>
    constexpr str_const(const char(&a)[N]) : _p(a), _sz(N-1){} // Move constructor. a is a char array of size N.
    constexpr char operator[](std::size_t n) const
    {
        return n < _sz ? _p[n] : throw std::out_of_range("");
    }
};

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

static inline void print_color(const struct Color &col)
{
    std::cout << "Color(" << col.red << ", " << col.green << ", " << col.blue << ", " << col.alpha << ")" << std::endl;
}

struct SDL2_GAME
{
    friend struct IMGUI_UI;
    SDL2_GAME()
    {
        std::cout << "SDL Clear Color: ";
        // print_color(SDL2_GAME::clear_color);

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
    {

    }

    void end_frame()
    {
        SDL_SetRenderDrawColor(*renderer, clear_color.red, clear_color.green, clear_color.blue, clear_color.alpha);
        SDL_RenderClear(*renderer);
        // TODO: Get Draw Data from ImGui?
        auto drawData = ImGui::GetDrawData();
        ImGui_ImplSDLRenderer_RenderDrawData(drawData);
        SDL_RenderPresent(*renderer);
    }
private:
    bool is_valid = false;
    std::shared_ptr<SDL_Window*> game_window;
    std::shared_ptr<SDL_Renderer*> renderer;
    static constexpr struct Color clear_color = Color(25, 25, 25, 255);
};

struct IMGUI_UI
{
    explicit IMGUI_UI(const SDL2_GAME &sdl2, const char* fontFileName = nullptr)
    {
        game_window = sdl2.game_window;
        renderer = sdl2.renderer;


        if(*game_window != nullptr && *renderer != nullptr)
        {
            __INIT_IMGUI();
            if(fontFileName != nullptr) 
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
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::PushFont(_HighDPI ? large_font : standard_font);
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


#define CLAMP(a, min, max) ((a) < (min) ? (min) : (a) > (max) ? (max) : (a))

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
    ImGui::SetNextWindowPos(ImVec2_Zero, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_Always);

    ImGui::Begin("Hello World", ((bool*)&show_main_window), 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

    if(ImGui::BeginMenuBar())
    {
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
    ImGui::Text("Frame Time: %lld", last_frame_time.frame_time.count());
    ImGui::Text("Delay Time (to hit 16ms): %lld", last_frame_time.delay_time.count());
    ImGui::End();
}


template<typename VIDEO_BACKEND>
static inline void SimpEventHandler(VIDEO_BACKEND& game, bool& stayAlive)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT) stayAlive = false;
        if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) stayAlive = false;
    }
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

        SDL_Delay(diff.count());
    }
}

int main(int argc, const char **argv)
{
    std::cout << "Hello World!" << std::endl;


    

    SDL2_GAME game;
    const char *curRenderDriver = SDL_GetHint(SDL_HINT_RENDER_DRIVER);
    if(curRenderDriver == NULL)
        std::cerr << "CUR RENDER DRIVER IS NULL!" << std::endl;
    else
        std::cout << "Cur Render Driver: " << curRenderDriver << std::endl;

    IMGUI_UI imgui = IMGUI_UI(game, "Fonts/Roboto-Medium.ttf");

    // std::chrono::

    SimpLoop(game, imgui);

    return 0;
}