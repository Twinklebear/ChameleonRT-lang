#include <array>
#include <iostream>
#include <vector>
#include <SDL.h>
#include <crtl/crtl.h>
#include <glm/glm.hpp>

uint32_t win_width = 1280;
uint32_t win_height = 720;

const std::string small_crtl = R"(
struct SceneParams {
    RWTexture2D<float4> image;
    // TODO: handling direct values here is a bit tricky, I need to generate a buffer
    // or generate push constant setup for DX12/Vulkan. On other backends need to make
    // a buffer for the app and treat it like a Buffer containing (I guess each individual?
    // or all value types?)
    // float test_constant;
};

in SceneParams scene;

struct RayGenParams {
    float4 color;
    Buffer<float4> data;
    float scale_factor;
};

ray_gen RayGen(RayGenParams params)
{
    // TODO: Do need to know the type the built in functions return in case
    // a conversion is needed here for example.
    uint2 pixel = ray_index();
    float4 c;
    c = params.color + params.data[0];
    scene.image[pixel] = params.color * params.scale_factor * c;
}
)";

const char *error_to_string(CRTL_ERROR err);
const char *api_to_string(CRTL_DEVICE_API api);

#define CHECK_CRTL_ERR(FN)                                                   \
    {                                                                        \
        auto fn_err = FN;                                                    \
        if (fn_err != CRTL_ERROR_NONE) {                                     \
            std::cout << #FN << " failed due to " << error_to_string(fn_err) \
                      << std::endl                                           \
                      << std::flush;                                         \
            throw std::runtime_error(#FN);                                   \
        }                                                                    \
    }

void error_callback(CRTL_ERROR err, const char *message)
{
    std::cout << "CRTL Error - " << error_to_string(err) << ": " << message << "\n"
              << std::flush;
}

void run_app(SDL_Window *window, const std::vector<std::string> &args);

int main(int argc, const char **argv)
{
    const std::vector<std::string> args(argv, argv + argc);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
        return -1;
    }

    const uint32_t window_flags = SDL_WINDOW_RESIZABLE;
    // TODO: for vulkan we need vulkan flags here,
    // for CPU/OptiX backends using OpenGL display we need GL flags

    SDL_Window *window = SDL_CreateWindow("ChameleonRT-Language Test App",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          win_width,
                                          win_height,
                                          window_flags);

    run_app(window, args);

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
    CRTLDevice device;
    CHECK_CRTL_ERR(crtl_new_device(CRTL_DEVICE_API_DX12, &device));

    CHECK_CRTL_ERR(crtl_register_device_error_callback(device, error_callback));

    CRTL_DEVICE_API api;
    CHECK_CRTL_ERR(crtl_get_device_api(device, &api));
    std::cout << "Device API: " << api_to_string(api) << "\n";

    CRTLBuffer buffer;
    CHECK_CRTL_ERR(crtl_new_buffer(
        device,
        CRTL_MEMORY_SPACE_UPLOAD,
        (CRTL_BUFFER_USAGE)(CRTL_BUFFER_USAGE_COPY_SRC | CRTL_BUFFER_USAGE_MAP_WRITE),
        sizeof(glm::vec4),
        &buffer));

    CRTLBufferView view;
    CHECK_CRTL_ERR(
        crtl_new_buffer_view(device, buffer, CRTL_DATA_TYPE_FLOAT4, 0, 1, &view));

    {
        glm::vec4 *mapping = nullptr;
        CHECK_CRTL_ERR(crtl_map_buffer_view(device,
                                            view,
                                            CRTL_BUFFER_MAP_MODE_WRITE,
                                            reinterpret_cast<void **>(&mapping)));

        *mapping = glm::vec4(0.2f, 0.2f, 1.f, 1.f);

        CHECK_CRTL_ERR(crtl_unmap_buffer_view(device, view));
    }

    const uint32_t render_target_dims[3] = {win_width, win_height, 1};
    CRTLTexture render_target;
    CHECK_CRTL_ERR(crtl_new_texture(device,
                                    CRTL_TEXTURE_TYPE_2D,
                                    CRTL_IMAGE_FORMAT_R8G8B8A8_UNORM,
                                    CRTL_IMAGE_USAGE_SHADER_READ_WRITE,
                                    render_target_dims,
                                    &render_target));

    CRTLShaderLibrary shader_library;
    CHECK_CRTL_ERR(crtl_new_shader_library(device, small_crtl.c_str(), &shader_library));

    CRTLShaderEntryPoint raygen_entry_point;
    CHECK_CRTL_ERR(crtl_get_shader_entry_point(
        device, shader_library, "RayGen", &raygen_entry_point));

    CRTLRaygenRecord raygen_record;
    CHECK_CRTL_ERR(crtl_new_raygen_record(device, raygen_entry_point, &raygen_record));

    CRTLShaderRecordParameterBlock raygen_params;
    CHECK_CRTL_ERR(
        crtl_new_shader_record_parameter_block(device, raygen_record, &raygen_params));

    {
        glm::vec4 color_param(0.2f, 0.2f, 0.8f, 1.f);
        CHECK_CRTL_ERR(crtl_set_parameter(
            device, raygen_params, "color", CRTL_DATA_TYPE_FLOAT4, &color_param));

        float scale_factor_param = 0.5f;
        CHECK_CRTL_ERR(crtl_set_parameter(device,
                                          raygen_params,
                                          "scale_factor",
                                          CRTL_DATA_TYPE_FLOAT,
                                          &scale_factor_param));
    }

    CHECK_CRTL_ERR(crtl_set_parameter(
        device, raygen_params, "data", CRTL_DATA_TYPE_BUFFER_VIEW, &view));

    CHECK_CRTL_ERR(
        crtl_set_shader_record_parameter_block(device, raygen_record, raygen_params));

    // Next step is to make the pipeline
    CRTLRTPipeline rt_pipeline;
    // TODO: Is it actually valid to not have any miss shaders? It only makes sense if
    // you'll never call trace ray
    CHECK_CRTL_ERR(crtl_new_rtpipeline(device, shader_library, 0, &rt_pipeline));

    CHECK_CRTL_ERR(crtl_set_raygen_record(device, rt_pipeline, raygen_record));

    // TODO: right now skipping the scene setup for testing but later will need to make
    // scene and set it as a parameter
    // CHECK_CRTL_ERR(crtl_set_scene(device, rt_pipeline, scene));

    CHECK_CRTL_ERR(crtl_build_shader_table(device, rt_pipeline));

    // Setup global parameters for the pipeline
    CRTLGlobalParameterBlock global_params;
    CHECK_CRTL_ERR(
        crtl_new_global_parameter_block(device, shader_library, &global_params));

    // TODO: populate the global param block

    // Now we should be able to run the pipeline
    CRTLQueue cmd_queue;
    CHECK_CRTL_ERR(crtl_new_queue(device, &cmd_queue));

    CRTLCommandAllocator cmd_allocator;
    CHECK_CRTL_ERR(crtl_new_command_allocator(device, cmd_queue, &cmd_allocator));

    CRTLCommandBuffer cmd_buffer;
    CHECK_CRTL_ERR(crtl_new_command_buffer(device, cmd_allocator, &cmd_buffer));

    CRTLEvent frame_event;
    CHECK_CRTL_ERR(crtl_new_event(device, &frame_event));

    CHECK_CRTL_ERR(crtl_upload_shader_table(device, cmd_buffer, rt_pipeline));
    // Here should queue a barrier (buffer should be fine but how to get the shader table
    // buffer ref? not exposed right now and forcing a global barrier when updating just
    // SBT for animation would be bad)
    CHECK_CRTL_ERR(crtl_barrier_global(device,
                                       cmd_buffer,
                                       CRTL_BARRIER_STAGE_ALL,
                                       CRTL_BARRIER_STAGE_RAYTRACING,
                                       CRTL_BARRIER_ACCESS_ALL,
                                       CRTL_BARRIER_ACCESS_ALL));
    CHECK_CRTL_ERR(crtl_set_rtpipeline(device, cmd_buffer, rt_pipeline));
    CHECK_CRTL_ERR(crtl_set_global_parameter_block(device, cmd_buffer, global_params));
    CHECK_CRTL_ERR(crtl_dispatch_rays(device, cmd_buffer, win_width, win_height));
    CHECK_CRTL_ERR(crtl_close_command_buffer(device, cmd_buffer));

    CHECK_CRTL_ERR(
        crtl_submit_command_buffer(device, cmd_queue, cmd_buffer, frame_event));
    CHECK_CRTL_ERR(crtl_await_event(device, frame_event));

#if 0
    crtr::dxr::ShaderLibrary shader_library(
        small_dxil, sizeof(small_dxil), {raygen_name});

    // TODO: descriptor heap vs inline in the SBT? Single buffer params or small
    // stuff/constants would be in SBT, arrays of resources should be pushed into a
    // descriptor heap?

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
        // TODO: for constants embedded in root signature: we need to be careful with size
        // and need to count the total # of 4 byte values we'll use for each constant. If
        // the SR is getting too large, it may be good to push some into a constant buffer
        // instead of the shader record, or if we know some would be updated often while
        // others are more static, we'd keep the static ones in the shader record but push
        // the frequently changing ones to a constant buffer. Maybe can expose a concept
        // in the API of SBT inlined data vs global memory data that can let the app
        // control this nicely The count for items in the inlined constants also needs to
        // come from inspecting the size of the thing to get the # of 4 byte values it
        // occupies
        local_root_sig = crtr::dxr::RootSignatureBuilder::local()
                             .add_constants("params.constant_buffer",
                                            params["constant_buffer"]["slot"].get<int>(),
                                            4,
                                            params["constant_buffer"]["space"].get<int>())
                             .add_constants("scale_factor",
                                            scale_factor["slot"].get<int>(),
                                            1,
                                            scale_factor["space"].get<int>())
                             .add_srv("params.data",
                                      params_data["slot"].get<int>(),
                                      params_data["space"].get<int>())
                             .create(display->device.Get());
    }

    crtr::dxr::DescriptorHeap global_desc_heap;
    crtr::dxr::RootSignature global_root_sig;
    {
        // Note: the mapping in expanded globals is just for the user facing side of the
        // API to let us map from the source code params to these params
        auto scene_image = shader_metadata["global_params"]["scene_image"];
        auto scene_test_constant =
            shader_metadata["global_params"]["scene_test_constant"];

        // TODO/Note: again cbv/uav/srv/etc. would be based on the register type mentioned
        // in the JSON. Note: Any texture descriptors have to go in the descriptor heap
        // though (if I'm remembering right?) So need to inspect the type of the item and
        // the register type

        global_desc_heap = crtr::dxr::DescriptorHeapBuilder()
                               .add_uav_range(scene_image["count"].get<int>(),
                                              scene_image["slot"].get<int>(),
                                              scene_image["space"].get<int>())
                               .create(display->device.Get());

        global_root_sig =
            crtr::dxr::RootSignatureBuilder::global()
                // TODO: this binds the whole heap, should also allow specifying subranges
                // for tables.
                .add_desc_heap("cbv_srv_uav_heap", global_desc_heap)
                // TODO: How does this constant binding work here? Inline in the
                // global root signature? Not sure it makes sense here..
                // Yes via
                // https://docs.microsoft.com/en-us/windows/win32/direct3d12/using-constants-directly-in-the-root-signature
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

    rt_pipeline.map_shader_table();
    {
        // Write the raygen shader record into the SBT
        uint8_t *map = rt_pipeline.shader_record(raygen_name);
        const crtr::dxr::RootSignature *sig = rt_pipeline.shader_signature(raygen_name);

        // Write the various parameters for the shader record

        // the constant buffer, params.color
        const glm::vec4 params_color(0.25f, 0.75f, 1.f, 1.f);
        std::memcpy(map + sig->offset("params.constant_buffer"),
                    &params_color,
                    sizeof(glm::vec4));

        const float scale_factor = 0.75f;
        std::memcpy(map + sig->offset("scale_factor"), &scale_factor, sizeof(float));

        D3D12_GPU_VIRTUAL_ADDRESS params_data_addr = data_buffer->GetGPUVirtualAddress();
        std::memcpy(map + sig->offset("params.data"),
                    &params_data_addr,
                    sizeof(D3D12_GPU_VIRTUAL_ADDRESS));
    }
    rt_pipeline.unmap_shader_table();

    // Copy the upload SBT to the copy in device memory
    {
        CHECK_ERR(cmd_allocator->Reset());
        CHECK_ERR(cmd_list->Reset(cmd_allocator.Get(), nullptr));
        rt_pipeline.upload_shader_table(cmd_list.Get());
        std::array<ID3D12CommandList *, 1> cmd_lists = {cmd_list.Get()};
        CHECK_ERR(cmd_list->Close());
        cmd_queue->ExecuteCommandLists(cmd_lists.size(), cmd_lists.data());
        sync_gpu(cmd_queue, fence, fence_evt, fence_value);
    }

    // Finally, build the descriptor heap
    {
        D3D12_CPU_DESCRIPTOR_HANDLE heap_handle = global_desc_heap.cpu_desc_handle();

        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {0};
        uav_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        display->device->CreateUnorderedAccessView(
            render_target.get(), nullptr, &uav_desc, heap_handle);
    }

    // Record the rendering commands
    CHECK_ERR(cmd_allocator->Reset());
    CHECK_ERR(cmd_list->Reset(cmd_allocator.Get(), nullptr));

    ID3D12DescriptorHeap *desc_heap = global_desc_heap.get();
    cmd_list->SetDescriptorHeaps(1, &desc_heap);
    cmd_list->SetComputeRootSignature(rt_pipeline.global_sig());
    // TODO: is there not a way to write this index into the global root signature?
    // or it must be set at list record time like this?
    // TODO: the constant re-packing changes the order of stuff in the descriptor table
    // which we now need to be aware of here. This is set at 0, but it actually gets
    // pushed to 1
    cmd_list->SetComputeRootDescriptorTable(1, global_desc_heap.gpu_desc_handle());
    float test_constant = 0.5f;
    // TODO: Binding slot refers to what: binding slot in the shader code? or the index in
    // the parameter list of the root signature? I'd guess the latter?
    cmd_list->SetComputeRoot32BitConstants(0, 1, &test_constant, 0);

    cmd_list->SetPipelineState1(rt_pipeline.get());
    D3D12_DISPATCH_RAYS_DESC dispatch_rays_desc =
        rt_pipeline.dispatch_rays(glm::uvec2(win_width, win_height));

    // TODO: RT pipeline builder needs to handle the case when there are no hit groups or
    // miss records
    dispatch_rays_desc.MissShaderTable.StartAddress = 0;
    dispatch_rays_desc.MissShaderTable.SizeInBytes = 0;
    dispatch_rays_desc.MissShaderTable.StrideInBytes = 0;

    dispatch_rays_desc.HitGroupTable.StartAddress = 0;
    dispatch_rays_desc.HitGroupTable.SizeInBytes = 0;
    dispatch_rays_desc.HitGroupTable.StrideInBytes = 0;

    cmd_list->DispatchRays(&dispatch_rays_desc);
    CHECK_ERR(cmd_list->Close());
    ID3D12CommandList *render_cmds = cmd_list.Get();
#endif

    std::cout << "Running!\n";
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                }
            }
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window)) {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_RESIZED) {
                win_width = event.window.data1;
                win_height = event.window.data2;

                // TODO resizing
            }
        }
    }

    crtl_release(device, render_target);
    crtl_release(device, view);
    crtl_release(device, buffer);
    crtl_release(device, device);
}

const char *error_to_string(CRTL_ERROR err)
{
    switch (err) {
    case CRTL_ERROR_NONE:
        return "CRTL_ERROR_NONE";
    case CRTL_ERROR_BACKEND_LOADING_FAILED:
        return "CRTL_ERROR_BACKEND_LOADING_FAILED";
    case CRTL_ERROR_OBJECT_NULL:
        return "CRTL_ERROR_OBJECT_NULL";
    case CRTL_ERROR_INVALID_OBJECT:
        return "CRTL_ERROR_INVALID_OBJECT";
    case CRTL_ERROR_INVALID_BUFFER_SIZE:
        return "CRTL_ERROR_INVALID_BUFFER_SIZE";
    case CRTL_ERROR_UNSUPPORTED_SYSTEM:
        return "CRTL_ERROR_UNSUPPORTED_SYSTEM";
    case CRTL_ERROR_INVALID_BUFFER_USAGE:
        return "CRTL_ERROR_INVALID_BUFFER_USAGE";
    case CRTL_ERROR_INVALID_BUFFER_USAGE_FOR_HEAP:
        return "CRTL_ERROR_INVALID_BUFFER_USAGE_FOR_HEAP";
    case CRTL_ERROR_BUFFER_VIEW_ALREADY_MAPPED:
        return "CRTL_ERROR_BUFFER_VIEW_ALREADY_MAPPED";
    case CRTL_ERROR_BUFFER_VIEW_NOT_MAPPED:
        return "CRTL_ERROR_BUFFER_VIEW_NOT_MAPPED";
    case CRTL_ERROR_SHADER_COMPILATION_FAILED:
        return "CRTL_ERROR_SHADER_COMPILATION_FAILED";
    case CRTL_ERROR_NATIVE_SHADER_COMPILATION_FAILED:
        return "CRTL_ERROR_NATIVE_SHADER_COMPILATION_FAILED";
    case CRTL_ERROR_ENTRY_POINT_NOT_FOUND:
        return "CRTL_ERROR_ENTRY_POINT_NOT_FOUND";
    case CRTL_ERROR_INVALID_SHADER_ENTRY_POINT:
        return "CRTL_ERROR_INVALID_SHADER_ENTRY_POINT";
    case CRTL_ERROR_INVALID_PARAMETER_NAME:
        return "CRTL_ERROR_INVALID_PARAMETER_NAME";
    case CRTL_ERROR_INCOMPATIBLE_SHADER_RECORD_PARAMETER_BLOCK:
        return "CRTL_ERROR_INCOMPATIBLE_SHADER_RECORD_PARAMETER_BLOCK";
    case CRTL_ERROR_INVALID_PARAMETER_TYPE:
        return "CRTL_ERROR_INVALID_PARAMETER_TYPE";
    case CRTL_ERROR_UNKNOWN:
    default:
        return "CRTL_ERROR_UNKNOWN";
    }
}

const char *api_to_string(CRTL_DEVICE_API api)
{
    switch (api) {
    case CRTL_DEVICE_API_DX12:
        return "CRTL_DEVICE_API_DX12";
    case CRTL_DEVICE_API_VULKAN:
        return "CRTL_DEVICE_API_VULKAN";
    case CRTL_DEVICE_API_METAL:
        return "CRTL_DEVICE_API_METAL";
    case CRTL_DEVICE_API_OPTIX:
        return "CRTL_DEVICE_API_OPTIX";
    case CRTL_DEVICE_API_EMBREE:
        return "CRTL_DEVICE_API_EMBREE";
    case CRTL_DEVICE_API_UNKNOWN:
    default:
        return "CRTL_DEVICE_API_UNKNOWN";
    }
}
