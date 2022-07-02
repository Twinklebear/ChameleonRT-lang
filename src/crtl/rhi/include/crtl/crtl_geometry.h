#pragma once

#include "crtl_buffer.h"
#include "crtl_device.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct TriangleGeometry : APIObject {};
}
typedef crtl_rhi::TriangleGeometry *CRTLTriangleGeometry;
#else
typedef CRTLAPIObject CRTLTriangleGeometry;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_RHI_EXPORT CRTLTriangleGeometry crtl_new_triangle_geometry(CRTLDevice device,
                                                                CRTLBufferView vertices,
                                                                CRTLBufferView indices);

#ifdef __cplusplus
}
#endif
