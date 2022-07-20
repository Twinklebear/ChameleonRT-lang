#pragma once

#include "crtl/crtl.h"
#include "shader_library.h"

namespace crtl {
/* A Device is an API + HW that we will execute rendering on,
 * e.g. DX12 + a GPU, Embree + a CPU, etc.
 */
class Device {
public:
    virtual ~Device() = default;

    virtual DEVICE_API device_api() const = 0;

    // Buffer APIs ====

    virtual CRTLBuffer new_buffer(CRTL_MEMORY_SPACE memory, size_t size_bytes) = 0;

    virtual CRTLBufferView new_view(CRTLBuffer buffer,
                                    CRTL_DATA_TYPE type,
                                    size_t offset_bytes,
                                    size_t n_elements) = 0;

    virtual void *map_view(CRTLBufferView view) = 0;

    virtual void unmap_view(CRTLBufferView view) = 0;

    // Event APIs ====

    virtual void await_event(CRTLEvent event) = 0;

    // Geometry APIs ====

    virtual CRTLTriangleGeometry new_triangle_geometry(CRTLBufferView vertices,
                                                       CRTLBufferView indices) = 0;

    virtual CRTLRenderable new_renderable(CRTLGeometry geometry,
                                          uint32_t n_ray_types) = 0;

    virtual void set_renderable_shader_record(CRTLRenderable renderable,
                                              uint32_t index,
                                              CRTLShaderRecord shader_record) = 0;

    virtual CRTLGroup new_group(uint32_t n_renderables) = 0;

    virtual void set_group_renderable(CRTLGroup group,
                                      uint32_t index,
                                      CRTLRenderable renderable) = 0;

    virtual CRTLInstance new_instance(CRTLGroup group) = 0;

    virtual void set_instance_transform(CRTLInstance instance,
                                        const float *transform_3x4) = 0;

    virtual CRTLScene new_scene(uint32_t n_instances, uint32_t n_ray_types) = 0;

    virtual void set_scene_instance(CRTLScene scene,
                                    uint32_t index,
                                    CRTLInstance instance) = 0;

    // Parameter Block APIs ====

    virtual void set_parameter(CRTLParameterBlock parameter_block,
                               const char *name,
                               CRTL_DATA_TYPE data_type,
                               void *parameter) = 0;

    // Queue APIs ====

    virtual CRTLQueue new_queue() = 0;

    virtual CRTLEvent submit_command_buffer(CRTLCommandBuffer cmd_buffer) = 0;

    virtual CRTLCommandAllocator new_command_allocator(CRTLQueue queue) = 0;

    virtual void reset_command_allocator(CRTLCommandAllocator cmd_allocator) = 0;

    virtual CRTLCommandBuffer new_command_buffer(CRTLCommandAllocator cmd_allocator) = 0;

    virtual void close_command_buffer(CRTLCommandBuffer cmd_buffer) = 0;

    virtual void copy_buffer_to_buffer(CRTLCommandBuffer cmd_buffer,
                                       CRTLBuffer src,
                                       uint64_t src_offset,
                                       CRTLBuffer dst,
                                       uint64_t dst_offset,
                                       uint64_t size) = 0;

    virtual void copy_buffer_to_texture(CRTLCommandBuffer cmd_buffer,
                                        CRTLBuffer src,
                                        uint64_t src_offset,
                                        CRTLTexture dst,
                                        CRTLBox3D region);

    virtual void copy_texture_to_buffer(CRTLCommandBuffer cmd_buffer,
                                        CRTLTexture src,
                                        CRTLBox3D region,
                                        CRTLBuffer dst,
                                        uint64_t dst_offset) = 0;

    virtual void build_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) = 0;

    virtual void compact_blas(CRTLCommandBuffer cmd_buffer, CRTLGroup group) = 0;

    virtual void build_tlas(CRTLCommandBuffer cmd_buffer, CRTLScene scene) = 0;

    virtual void upload_shader_table(CRTLCommandBuffer cmd_buffer,
                                     CRTLRTPipeline pipeline) = 0;

    virtual void set_rtpipeline(CRTLCommandBuffer cmd_buffer,
                                CRTLRTPipeline pipeline) = 0;

    virtual void set_global_parameter_block(CRTLCommandBuffer cmd_buffer,
                                            CRTLGlobalParameterBlock parameter_block) = 0;

    virtual void dispatch_rays(CRTLCommandBuffer cmd_buffer,
                               uint32_t width,
                               uint32_t height) = 0;

    // RT Pipeline APIs ====

    virtual CRTLRTPipeline new_rtpipeline(CRTLShaderLibrary shader_library,
                                          uint32_t n_miss_records) = 0;

    virtual void set_raygen_record(CRTLRTPipeline pipeline,
                                   CRTLShaderRecord raygen_record) = 0;

    virtual void set_miss_record(CRTLRTPipeline pipeline,
                                 uint32_t index,
                                 CRTLShaderRecord miss_record) = 0;

    virtual void set_scene(CRTLRTPipeline pipeline, CRTLScene scene) = 0;

    virtual void build_shader_table(CRTLRTPipeline pipeline) = 0;

    virtual CRTLGlobalParameterBlock new_global_parameter_block(
        CRTLRTPipeline pipeline) = 0;

    // Shader APIs ====

    virtual CRTLShaderLibrary new_shader_library(const char *library_src) = 0;

    virtual CRTLShaderEntryPoint get_shader_entry_point(CRTLShaderLibrary shader_library,
                                                        const char *entry_point) = 0;

    virtual CRTLShaderParameterBlock new_shader_parameter_block(
        CRTLShaderEntryPoint entry_point) = 0;

    virtual CRTLShaderRecord new_shader_record(CRTLShaderEntryPoint entry_point) = 0;

    virtual void set_shader_parameter_block(CRTLShaderRecord shader_record,
                                            CRTLShaderParameterBlock parameter_block) = 0;

    // Texture APIs ====

    virtual CRTLTexture new_texture(CRTL_TEXTURE_TYPE texture_type,
                                    CRTL_IMAGE_FORMAT image_format,
                                    uint32_t dimensions[3]) = 0;
};
}
