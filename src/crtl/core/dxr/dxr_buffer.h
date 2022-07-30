#pragma once

#include "api_object.h"
#include "dxr_resource.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class Buffer : public crtl::APIObject, public Resource {
    size_t buf_size = 0;

    static D3D12_RESOURCE_DESC res_desc(size_t nbytes, D3D12_RESOURCE_FLAGS flags);

    static Buffer create(ID3D12Device *device,
                         size_t nbytes,
                         D3D12_RESOURCE_STATES state,
                         D3D12_HEAP_PROPERTIES props,
                         D3D12_RESOURCE_DESC desc);

public:
    // Allocate an upload buffer of the desired size
    static Buffer upload(ID3D12Device *device,
                         size_t nbytes,
                         D3D12_RESOURCE_STATES state,
                         D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

    // Allocate a GPU-side buffer of the desired size
    static Buffer device(ID3D12Device *device,
                         size_t nbytes,
                         D3D12_RESOURCE_STATES state,
                         D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

    // Allocate a readback buffer of the desired size
    static Buffer readback(ID3D12Device *device,
                           size_t nbytes,
                           D3D12_RESOURCE_STATES state,
                           D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

    // Map the whole range for potentially being read
    void *map();

    // Map to read a specific or empty range
    void *map(D3D12_RANGE read);

    // Unmap and mark the whole range as written
    void unmap();

    // Unmap and mark a specific range as written
    void unmap(D3D12_RANGE written);

    size_t size() const;
};

}
}
