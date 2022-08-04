#pragma once

#include "crtl/crtl_enums.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

// TODO: Here a bunch of mappers from CRTL enum values to DXR

D3D12_HEAP_PROPERTIES memory_space_to_heap_properties(CRTL_MEMORY_SPACE memory_space);

}
}
