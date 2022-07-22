#include "crtl/crtl_geometry.h"
#include "device.h"

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_new_triangle_geometry(CRTLDevice device,
                           CRTLBufferView vertices,
                           CRTLBufferView indices,
                           CRTLTriangleGeometry *geometry)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_triangle_geometry(vertices, indices, geometry);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_renderable(CRTLDevice device,
                                                          CRTLGeometry geometry,
                                                          uint32_t n_ray_types,
                                                          CRTLRenderable *renderable)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_renderable(geometry, n_ray_types, renderable);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR
crtl_set_renderable_shader_record(CRTLDevice device,
                                  CRTLRenderable renderable,
                                  uint32_t index,
                                  CRTLShaderRecord shader_record)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_renderable_shader_record(renderable, index, shader_record);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_group(CRTLDevice device,
                                                     uint32_t n_renderables,
                                                     CRTLGroup *group)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_group(n_renderables, group);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_set_group_renderable(CRTLDevice device,
                                                                CRTLGroup group,
                                                                uint32_t index,
                                                                CRTLRenderable renderable)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_group_renderable(group, index, renderable);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_instance(CRTLDevice device,
                                                        CRTLGroup group,
                                                        CRTLInstance *instance)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_instance(group, instance);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_set_instance_transform(
    CRTLDevice device, CRTLInstance instance, const float *transform_3x4)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_instance_transform(instance, transform_3x4);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_new_scene(CRTLDevice device,
                                                     uint32_t n_instances,
                                                     uint32_t n_ray_types,
                                                     CRTLScene *scene)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->new_scene(n_instances, n_ray_types, scene);
}

extern "C" CRTL_RHI_EXPORT CRTL_ERROR crtl_set_scene_instance(CRTLDevice device,
                                                              CRTLScene scene,
                                                              uint32_t index,
                                                              CRTLInstance instance)
{
    crtl::Device *d = reinterpret_cast<crtl::Device *>(device);
    return d->set_scene_instance(scene, index, instance);
}
