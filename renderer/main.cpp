#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>
#include <SDL.h>
#include "display/display.h"
#include "display/imgui_impl_sdl.h"
#include "dx12_utils.h"
#include "dxdisplay.h"
#include "imgui.h"
#include "util.h"

int win_width = 1280;
int win_height = 720;

void run_app(SDL_Window *window);

int main(int argc, const char **argv)
{
    const std::vector<std::string> args(argv, argv + argc);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
        return -1;
    }

    const uint32_t window_flags = 0;  // SDL_WINDOW_RESIZABLE;
    // TODO: for vulkan we need vulkan flags here,
    // for CPU/OptiX backends using OpenGL display we need GL flags

    SDL_Window *window = SDL_CreateWindow("ChameleonRT-Language Test App",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          win_width,
                                          win_height,
                                          window_flags);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_Init(window);

    run_app(window);

    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

glm::vec2 transform_mouse(glm::vec2 in)
{
    return glm::vec2(in.x * 2.f / win_width - 1.f, 1.f - 2.f * in.y / win_height);
}

void run_app(SDL_Window *window)
{
    auto display = std::make_shared<crtr::dxr::DXDisplay>(window);
    display->resize(win_width, win_height);

    auto render_target =
        crtr::dxr::Texture2D::device(display->device.Get(),
                                     glm::uvec2(win_width, win_height),
                                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                     DXGI_FORMAT_R8G8B8A8_UNORM,
                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    ImGuiIO &io = ImGui::GetIO();

    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                done = true;
            }
            if (!io.WantCaptureKeyboard && event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                }
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window)) {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_RESIZED) {
                win_width = event.window.data1;
                win_height = event.window.data2;
                io.DisplaySize.x = win_width;
                io.DisplaySize.y = win_height;

                // TODO resizing
            }
        }

        display->new_frame();

        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::Begin("ChameleonRT Lang Demo app");

        ImGui::End();
        ImGui::Render();

        display->display_native(render_target);
    }
}
