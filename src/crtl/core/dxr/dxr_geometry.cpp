#include "dxr_geometry.h"
#include <wingdi.h>

namespace crtl {
namespace dxr {

Geometry::Geometry(Buffer verts,
                   Buffer indices,
                   Buffer normals,
                   Buffer uvs,
                   D3D12_RAYTRACING_GEOMETRY_FLAGS geom_flags)
    : vertex_buf(verts), index_buf(indices), normal_buf(normals), uv_buf(uvs)
{
    desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    desc.Triangles.VertexBuffer.StartAddress = vertex_buf->GetGPUVirtualAddress();
    desc.Triangles.VertexBuffer.StrideInBytes = sizeof(float) * 3;
    desc.Triangles.VertexCount =
        vertex_buf.size() / desc.Triangles.VertexBuffer.StrideInBytes;
    desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;

    desc.Triangles.IndexBuffer = index_buf->GetGPUVirtualAddress();
    desc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
    desc.Triangles.IndexCount = index_buf.size() / sizeof(uint32_t);
    desc.Triangles.Transform3x4 = 0;
    desc.Flags = geom_flags;
}

}
}
