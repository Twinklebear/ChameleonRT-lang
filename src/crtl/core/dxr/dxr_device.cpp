#include "dxr_device.h"
#include "dxr_buffer.h"
#include "error.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

DXRDevice::DXRDevice()
{
    // Enable debugging for D3D12
#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debug_controller;
        auto err = D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller));
        if (FAILED(err)) {
            throw std::runtime_error("DXRDevice failed to enable debug layer");
        }
        debug_controller->EnableDebugLayer();
    }
#endif

#ifdef _DEBUG
    const uint32_t factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#else
    const uint32_t factory_flags = 0;
#endif
    CHECK_ERR(CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&dxgi_factory)));

    d3d12_device = dxr::create_dxr_device(dxgi_factory);
    if (!d3d12_device) {
        throw Error("DXRDevice failed to find a DXR capable GPU",
                    CRTL_ERROR_UNSUPPORTED_SYSTEM);
    }
}

DXRDevice::~DXRDevice()
{
    // TODO
}

CRTL_DEVICE_API DXRDevice::device_api() const
{
    return CRTL_DEVICE_API_DX12;
}

CRTL_ERROR DXRDevice::get_native_handle(CRTLAPIObject object,
                                        CRTLNativeHandle *native_handle)
{
    // TODO
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::retain(crtl::APIObject *obj)
{
    auto fnd = api_objects.find(obj);
    if (fnd == api_objects.end()) {
        return CRTL_ERROR_INVALID_OBJECT;
    }
    obj->app_ref_count--;

    // If the app no longer holds any rceferences to this object we can remove its
    // reference
    if (obj->app_ref_count <= 0) {
        api_objects.erase(obj);
    }
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::release(crtl::APIObject *obj)
{
    auto fnd = api_objects.find(obj);
    if (fnd == api_objects.end()) {
        return CRTL_ERROR_INVALID_OBJECT;
    }
    obj->app_ref_count++;
    return CRTL_ERROR_NONE;
}

// Buffer APIs ====

CRTL_ERROR DXRDevice::new_buffer(CRTL_MEMORY_SPACE memory_space,
                                 CRTL_BUFFER_USAGE usages,
                                 size_t size_bytes,
                                 CRTLBuffer *buffer)
{
    auto buf = make_api_object<Buffer>(this, memory_space, usages, size_bytes);
    *buffer = reinterpret_cast<CRTLBuffer>(buf.get());
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_buffer_view(CRTLBuffer buffer,
                                      CRTL_DATA_TYPE type,
                                      size_t offset_bytes,
                                      size_t n_elements,
                                      CRTLBufferView *view)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::map_buffer_view(CRTLBufferView view,
                                      CRTL_BUFFER_MAP_MODE map_mode,
                                      void **mapping)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::unmap_buffer_view(CRTLBufferView view)
{
    return CRTL_ERROR_NONE;
}

// Event APIs ====

CRTL_ERROR DXRDevice::new_event(CRTLEvent *event)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::await_event(CRTLEvent event)
{
    return CRTL_ERROR_NONE;
}

// Geometry APIs ====

CRTL_ERROR DXRDevice::new_triangle_geometry(CRTLBufferView vertices,
                                            CRTLBufferView indices,
                                            CRTL_GEOMETRY_FLAG flags,
                                            CRTLTriangleGeometry *geometry)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_renderable(CRTLGeometry geometry,
                                     uint32_t n_ray_types,
                                     CRTLRenderable *renderable)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_renderable_shader_record(CRTLRenderable renderable,
                                                   uint32_t index,
                                                   CRTLShaderRecord shader_record)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_group(
    uint32_t n_renderables,
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
    CRTLGroup *group)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_group_renderable(CRTLGroup group,
                                           uint32_t index,
                                           CRTLRenderable renderable)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_instance(CRTLGroup group,
                                   CRTL_INSTANCE_FLAG flags,
                                   CRTLInstance *instance)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_instance_transform(CRTLInstance instance,
                                             const float *transform_3x4)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_scene(
    uint32_t n_instances,
    uint32_t n_ray_types,
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
    CRTLScene *scene)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_scene_instance(CRTLScene scene,
                                         uint32_t index,
                                         CRTLInstance instance)
{
    return CRTL_ERROR_NONE;
}

// Parameter Block APIs ====

CRTL_ERROR DXRDevice::set_parameter(CRTLParameterBlock parameter_block,
                                    const char *name,
                                    CRTL_DATA_TYPE data_type,
                                    void *parameter)
{
    return CRTL_ERROR_NONE;
}

// Queue APIs ====

CRTL_ERROR DXRDevice::new_queue(CRTLQueue *queue)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_command_allocator(CRTLQueue queue,
                                            CRTLCommandAllocator *cmd_allocator)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::reset_command_allocator(CRTLCommandAllocator cmd_allocator)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_command_buffer(CRTLCommandAllocator cmd_allocator,
                                         CRTLCommandBuffer *cmd_buffer)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::close_command_buffer(CRTLCommandBuffer cmd_buffer)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::submit_command_buffer(CRTLQueue queue,
                                            CRTLCommandBuffer cmd_buffer,
                                            CRTLEvent event)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::copy_buffer_to_buffer(CRTLCommandBuffer cmd_buffer,
                                            CRTLBuffer src,
                                            uint64_t src_offset,
                                            CRTLBuffer dst,
                                            uint64_t dst_offset,
                                            uint64_t size)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::copy_buffer_to_texture(CRTLCommandBuffer cmd_buffer,
                                             CRTLBuffer src,
                                             uint64_t src_offset,
                                             CRTLTexture dst,
                                             CRTLBox3D region)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::copy_texture_to_buffer(CRTLCommandBuffer cmd_buffer,
                                             CRTLTexture src,
                                             CRTLBox3D region,
                                             CRTLBuffer dst,
                                             uint64_t dst_offset)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::build_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::compact_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::update_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::build_tlas(CRTLCommandBuffer cmd_buffer, CRTLScene scene)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::update_tlas(CRTLCommandBuffer cmd_buffer, CRTLScene scene)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::upload_shader_table(CRTLCommandBuffer cmd_buffer,
                                          CRTLRTPipeline pipeline)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_rtpipeline(CRTLCommandBuffer cmd_buffer,
                                     CRTLRTPipeline pipeline)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_global_parameter_block(CRTLCommandBuffer cmd_buffer,
                                                 CRTLGlobalParameterBlock parameter_block)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::dispatch_rays(CRTLCommandBuffer cmd_buffer,
                                    uint32_t width,
                                    uint32_t height)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::barrier_global(CRTLCommandBuffer cmd_buffer,
                                     CRTL_BARRIER_STAGE src_stages,
                                     CRTL_BARRIER_STAGE dst_stages,
                                     CRTL_BARRIER_ACCESS src_accesses,
                                     CRTL_BARRIER_ACCESS dst_accesses)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::barrier_buffer(CRTLCommandBuffer cmd_buffer,
                                     CRTL_BARRIER_STAGE src_stages,
                                     CRTL_BARRIER_STAGE dst_stages,
                                     CRTL_BARRIER_ACCESS src_accesses,
                                     CRTL_BARRIER_ACCESS dst_accesses,
                                     CRTLBuffer buffer,
                                     uint64_t offset,
                                     uint64_t size)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::barrier_texture(CRTLCommandBuffer cmd_buffer,
                                      CRTL_BARRIER_STAGE src_stages,
                                      CRTL_BARRIER_STAGE dst_stages,
                                      CRTL_BARRIER_ACCESS src_accesses,
                                      CRTL_BARRIER_ACCESS dst_accesses,
                                      CRTLTexture texture)
{
    return CRTL_ERROR_NONE;
}

// RT Pipeline APIs ====

CRTL_ERROR DXRDevice::new_rtpipeline(CRTLShaderLibrary shader_library,
                                     uint32_t n_miss_records,
                                     CRTLRTPipeline *pipeline)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_raygen_record(CRTLRTPipeline pipeline,
                                        CRTLShaderRecord raygen_record)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_miss_record(CRTLRTPipeline pipeline,
                                      uint32_t index,
                                      CRTLShaderRecord miss_record)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_scene(CRTLRTPipeline pipeline, CRTLScene scene)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::build_shader_table(CRTLRTPipeline pipeline)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_global_parameter_block(
    CRTLRTPipeline pipeline, CRTLGlobalParameterBlock *parameter_block)
{
    return CRTL_ERROR_NONE;
}

// Shader APIs ====

CRTL_ERROR DXRDevice::new_shader_library(const char *library_src,
                                         CRTLShaderLibrary *shader_library)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::get_shader_entry_point(CRTLShaderLibrary shader_library,
                                             const char *entry_point_name,
                                             CRTLShaderEntryPoint *entry_point)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_shader_parameter_block(
    CRTLShaderEntryPoint entry_point, CRTLShaderParameterBlock *parameter_block)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_shader_record(CRTLShaderEntryPoint entry_point,
                                        CRTLShaderRecord *shader_record)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::set_shader_parameter_block(CRTLShaderRecord shader_record,
                                                 CRTLShaderParameterBlock parameter_block)
{
    return CRTL_ERROR_NONE;
}

// Texture APIs ====

CRTL_ERROR DXRDevice::new_texture(CRTL_TEXTURE_TYPE texture_type,
                                  CRTL_IMAGE_FORMAT format,
                                  CRTL_IMAGE_USAGE usages,
                                  uint32_t dimensions[3],
                                  CRTLTexture *texture)
{
    return CRTL_ERROR_NONE;
}
ComPtr<IDXGIFactory2> DXRDevice::get_dxgi_factory()
{
    return dxgi_factory;
}

ComPtr<ID3D12Device5> DXRDevice::get_d3d12_device()
{
    return d3d12_device;
}
}
}
