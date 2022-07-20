#include "crtl/crtl_geometry.h"

extern "C" CRTL_RHI_EXPORT CRTLTriangleGeometry crtl_new_triangle_geometry(
    CRTLDevice device, CRTLBufferView vertices, CRTLBufferView indices)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT CRTLRenderable crtl_new_renderable(CRTLDevice device,
                                                              CRTLGeometry geometry,
                                                              uint32_t n_ray_types)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_set_renderable_shader_record(
    CRTLDevice device,
    CRTLRenderable renderable,
    uint32_t index,
    CRTLShaderRecord shader_record)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT CRTLGroup crtl_new_group(CRTLDevice device,
                                                    uint32_t n_renderables)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_set_group_renderable(CRTLDevice device,
                                                          CRTLGroup group,
                                                          uint32_t index,
                                                          CRTLRenderable renderable)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT CRTLInstance crtl_new_instance(CRTLDevice device,
                                                          CRTLGroup group)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_set_instance_transform(CRTLDevice device,
                                                            CRTLInstance instance,
                                                            const float *transform_3x4)
{
    // TODO
}

extern "C" CRTL_RHI_EXPORT CRTLScene crtl_new_scene(CRTLDevice device,
                                                    uint32_t n_instances,
                                                    uint32_t n_ray_types)
{
    return nullptr;
}

extern "C" CRTL_RHI_EXPORT void crtl_set_scene_instance(CRTLDevice device,
                                                        CRTLScene scene,
                                                        uint32_t index,
                                                        CRTLInstance instance)
{
    // TODO
}
