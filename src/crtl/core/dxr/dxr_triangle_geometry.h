#pragma once

#include "dxr_buffer.h"
#include "dxr_geometry.h"

namespace crtl {
namespace dxr {

class TriangleGeometry : public Geometry {
public:
    Buffer vertex_buf, index_buf, normal_buf, uv_buf;

    // TODO: Allow other vertex and index formats? Right now this
    // assumes vec3f verts and uint3 indices
    TriangleGeometry(Buffer vertex_buf,
                     Buffer index_buf,
                     Buffer normal_buf,
                     Buffer uv_buf,
                     D3D12_RAYTRACING_GEOMETRY_FLAGS geom_flags =
                         D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE);
};

}
}
