#pragma once

#include <string>
#include <vector>
#include "crtl_dxr_export.h"
#include "dxr_descriptor_heap.h"
#include "dxr_root_parameter.h"
#include "dxr_utils.h"
#include "parallel_hashmap/phmap.h"

namespace crtl {
namespace dxr {

class RootSignature;
class RTPipelineBuilder;
class RTPipeline;

// The table doesn't need a name since
// I think there should only ever be one, since you can specify a bunch of ranges
// to reference the single pointer you get, right?
// Actually since it refers into the global descriptor heap we don't need to write
// anything into the actual shader table. I wonder why the RT gems book and other
// resources seem to imply this? Because it seems like you can't read from a different
// heap than the one which is bound.
class CRTL_DXR_EXPORT RootSignatureBuilder {
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    std::vector<RootParam> params;

    void add_descriptor(D3D12_ROOT_PARAMETER_TYPE desc_type,
                        const std::string &name,
                        uint32_t shader_register,
                        uint32_t space);

public:
    static RootSignatureBuilder global();
    static RootSignatureBuilder local();

    RootSignatureBuilder &add_constants(const std::string &name,
                                        uint32_t shader_register,
                                        uint32_t num_vals,
                                        uint32_t space);

    RootSignatureBuilder &add_srv(const std::string &name,
                                  uint32_t shader_register,
                                  uint32_t space);
    RootSignatureBuilder &add_uav(const std::string &name,
                                  uint32_t shader_register,
                                  uint32_t space);
    RootSignatureBuilder &add_cbv(const std::string &name,
                                  uint32_t shader_register,
                                  uint32_t space);
    RootSignatureBuilder &add_desc_heap(const std::string &name,
                                        const DescriptorHeap &heap);

    RootSignature create(ID3D12Device *device);
};

class CRTL_DXR_EXPORT RootSignature {
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> sig;

    // The offsets of the parameters into the shader arguments part
    // of the shader record. The offsets returned account for the shader identifier size
    phmap::flat_hash_map<std::string, RootParam> param_offsets;

    friend class RootSignatureBuilder;
    friend class RTPipelineBuilder;
    friend class RTPipeline;

    RootSignature(D3D12_ROOT_SIGNATURE_FLAGS flags,
                  Microsoft::WRL::ComPtr<ID3D12RootSignature> sig,
                  const std::vector<RootParam> &params);

public:
    RootSignature() = default;

    // Returns size_t max if no such param
    size_t offset(const std::string &name) const;
    size_t size(const std::string &name) const;

    // Return the total size of the root signature arguments
    size_t total_size() const;

    ID3D12RootSignature *operator->();
    ID3D12RootSignature *get();
};
}
}
