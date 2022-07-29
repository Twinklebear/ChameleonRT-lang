#pragma once

#include <stdint.h>
#include "crtl_buffer.h"
#include "crtl_device.h"
#include "crtl_shader.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct Geometry : APIObject {};
struct TriangleGeometry : Geometry {};
struct Renderable : APIObject {};
struct Group : APIObject {};
struct Instance : APIObject {};
struct Scene : APIObject {};
}
typedef crtl_rhi::Geometry *CRTLGeometry;
typedef crtl_rhi::TriangleGeometry *CRTLTriangleGeometry;
typedef crtl_rhi::Renderable *CRTLRenderable;
typedef crtl_rhi::Group *CRTLGroup;
typedef crtl_rhi::Instance *CRTLInstance;
typedef crtl_rhi::Scene *CRTLScene;
#else
typedef CRTLAPIObject CRTLGeometry;
typedef CRTLGeometry CRTLTriangleGeometry;
typedef CRTLAPIObject CRTLRenderable;
typedef CRTLAPIObject CRTLGroup;
typedef CRTLAPIObject CRTLInstance;
typedef CRTLAPIObject CRTLScene;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Also will need to take some flags for enabling any hit, dynamic geom, etc.
// Some of these flags (any hit or not) will make more sense on the renderable.
CRTL_RHI_EXPORT CRTL_ERROR crtl_new_triangle_geometry(CRTLDevice device,
                                                      CRTLBufferView vertices,
                                                      CRTLBufferView indices,
                                                      CRTL_GEOMETRY_FLAG flags,
                                                      CRTLTriangleGeometry *geometry);

CRTL_RHI_EXPORT CRTL_ERROR crtl_new_renderable(CRTLDevice device,
                                               CRTLGeometry geometry,
                                               uint32_t n_ray_types,
                                               CRTLRenderable *renderable);

// TODO: Can also have an API that takes an array of records for convenience
CRTL_RHI_EXPORT CRTL_ERROR
crtl_set_renderable_shader_record(CRTLDevice device,
                                  CRTLRenderable renderable,
                                  uint32_t index,
                                  CRTLShaderRecord shader_record);

// TODO: Group vs BLAS name?
CRTL_RHI_EXPORT CRTL_ERROR
crtl_new_group(CRTLDevice device,
               uint32_t n_renderables,
               CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
               CRTLGroup *group);

// TODO: Can also have an API that takes an array of renderables for convenience
CRTL_RHI_EXPORT CRTL_ERROR crtl_set_group_renderable(CRTLDevice device,
                                                     CRTLGroup group,
                                                     uint32_t index,
                                                     CRTLRenderable renderable);

CRTL_RHI_EXPORT CRTL_ERROR crtl_new_instance(CRTLDevice device,
                                             CRTLGroup group,
                                             CRTL_INSTANCE_FLAG flags,
                                             CRTLInstance *instance);

/* Set the instance transformation matrix. transform_3x4 should be a 3x4 row-major matrix
 */
CRTL_RHI_EXPORT CRTL_ERROR crtl_set_instance_transform(CRTLDevice device,
                                                       CRTLInstance instance,
                                                       const float *transform_3x4);

// TODO: Also need to take some flags about dynamic scene etc.
// Does taking the n_instances and n_shader_records flags make sense here?
CRTL_RHI_EXPORT CRTL_ERROR
crtl_new_scene(CRTLDevice device,
               uint32_t n_instances,
               uint32_t n_ray_types,
               CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG acceleration_structure_flags,
               CRTLScene *scene);

// TODO: Can also have API that takes an array for convenience
CRTL_RHI_EXPORT CRTL_ERROR crtl_set_scene_instance(CRTLDevice device,
                                                   CRTLScene scene,
                                                   uint32_t index,
                                                   CRTLInstance instance);

#ifdef __cplusplus
}
#endif
