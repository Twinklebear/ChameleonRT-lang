#pragma once

#include "api_object.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class Geometry : public crtl::APIObject {
public:
    D3D12_RAYTRACING_GEOMETRY_DESC desc = {};

    virtual ~Geometry() = default;

    // num prims method, etc
};

}
}
