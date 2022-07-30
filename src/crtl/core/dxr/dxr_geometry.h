#pragma once

#include "api_object.h"
#include "dxr_buffer.h"
#include "dxr_texture.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

// TODO: Rename to triangle geometry
class Geometry : public crtl::APIObject {
public:
    Buffer vertex_buf, index_buf, normal_buf, uv_buf;
    D3D12_RAYTRACING_GEOMETRY_DESC desc = {0};

    // TODO: Allow other vertex and index formats? Right now this
    // assumes vec3f verts and uint3 indices
    Geometry(Buffer vertex_buf,
             Buffer index_buf,
             Buffer normal_buf,
             Buffer uv_buf,
             D3D12_RAYTRACING_GEOMETRY_FLAGS geom_flags =
                 D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE);
};

}
}
