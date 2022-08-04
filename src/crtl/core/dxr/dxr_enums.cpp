#pragma once

#include "dxr_enums.h"

namespace crtl {
namespace dxr {

const D3D12_HEAP_PROPERTIES UPLOAD_HEAP_PROPS = {
    D3D12_HEAP_TYPE_UPLOAD,
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
    D3D12_MEMORY_POOL_UNKNOWN,
    0,
    0,
};

const D3D12_HEAP_PROPERTIES DEVICE_HEAP_PROPS = {
    D3D12_HEAP_TYPE_DEFAULT,
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
    D3D12_MEMORY_POOL_UNKNOWN,
    0,
    0,
};

const D3D12_HEAP_PROPERTIES READBACK_HEAP_PROPS = {
    D3D12_HEAP_TYPE_READBACK,
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
    D3D12_MEMORY_POOL_UNKNOWN,
    0,
    0,
};

D3D12_HEAP_PROPERTIES memory_space_to_heap_properties(CRTL_MEMORY_SPACE memory_space)
{
    switch (memory_space) {
    case CRTL_MEMORY_SPACE_DEVICE:
        return DEVICE_HEAP_PROPS;
    case CRTL_MEMORY_SPACE_READBACK:
        return READBACK_HEAP_PROPS;
    case CRTL_MEMORY_SPACE_UPLOAD:
        return UPLOAD_HEAP_PROPS;
    default:
        return DEVICE_HEAP_PROPS;
    }
}

}
}
