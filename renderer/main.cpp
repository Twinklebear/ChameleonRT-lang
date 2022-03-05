#include <algorithm>
#include <array>
#include <fstream>
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
#include "dxr_utils.h"
#include "imgui.h"
#include "json.hpp"
#include "util.h"

// TODO Note: The precompiled shaders would be included into the app and would need to be in
// some header structure where I could get the right binary for the backend. Runtime
// compilation would take the original CRTL source and build for the target
// For this test though I can just include the compiled dxil directly
#include "small_embedded_dxil.h"

int win_width = 1280;
int win_height = 720;

using Microsoft::WRL::ComPtr;

void run_app(SDL_Window *window, const std::vector<std::string> &args);

void sync_gpu(ComPtr<ID3D12CommandQueue> cmd_queue,
              ComPtr<ID3D12Fence> fence,
              HANDLE fence_evt,
              uint64_t &fence_value);

int main(int argc, const char **argv)
{
    const std::vector<std::string> args(argv, argv + argc);

    // Test loading my compiled shader file
    if (args.size() != 2) {
        std::cerr << "Error: pass the compiled metadata file small.json\n";
        return 1;
    }

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

    run_app(window, args);

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

void run_app(SDL_Window *window, const std::vector<std::string> &args)
{
    auto display = std::make_shared<crtr::dxr::DXDisplay>(window);
    display->resize(win_width, win_height);

    ComPtr<ID3D12CommandQueue> cmd_queue;
    ComPtr<ID3D12CommandAllocator> cmd_allocator;
    ComPtr<ID3D12GraphicsCommandList4> cmd_list;
    uint64_t fence_value = 1;
    ComPtr<ID3D12Fence> fence;
    HANDLE fence_evt;

    display->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    fence_evt = CreateEvent(nullptr, false, false, nullptr);

    // Create the command queue and command allocator
    D3D12_COMMAND_QUEUE_DESC queue_desc = {0};
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CHECK_ERR(display->device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&cmd_queue)));
    CHECK_ERR(display->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                      IID_PPV_ARGS(&cmd_allocator)));

    // Make the command lists
    CHECK_ERR(display->device->CreateCommandList(0,
                                                 D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                 cmd_allocator.Get(),
                                                 nullptr,
                                                 IID_PPV_ARGS(&cmd_list)));
    CHECK_ERR(cmd_list->Close());

    auto render_target =
        crtr::dxr::Texture2D::device(display->device.Get(),
                                     glm::uvec2(win_width, win_height),
                                     D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                     DXGI_FORMAT_R8G8B8A8_UNORM,
                                     D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    // Make the data buffer the shader will expect and write a value into it
    auto data_buffer = crtr::dxr::Buffer::device(
        display->device.Get(), sizeof(glm::vec4), D3D12_RESOURCE_STATE_COPY_DEST);
    {
        auto upload_buffer = crtr::dxr::Buffer::upload(
            display->device.Get(), sizeof(glm::vec4), D3D12_RESOURCE_STATE_GENERIC_READ);

        glm::vec4 value(0.2f, 0.2f, 1.f, 1.f);
        std::memcpy(upload_buffer.map(), &value, sizeof(glm::vec4));
        upload_buffer.unmap();

        CHECK_ERR(cmd_list->Reset(cmd_allocator.Get(), nullptr));
        cmd_list->CopyResource(data_buffer.get(), upload_buffer.get());

        auto barrier = crtr::dxr::barrier_transition(
            data_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        cmd_list->ResourceBarrier(1, &barrier);
        CHECK_ERR(cmd_list->Close());
        ID3D12CommandList *cmd_lists = cmd_list.Get();
        cmd_queue->ExecuteCommandLists(1, &cmd_lists);
        sync_gpu(cmd_queue, fence, fence_evt, fence_value);
    }

    nlohmann::json shader_metadata;
    {
        std::ifstream fin(args[1].c_str());
        fin >> shader_metadata;
    }

    // We know there's only this one shader
    auto raygen_metadata = shader_metadata["entry_points"]["RayGen"];
    const auto raygen_name = utf8_to_utf16(raygen_metadata["name"].get<std::string>());

    crtr::dxr::ShaderLibrary shader_library(small_dxil, sizeof(small_dxil), {raygen_name});

    // TODO: descriptor heap vs inline in the SBT? Single buffer params or small
    // stuff/constants would be in SBT, arrays of resources should be pushed into a descriptor
    // heap?

    // Build local and global root signatures for the parameters
    crtr::dxr::RootSignature local_root_sig;
    {
        auto raygen_params = raygen_metadata["parameters"];
        // TODO: Here future code should pack constants together to avoid extra padding
        // For this test we know we have two constants
        auto params = raygen_params["params"];
        auto params_data = params["members"]["data"];
        auto scale_factor = raygen_params["scale_factor"];

        // TODO: note here srv/uav/etc would be based on the register type
        // listed on the entry
        local_root_sig = crtr::dxr::RootSignatureBuilder::local()
                             .add_cbv("params.constant_buffer",
                                      params["constant_buffer"]["slot"].get<int>(),
                                      params["constant_buffer"]["space"].get<int>())
                             .add_cbv("scale_factor",
                                      scale_factor["slot"].get<int>(),
                                      scale_factor["space"].get<int>())
                             .add_srv("params.data",
                                      params_data["slot"].get<int>(),
                                      params_data["space"].get<int>())
                             .create(display->device.Get());
    }

    crtr::dxr::DescriptorHeap global_desc_heap;
    crtr::dxr::RootSignature global_root_sig;
    {
        // Note: the mapping in expanded globals is just for the user facing side of the API
        // to let us map from the source code params to these params
        auto scene_image = shader_metadata["global_params"]["scene_image"];
        auto scene_test_constant = shader_metadata["global_params"]["scene_test_constant"];

        // TODO/Note: again cbv/uav/srv/etc. would be based on the register type mentioned in
        // the JSON. Note: Any texture descriptors have to go in the descriptor heap though (if
        // I'm remembering right?) So need to inspect the type of the item and the register
        // type

        global_desc_heap = crtr::dxr::DescriptorHeapBuilder()
                               .add_uav_range(scene_image["count"].get<int>(),
                                              scene_image["slot"].get<int>(),
                                              scene_image["space"].get<int>())
                               .create(display->device.Get());

        global_root_sig = crtr::dxr::RootSignatureBuilder::global()
                              .add_desc_heap("cbv_srv_uav_heap", global_desc_heap)
                              .add_constants("scene.test_constant",
                                             scene_test_constant["slot"].get<int>(),
                                             scene_test_constant["count"].get<int>(),
                                             scene_test_constant["space"].get<int>())
                              .create(display->device.Get());
    }

    // TODO: shader payload size and attrib size we could compute in the compiler?
    // max recursion probably not?
    auto rt_pipeline =
        crtr::dxr::RTPipelineBuilder()
            .add_shader_library(shader_library)
            .set_global_root_sig(global_root_sig)
            .set_ray_gen(raygen_name)
            .set_shader_root_sig({raygen_name}, local_root_sig)
            .configure_shader_payload(
                shader_library.export_names(), 4 * sizeof(float), 2 * sizeof(float))
            .set_max_recursion(1)
            .create(display->device.Get());

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

void sync_gpu(ComPtr<ID3D12CommandQueue> cmd_queue,
              ComPtr<ID3D12Fence> fence,
              HANDLE fence_evt,
              uint64_t &fence_value)
{
    const uint64_t signal_val = fence_value++;
    CHECK_ERR(cmd_queue->Signal(fence.Get(), signal_val));

    if (fence->GetCompletedValue() < signal_val) {
        CHECK_ERR(fence->SetEventOnCompletion(signal_val, fence_evt));
        WaitForSingleObject(fence_evt, INFINITE);
    }
}
