#pragma once

#include <memory>
#include "crtl_dxr_export.h"
#include "device.h"
#include "dxr_utils.h"
#include "parallel_hashmap/phmap.h"

namespace crtl {
namespace dxr {
class CRTL_DXR_EXPORT DXRDevice : public Device {
    /* We return a raw pointer to the app representing its reference(s) to the object and
     * track the internal shared_ptr and reference count in the api_objects map. The one
     * or more application references to the object are represented through the shared ptr
     * stored in the map. When the app ref count reaches 0, the entry is removed from the
     * map
     */
    phmap::flat_hash_map<crtl::APIObject *, std::shared_ptr<crtl::APIObject>> api_objects;

    // D3D12 API Objects
    Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory;
    Microsoft::WRL::ComPtr<ID3D12Device5> d3d12_device;

public:
    int app_ref_count = 0;

    DXRDevice();

    ~DXRDevice() override;

    // CRTL APIs

    CRTL_DEVICE_API device_api() const override;

    CRTL_ERROR get_native_handle(CRTLAPIObject object,
                                 CRTLNativeHandle *native_handle) override;

    CRTL_ERROR retain(crtl::APIObject *obj);

    CRTL_ERROR release(crtl::APIObject *obj);

    // Buffer APIs ====

    CRTL_ERROR new_buffer(CRTL_MEMORY_SPACE memory,
                          CRTL_BUFFER_USAGE usages,
                          size_t size_bytes,
                          CRTLBuffer *buffer) override;

    CRTL_ERROR new_buffer_view(CRTLBuffer buffer,
                               CRTL_DATA_TYPE type,
                               size_t offset_bytes,
                               size_t n_elements,
                               CRTLBufferView *view) override;

    CRTL_ERROR map_buffer_view(CRTLBufferView view,
                               CRTL_BUFFER_MAP_MODE map_mode,
                               void **mapping) override;

    CRTL_ERROR unmap_buffer_view(CRTLBufferView view) override;

    // Event APIs ====

    CRTL_ERROR new_event(CRTLEvent *event) override;

    CRTL_ERROR await_event(CRTLEvent event) override;

    // Geometry APIs ====

    CRTL_ERROR new_triangle_geometry(CRTLBufferView vertices,
                                     CRTLBufferView indices,
                                     CRTL_GEOMETRY_FLAG flags,
                                     CRTLTriangleGeometry *geometry) override;

    CRTL_ERROR new_renderable(CRTLGeometry geometry,
                              uint32_t n_ray_types,
                              CRTLRenderable *renderable) override;

    CRTL_ERROR set_renderable_shader_record(CRTLRenderable renderable,
                                            uint32_t index,
                                            CRTLShaderRecord shader_record) override;

    CRTL_ERROR new_group(
        uint32_t n_renderables,
        CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
        CRTLGroup *group) override;

    CRTL_ERROR set_group_renderable(CRTLGroup group,
                                    uint32_t index,
                                    CRTLRenderable renderable) override;

    CRTL_ERROR new_instance(CRTLGroup group,
                            CRTL_INSTANCE_FLAG flags,
                            CRTLInstance *instance) override;

    CRTL_ERROR set_instance_transform(CRTLInstance instance,
                                      const float *transform_3x4) override;

    CRTL_ERROR new_scene(
        uint32_t n_instances,
        uint32_t n_ray_types,
        CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
        CRTLScene *scene) override;

    CRTL_ERROR set_scene_instance(CRTLScene scene,
                                  uint32_t index,
                                  CRTLInstance instance) override;

    // Parameter Block APIs ====

    CRTL_ERROR set_parameter(CRTLParameterBlock parameter_block,
                             const char *name,
                             CRTL_DATA_TYPE data_type,
                             void *parameter) override;

    // Queue APIs ====

    CRTL_ERROR new_queue(CRTLQueue *queue) override;

    CRTL_ERROR new_command_allocator(CRTLQueue queue,
                                     CRTLCommandAllocator *cmd_allocator) override;

    CRTL_ERROR reset_command_allocator(CRTLCommandAllocator cmd_allocator) override;

    CRTL_ERROR new_command_buffer(CRTLCommandAllocator cmd_allocator,
                                  CRTLCommandBuffer *cmd_buffer) override;

    CRTL_ERROR close_command_buffer(CRTLCommandBuffer cmd_buffer) override;

    CRTL_ERROR submit_command_buffer(CRTLQueue queue,
                                     CRTLCommandBuffer cmd_buffer,
                                     CRTLEvent event) override;

    CRTL_ERROR copy_buffer_to_buffer(CRTLCommandBuffer cmd_buffer,
                                     CRTLBuffer src,
                                     uint64_t src_offset,
                                     CRTLBuffer dst,
                                     uint64_t dst_offset,
                                     uint64_t size) override;

    CRTL_ERROR copy_buffer_to_texture(CRTLCommandBuffer cmd_buffer,
                                      CRTLBuffer src,
                                      uint64_t src_offset,
                                      CRTLTexture dst,
                                      CRTLBox3D region) override;

    CRTL_ERROR copy_texture_to_buffer(CRTLCommandBuffer cmd_buffer,
                                      CRTLTexture src,
                                      CRTLBox3D region,
                                      CRTLBuffer dst,
                                      uint64_t dst_offset) override;

    CRTL_ERROR build_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) override;

    CRTL_ERROR compact_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) override;

    CRTL_ERROR update_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) override;

    CRTL_ERROR build_tlas(CRTLCommandBuffer cmd_buffer, CRTLScene scene) override;

    CRTL_ERROR update_tlas(CRTLCommandBuffer cmd_buffer, CRTLScene scene) override;

    CRTL_ERROR upload_shader_table(CRTLCommandBuffer cmd_buffer,
                                   CRTLRTPipeline pipeline) override;

    CRTL_ERROR set_rtpipeline(CRTLCommandBuffer cmd_buffer,
                              CRTLRTPipeline pipeline) override;

    CRTL_ERROR set_global_parameter_block(
        CRTLCommandBuffer cmd_buffer, CRTLGlobalParameterBlock parameter_block) override;

    CRTL_ERROR dispatch_rays(CRTLCommandBuffer cmd_buffer,
                             uint32_t width,
                             uint32_t height) override;

    CRTL_ERROR barrier_global(CRTLCommandBuffer cmd_buffer,
                              CRTL_BARRIER_STAGE src_stages,
                              CRTL_BARRIER_STAGE dst_stages,
                              CRTL_BARRIER_ACCESS src_accesses,
                              CRTL_BARRIER_ACCESS dst_accesses) override;

    CRTL_ERROR barrier_buffer(CRTLCommandBuffer cmd_buffer,
                              CRTL_BARRIER_STAGE src_stages,
                              CRTL_BARRIER_STAGE dst_stages,
                              CRTL_BARRIER_ACCESS src_accesses,
                              CRTL_BARRIER_ACCESS dst_accesses,
                              CRTLBuffer buffer,
                              uint64_t offset,
                              uint64_t size) override;

    CRTL_ERROR barrier_texture(CRTLCommandBuffer cmd_buffer,
                               CRTL_BARRIER_STAGE src_stages,
                               CRTL_BARRIER_STAGE dst_stages,
                               CRTL_BARRIER_ACCESS src_accesses,
                               CRTL_BARRIER_ACCESS dst_accesses,
                               CRTLTexture texture) override;

    // RT Pipeline APIs ====

    CRTL_ERROR new_rtpipeline(CRTLShaderLibrary shader_library,
                              uint32_t n_miss_records,
                              CRTLRTPipeline *pipeline) override;

    CRTL_ERROR set_raygen_record(CRTLRTPipeline pipeline,
                                 CRTLShaderRecord raygen_record) override;

    CRTL_ERROR set_miss_record(CRTLRTPipeline pipeline,
                               uint32_t index,
                               CRTLShaderRecord miss_record) override;

    CRTL_ERROR set_scene(CRTLRTPipeline pipeline, CRTLScene scene) override;

    CRTL_ERROR build_shader_table(CRTLRTPipeline pipeline) override;

    // Shader APIs ====

    CRTL_ERROR new_shader_library(const char *library_src,
                                  CRTLShaderLibrary *shader_library) override;

    CRTL_ERROR new_global_parameter_block(
        CRTLShaderLibrary shader_library,
        CRTLGlobalParameterBlock *parameter_block) override;

    CRTL_ERROR get_shader_entry_point(CRTLShaderLibrary shader_library,
                                      const char *entry_point_name,
                                      CRTLShaderEntryPoint *entry_point) override;

    CRTL_ERROR new_shader_parameter_block(
        CRTLShaderEntryPoint entry_point,
        CRTLShaderParameterBlock *parameter_block) override;

    CRTL_ERROR new_shader_record(CRTLShaderEntryPoint entry_point,
                                 CRTLShaderRecord *shader_record) override;

    CRTL_ERROR set_shader_record_parameter_block(
        CRTLShaderRecord shader_record,
        CRTLShaderParameterBlock parameter_block) override;

    // Texture APIs ====

    CRTL_ERROR new_texture(CRTL_TEXTURE_TYPE texture_type,
                           CRTL_IMAGE_FORMAT format,
                           CRTL_IMAGE_USAGE usages,
                           const uint32_t dimensions[3],
                           CRTLTexture *texture) override;

    // Internal APIs for DXRDevice
    template <typename T, typename... Args>
    std::shared_ptr<T> make_api_object(Args &&...args)
    {
        auto o = std::make_shared<T>(std::forward<Args>(args)...);
        o->app_ref_count = 1;
        api_objects[o.get()] = o;
        return o;
    }

    template <typename T>
    std::shared_ptr<T> lookup_api_object(crtl::APIObject *o)
    {
        auto fnd = api_objects.find(o);
        if (fnd != api_objects.end()) {
            return std::dynamic_pointer_cast<T>(fnd->second);
        }
        return nullptr;
    }

    template <typename Fn>
    CRTL_ERROR wrap_try_catch(const Fn &fn)
    {
        try {
            return fn();
        } catch (const Error &e) {
            report_error(e.get_error(), e.what());
            return e.get_error();

        }
        // The wildcard catch shouldn't be enabled in debug builds to make it easier to
        // catch exceptions thrown from errors in the backend
#ifndef _DEBUG
        catch (const std::runtime_error &e) {
            report_error(CRTL_ERROR_UNKNOWN, e.what());
            return CRTL_ERROR_UNKNOWN;
        }
#endif
    }

    Microsoft::WRL::ComPtr<IDXGIFactory2> get_dxgi_factory();

    Microsoft::WRL::ComPtr<ID3D12Device5> get_d3d12_device();
};
}
}
