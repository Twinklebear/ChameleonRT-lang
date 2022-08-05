#pragma once

#include "api_object.h"
#include "crtl_dxr_export.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class CRTL_DXR_EXPORT Geometry : public crtl::APIObject {
public:
    D3D12_RAYTRACING_GEOMETRY_DESC desc = {};

    virtual ~Geometry() = default;

    // num prims method, etc
};
}
}
