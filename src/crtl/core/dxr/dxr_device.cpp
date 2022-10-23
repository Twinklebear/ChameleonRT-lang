#include "dxr_device.h"
#include "error.h"

#include "dxr_buffer.h"
#include "dxr_buffer_view.h"
#include "dxr_shader_entry_point.h"
#include "dxr_shader_library.h"
#include "dxr_shader_record.h"
#include "dxr_shader_record_parameter_block.h"
#include "dxr_texture.h"

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
    obj->app_ref_count++;
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::release(crtl::APIObject *obj)
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

// Buffer APIs ====

CRTL_ERROR DXRDevice::new_buffer(CRTL_MEMORY_SPACE memory_space,
                                 CRTL_BUFFER_USAGE usages,
                                 size_t size_bytes,
                                 CRTLBuffer *buffer)
{
    return wrap_try_catch([&]() {
        auto buf = make_api_object<Buffer>(this, memory_space, usages, size_bytes);
        *buffer = reinterpret_cast<CRTLBuffer>(buf.get());
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::new_buffer_view(CRTLBuffer buffer,
                                      CRTL_DATA_TYPE type,
                                      size_t offset_bytes,
                                      size_t n_elements,
                                      CRTLBufferView *view)
{
    auto buf = lookup_api_object<Buffer>(reinterpret_cast<crtl::APIObject *>(buffer));
    if (!buf) {
        report_error(CRTL_ERROR_INVALID_OBJECT,
                     "crtl_new_buffer_view: The CRTLBuffer passed is invalid");
        return CRTL_ERROR_INVALID_OBJECT;
    }
    return wrap_try_catch([&]() {
        auto v = make_api_object<BufferView>(buf, type, offset_bytes, n_elements);
        *view = reinterpret_cast<CRTLBufferView>(v.get());
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::map_buffer_view(CRTLBufferView view,
                                      CRTL_BUFFER_MAP_MODE map_mode,
                                      void **mapping)
{
    auto v = lookup_api_object<BufferView>(reinterpret_cast<crtl::APIObject *>(view));
    if (!v) {
        report_error(CRTL_ERROR_INVALID_OBJECT,
                     "crtl_map_buffer_view: The CRTLBufferView passed is invalid");
        return CRTL_ERROR_INVALID_OBJECT;
    }
    return wrap_try_catch([&]() {
        *mapping = v->map(map_mode);
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::unmap_buffer_view(CRTLBufferView view)
{
    auto v = lookup_api_object<BufferView>(reinterpret_cast<crtl::APIObject *>(view));
    if (!v) {
        report_error(CRTL_ERROR_INVALID_OBJECT,
                     "crtl_unmap_buffer_view: The CRTLBufferView passed is invalid");
        return CRTL_ERROR_INVALID_OBJECT;
    }
    return wrap_try_catch([&]() {
        v->unmap();
        return CRTL_ERROR_NONE;
    });
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
    return wrap_try_catch([&]() {
        auto pb = lookup_api_object<ParameterBlock>(
            reinterpret_cast<crtl::APIObject *>(parameter_block));
        switch (data_type) {
        case CRTL_DATA_TYPE_INT:
        case CRTL_DATA_TYPE_INT2:
        case CRTL_DATA_TYPE_INT3:
        case CRTL_DATA_TYPE_INT4:
        case CRTL_DATA_TYPE_INT2X1:
        case CRTL_DATA_TYPE_INT3X1:
        case CRTL_DATA_TYPE_INT4X1:
        case CRTL_DATA_TYPE_INT1X2:
        case CRTL_DATA_TYPE_INT2X2:
        case CRTL_DATA_TYPE_INT3X2:
        case CRTL_DATA_TYPE_INT4X2:
        case CRTL_DATA_TYPE_INT1X3:
        case CRTL_DATA_TYPE_INT2X3:
        case CRTL_DATA_TYPE_INT3X3:
        case CRTL_DATA_TYPE_INT4X3:
        case CRTL_DATA_TYPE_INT1X4:
        case CRTL_DATA_TYPE_INT2X4:
        case CRTL_DATA_TYPE_INT3X4:
        case CRTL_DATA_TYPE_INT4X4:
        case CRTL_DATA_TYPE_UINT:
        case CRTL_DATA_TYPE_UINT2:
        case CRTL_DATA_TYPE_UINT3:
        case CRTL_DATA_TYPE_UINT4:
        case CRTL_DATA_TYPE_UINT2X1:
        case CRTL_DATA_TYPE_UINT3X1:
        case CRTL_DATA_TYPE_UINT4X1:
        case CRTL_DATA_TYPE_UINT1X2:
        case CRTL_DATA_TYPE_UINT2X2:
        case CRTL_DATA_TYPE_UINT3X2:
        case CRTL_DATA_TYPE_UINT4X2:
        case CRTL_DATA_TYPE_UINT1X3:
        case CRTL_DATA_TYPE_UINT2X3:
        case CRTL_DATA_TYPE_UINT3X3:
        case CRTL_DATA_TYPE_UINT4X3:
        case CRTL_DATA_TYPE_UINT1X4:
        case CRTL_DATA_TYPE_UINT2X4:
        case CRTL_DATA_TYPE_UINT3X4:
        case CRTL_DATA_TYPE_UINT4X4:
        case CRTL_DATA_TYPE_FLOAT:
        case CRTL_DATA_TYPE_FLOAT2:
        case CRTL_DATA_TYPE_FLOAT3:
        case CRTL_DATA_TYPE_FLOAT4:
        case CRTL_DATA_TYPE_FLOAT2X1:
        case CRTL_DATA_TYPE_FLOAT3X1:
        case CRTL_DATA_TYPE_FLOAT4X1:
        case CRTL_DATA_TYPE_FLOAT1X2:
        case CRTL_DATA_TYPE_FLOAT2X2:
        case CRTL_DATA_TYPE_FLOAT3X2:
        case CRTL_DATA_TYPE_FLOAT4X2:
        case CRTL_DATA_TYPE_FLOAT1X3:
        case CRTL_DATA_TYPE_FLOAT2X3:
        case CRTL_DATA_TYPE_FLOAT3X3:
        case CRTL_DATA_TYPE_FLOAT4X3:
        case CRTL_DATA_TYPE_FLOAT1X4:
        case CRTL_DATA_TYPE_FLOAT2X4:
        case CRTL_DATA_TYPE_FLOAT3X4:
        case CRTL_DATA_TYPE_FLOAT4X4:
        case CRTL_DATA_TYPE_DOUBLE:
        case CRTL_DATA_TYPE_DOUBLE2:
        case CRTL_DATA_TYPE_DOUBLE3:
        case CRTL_DATA_TYPE_DOUBLE4:
        case CRTL_DATA_TYPE_DOUBLE2X1:
        case CRTL_DATA_TYPE_DOUBLE3X1:
        case CRTL_DATA_TYPE_DOUBLE4X1:
        case CRTL_DATA_TYPE_DOUBLE1X2:
        case CRTL_DATA_TYPE_DOUBLE2X2:
        case CRTL_DATA_TYPE_DOUBLE3X2:
        case CRTL_DATA_TYPE_DOUBLE4X2:
        case CRTL_DATA_TYPE_DOUBLE1X3:
        case CRTL_DATA_TYPE_DOUBLE2X3:
        case CRTL_DATA_TYPE_DOUBLE3X3:
        case CRTL_DATA_TYPE_DOUBLE4X3:
        case CRTL_DATA_TYPE_DOUBLE1X4:
        case CRTL_DATA_TYPE_DOUBLE2X4:
        case CRTL_DATA_TYPE_DOUBLE3X4:
        case CRTL_DATA_TYPE_DOUBLE4X4:
            pb->set_parameter(name, data_type, parameter);
            break;
        case CRTL_DATA_TYPE_BUFFER_VIEW:
        case CRTL_DATA_TYPE_RWBUFFER_VIEW:
            pb->set_parameter(name,
                              data_type,
                              lookup_api_object<BufferView>(
                                  *reinterpret_cast<crtl::APIObject **>(parameter)));
            break;
        // TODO: texture/acceleration structures
        // case CRTL_DATA_TYPE_TEXTURE:
        // case CRTL_DATA_TYPE_RWTEXTURE:
        // case CRTL_DATA_TYPE_ACCELERATION_STRUCTURE:

        // Raw buffers/RWbuffer not supported, app can just make a byte view of it to set
        // the whole buffer as a param case CRTL_DATA_TYPE_BUFFER: case
        // CRTL_DATA_TYPE_RWBUFFER:
        default:
            return CRTL_ERROR_INVALID_PARAMETER_TYPE;
        }
        return CRTL_ERROR_NONE;
    });
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

// Shader APIs ====

CRTL_ERROR DXRDevice::new_shader_library(const char *library_src,
                                         CRTLShaderLibrary *shader_library)
{
    return wrap_try_catch([&]() {
        auto lib = make_api_object<ShaderLibrary>(library_src);
        *shader_library = reinterpret_cast<CRTLShaderLibrary>(lib.get());
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::new_global_parameter_block(
    CRTLShaderLibrary shader_library, CRTLGlobalParameterBlock *parameter_block)
{
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::get_shader_entry_point(CRTLShaderLibrary shader_library,
                                             const char *entry_point_name,
                                             CRTLShaderEntryPoint *entry_point)
{
    return wrap_try_catch([&]() {
        auto slib = lookup_api_object<ShaderLibrary>(
            reinterpret_cast<crtl::APIObject *>(shader_library));
        auto entry_pt = make_api_object<ShaderEntryPoint>(this, entry_point_name, slib);
        *entry_point = reinterpret_cast<CRTLShaderEntryPoint>(entry_pt.get());
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::new_hitgroup_record(CRTLShaderEntryPoint closest_hit,
                                          CRTLShaderEntryPoint intersection_optional,
                                          CRTLShaderEntryPoint any_hit_optional,
                                          CRTLHitGroupRecord *shader_record)
{
    // TODO: will be changed
    /*
    return wrap_try_catch([&]() {
        auto chit = lookup_api_object<ShaderEntryPoint>(
            reinterpret_cast<crtl::APIObject *>(closest_hit));
        auto intersection = lookup_api_object<ShaderEntryPoint>(
            reinterpret_cast<crtl::APIObject *>(intersection_optional));
        auto any_hit = lookup_api_object<ShaderEntryPoint>(
            reinterpret_cast<crtl::APIObject *>(any_hit_optional));

        auto sr = make_api_object<HitGroupRecord>(chit, intersection, any_hit);
        *shader_record = reinterpret_cast<CRTLHitGroupRecord>(sr.get());
        return CRTL_ERROR_NONE;
    });
    */
    return CRTL_ERROR_NONE;
}

CRTL_ERROR DXRDevice::new_miss_record(CRTLShaderEntryPoint miss,
                                      CRTLMissRecord *shader_record)
{
    return wrap_try_catch([&]() {
        auto m = lookup_api_object<ShaderEntryPoint>(
            reinterpret_cast<crtl::APIObject *>(miss));

        auto sr = make_api_object<MissRecord>(m);
        *shader_record = reinterpret_cast<CRTLMissRecord>(sr.get());
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::new_raygen_record(CRTLShaderEntryPoint raygen,
                                        CRTLRaygenRecord *shader_record)
{
    return wrap_try_catch([&]() {
        auto rg = lookup_api_object<ShaderEntryPoint>(
            reinterpret_cast<crtl::APIObject *>(raygen));

        auto sr = make_api_object<RaygenRecord>(rg);
        *shader_record = reinterpret_cast<CRTLRaygenRecord>(sr.get());
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::new_shader_record_parameter_block(
    CRTLShaderRecord shader_record, CRTLShaderRecordParameterBlock *parameter_block)
{
    return wrap_try_catch([&]() {
        auto sr = lookup_api_object<ShaderRecord>(
            reinterpret_cast<crtl::APIObject *>(shader_record));
        auto pb = make_api_object<ShaderRecordParameterBlock>(sr);
        *parameter_block = reinterpret_cast<CRTLShaderRecordParameterBlock>(pb.get());
        return CRTL_ERROR_NONE;
    });
}

CRTL_ERROR DXRDevice::set_shader_record_parameter_block(
    CRTLShaderRecord shader_record, CRTLShaderRecordParameterBlock parameter_block)
{
    return wrap_try_catch([&]() {
        auto sr = lookup_api_object<ShaderRecord>(
            reinterpret_cast<crtl::APIObject *>(shader_record));
        auto pb = lookup_api_object<ShaderRecordParameterBlock>(
            reinterpret_cast<crtl::APIObject *>(parameter_block));
        sr->set_parameter_block(pb);
        return CRTL_ERROR_NONE;
    });
}

// Texture APIs ====

CRTL_ERROR DXRDevice::new_texture(CRTL_TEXTURE_TYPE texture_type,
                                  CRTL_IMAGE_FORMAT format,
                                  CRTL_IMAGE_USAGE usages,
                                  const uint32_t dimensions[3],
                                  CRTLTexture *texture)
{
    return wrap_try_catch([&]() {
        const glm::uvec3 d(dimensions[0], dimensions[1], dimensions[2]);
        auto tex = make_api_object<Texture>(this, texture_type, format, usages, d);
        *texture = reinterpret_cast<CRTLTexture>(tex.get());
        return CRTL_ERROR_NONE;
    });
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
