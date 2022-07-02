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
}
typedef crtl_rhi::Geometry *CRTLGeometry;
typedef crtl_rhi::TriangleGeometry *CRTLTriangleGeometry;
typedef crtl_rhi::Renderable *CRTLRenderable;
typedef crtl_rhi::Group *CRTLGroup;
typedef crtl_rhi::Instance *CRTLInstance;
#else
typedef CRTLAPIObject CRTLGeometry;
typedef CRTLGeometry CRTLTriangleGeometry;
typedef CRTLAPIObject CRTLRenderable;
typedef CRTLAPIObject CRTLGroup;
typedef CRTLAPIObject CRTLInstance;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_RHI_EXPORT CRTLTriangleGeometry crtl_new_triangle_geometry(CRTLDevice device,
                                                                CRTLBufferView vertices,
                                                                CRTLBufferView indices);

CRTL_RHI_EXPORT CRTLRenderable crtl_new_renderable(CRTLDevice device,
                                                   CRTLGeometry geometry,
                                                   uint32_t n_shader_records);

// TODO: Can also have an API that takes an array of records for convenience
CRTL_RHI_EXPORT void crtl_set_renderable_shader_record(CRTLDevice device,
                                                       CRTLRenderable renderable,
                                                       uint32_t index,
                                                       CRTLShaderRecord shader_record);

CRTL_RHI_EXPORT CRTLGroup crtl_new_group(CRTLDevice device, uint32_t n_renderables);

// TODO: Can also have an API that takes an array of renderables for convenience
CRTL_RHI_EXPORT void crtl_set_group_renderable(CRTLDevice device,
                                               CRTLGroup group,
                                               uint32_t index,
                                               CRTLRenderable renderable);

CRTL_RHI_EXPORT CRTLInstance crtl_new_instance(CRTLDevice device, CRTLGroup group);

/* Set the instance transformation matrix. transform_3x4 should be a 3x4 row-major matrix
 */
CRTL_RHI_EXPORT void crtl_set_instance_transform(CRTLDevice device,
                                                 CRTLInstance instance,
                                                 const float *transform_3x4);

#ifdef __cplusplus
}
#endif
