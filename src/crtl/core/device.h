#pragma once

#include "api_object.h"
#include "crtl/crtl.h"
#include "shader_library.h"

namespace crtl {
/* A Device is an API + HW that we will execute rendering on,
 * e.g. DX12 + a GPU, Embree + a CPU, etc.
 */
class CRTL_EXPORT Device : public APIObject {
    CRTLErrorCallback error_callback = nullptr;

public:
    Device() = default;

    virtual ~Device() = default;

    virtual CRTL_DEVICE_API device_api() const = 0;

    CRTL_ERROR register_error_callback(CRTLErrorCallback error_callback);

    virtual CRTL_ERROR get_native_handle(CRTLAPIObject object,
                                         CRTLNativeHandle *native_handle) = 0;

    // Buffer APIs ====

    virtual CRTL_ERROR new_buffer(CRTL_MEMORY_SPACE memory_space,
                                  CRTL_BUFFER_USAGE usages,
                                  size_t size_bytes,
                                  CRTLBuffer *buffer) = 0;

    virtual CRTL_ERROR new_buffer_view(CRTLBuffer buffer,
                                       CRTL_DATA_TYPE type,
                                       size_t offset_bytes,
                                       size_t n_elements,
                                       CRTLBufferView *view) = 0;

    virtual CRTL_ERROR map_buffer_view(CRTLBufferView view, void **mapping) = 0;

    virtual CRTL_ERROR unmap_buffer_view(CRTLBufferView view) = 0;

    // Event APIs ====

    virtual CRTL_ERROR new_event(CRTLEvent *event) = 0;

    virtual CRTL_ERROR await_event(CRTLEvent event) = 0;

    // Geometry APIs ====

    virtual CRTL_ERROR new_triangle_geometry(CRTLBufferView vertices,
                                             CRTLBufferView indices,
                                             CRTL_GEOMETRY_FLAG flags,
                                             CRTLTriangleGeometry *geometry) = 0;

    virtual CRTL_ERROR new_renderable(CRTLGeometry geometry,
                                      uint32_t n_ray_types,
                                      CRTLRenderable *renderable) = 0;

    virtual CRTL_ERROR set_renderable_shader_record(CRTLRenderable renderable,
                                                    uint32_t index,
                                                    CRTLShaderRecord shader_record) = 0;

    virtual CRTL_ERROR new_group(
        uint32_t n_renderables,
        CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
        CRTLGroup *group) = 0;

    virtual CRTL_ERROR set_group_renderable(CRTLGroup group,
                                            uint32_t index,
                                            CRTLRenderable renderable) = 0;

    virtual CRTL_ERROR new_instance(CRTLGroup group,
                                    CRTL_INSTANCE_FLAG flags,
                                    CRTLInstance *instance) = 0;

    virtual CRTL_ERROR set_instance_transform(CRTLInstance instance,
                                              const float *transform_3x4) = 0;

    virtual CRTL_ERROR new_scene(
        uint32_t n_instances,
        uint32_t n_ray_types,
        CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
        CRTLScene *scene) = 0;

    virtual CRTL_ERROR set_scene_instance(CRTLScene scene,
                                          uint32_t index,
                                          CRTLInstance instance) = 0;

    // Parameter Block APIs ====

    virtual CRTL_ERROR set_parameter(CRTLParameterBlock parameter_block,
                                     const char *name,
                                     CRTL_DATA_TYPE data_type,
                                     void *parameter) = 0;

    // Queue APIs ====

    virtual CRTL_ERROR new_queue(CRTLQueue *queue) = 0;

    virtual CRTL_ERROR new_command_allocator(CRTLQueue queue,
                                             CRTLCommandAllocator *cmd_allocator) = 0;

    virtual CRTL_ERROR reset_command_allocator(CRTLCommandAllocator cmd_allocator) = 0;

    virtual CRTL_ERROR new_command_buffer(CRTLCommandAllocator cmd_allocator,
                                          CRTLCommandBuffer *cmd_buffer) = 0;

    virtual CRTL_ERROR close_command_buffer(CRTLCommandBuffer cmd_buffer) = 0;

    virtual CRTL_ERROR submit_command_buffer(CRTLQueue queue,
                                             CRTLCommandBuffer cmd_buffer,
                                             CRTLEvent event) = 0;

    virtual CRTL_ERROR copy_buffer_to_buffer(CRTLCommandBuffer cmd_buffer,
                                             CRTLBuffer src,
                                             uint64_t src_offset,
                                             CRTLBuffer dst,
                                             uint64_t dst_offset,
                                             uint64_t size) = 0;

    virtual CRTL_ERROR copy_buffer_to_texture(CRTLCommandBuffer cmd_buffer,
                                              CRTLBuffer src,
                                              uint64_t src_offset,
                                              CRTLTexture dst,
                                              CRTLBox3D region) = 0;

    virtual CRTL_ERROR copy_texture_to_buffer(CRTLCommandBuffer cmd_buffer,
                                              CRTLTexture src,
                                              CRTLBox3D region,
                                              CRTLBuffer dst,
                                              uint64_t dst_offset) = 0;

    virtual CRTL_ERROR build_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) = 0;

    virtual CRTL_ERROR compact_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) = 0;

    virtual CRTL_ERROR update_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) = 0;

    virtual CRTL_ERROR build_tlas(CRTLCommandBuffer cmd_buffer, CRTLScene scene) = 0;

    virtual CRTL_ERROR update_tlas(CRTLCommandBuffer cmd_buffer, CRTLScene scene) = 0;

    virtual CRTL_ERROR upload_shader_table(CRTLCommandBuffer cmd_buffer,
                                           CRTLRTPipeline pipeline) = 0;

    virtual CRTL_ERROR set_rtpipeline(CRTLCommandBuffer cmd_buffer,
                                      CRTLRTPipeline pipeline) = 0;

    virtual CRTL_ERROR set_global_parameter_block(
        CRTLCommandBuffer cmd_buffer, CRTLGlobalParameterBlock parameter_block) = 0;

    virtual CRTL_ERROR dispatch_rays(CRTLCommandBuffer cmd_buffer,
                                     uint32_t width,
                                     uint32_t height) = 0;

    virtual CRTL_ERROR barrier_global(CRTLCommandBuffer cmd_buffer,
                                      CRTL_BARRIER_STAGE src_stages,
                                      CRTL_BARRIER_STAGE dst_stages,
                                      CRTL_BARRIER_ACCESS src_accesses,
                                      CRTL_BARRIER_ACCESS dst_accesses) = 0;

    virtual CRTL_ERROR barrier_buffer(CRTLCommandBuffer cmd_buffer,
                                      CRTL_BARRIER_STAGE src_stages,
                                      CRTL_BARRIER_STAGE dst_stages,
                                      CRTL_BARRIER_ACCESS src_accesses,
                                      CRTL_BARRIER_ACCESS dst_accesses,
                                      CRTLBuffer buffer,
                                      uint64_t offset,
                                      uint64_t size) = 0;

    virtual CRTL_ERROR barrier_texture(CRTLCommandBuffer cmd_buffer,
                                       CRTL_BARRIER_STAGE src_stages,
                                       CRTL_BARRIER_STAGE dst_stages,
                                       CRTL_BARRIER_ACCESS src_accesses,
                                       CRTL_BARRIER_ACCESS dst_accesses,
                                       CRTLTexture texture) = 0;

    // RT Pipeline APIs ====

    virtual CRTL_ERROR new_rtpipeline(CRTLShaderLibrary shader_library,
                                      uint32_t n_miss_records,
                                      CRTLRTPipeline *pipeline) = 0;

    virtual CRTL_ERROR set_raygen_record(CRTLRTPipeline pipeline,
                                         CRTLShaderRecord raygen_record) = 0;

    virtual CRTL_ERROR set_miss_record(CRTLRTPipeline pipeline,
                                       uint32_t index,
                                       CRTLShaderRecord miss_record) = 0;

    virtual CRTL_ERROR set_scene(CRTLRTPipeline pipeline, CRTLScene scene) = 0;

    virtual CRTL_ERROR build_shader_table(CRTLRTPipeline pipeline) = 0;

    virtual CRTL_ERROR new_global_parameter_block(
        CRTLRTPipeline pipeline, CRTLGlobalParameterBlock *parameter_block) = 0;

    // Shader APIs ====

    virtual CRTL_ERROR new_shader_library(const char *library_src,
                                          CRTLShaderLibrary *shader_library) = 0;

    virtual CRTL_ERROR get_shader_entry_point(CRTLShaderLibrary shader_library,
                                              const char *entry_point_name,
                                              CRTLShaderEntryPoint *entry_point) = 0;

    virtual CRTL_ERROR new_shader_parameter_block(
        CRTLShaderEntryPoint entry_point, CRTLShaderParameterBlock *parameter_block) = 0;

    virtual CRTL_ERROR new_shader_record(CRTLShaderEntryPoint entry_point,
                                         CRTLShaderRecord *shader_record) = 0;

    virtual CRTL_ERROR set_shader_parameter_block(
        CRTLShaderRecord shader_record, CRTLShaderParameterBlock parameter_block) = 0;

    // Texture APIs ====

    virtual CRTL_ERROR new_texture(CRTL_TEXTURE_TYPE texture_type,
                                   CRTL_IMAGE_FORMAT format,
                                   CRTL_IMAGE_USAGE usages,
                                   uint32_t dimensions[3],
                                   CRTLTexture *texture) = 0;

protected:
    /* Internal convenience API for device implementations to report error's to the
     * application's callback
     */
    void report_error(CRTL_ERROR error, const std::string &message);
};
}
