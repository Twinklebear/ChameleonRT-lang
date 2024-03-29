#include "dxr_root_signature.h"
#include <algorithm>
#include <numeric>
#include "util.h"

namespace crtl {
namespace dxr {

using Microsoft::WRL::ComPtr;

RootSignatureBuilder RootSignatureBuilder::global()
{
    RootSignatureBuilder sig;
    sig.flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    return sig;
}

RootSignatureBuilder RootSignatureBuilder::local()
{
    RootSignatureBuilder sig;
    sig.flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
    return sig;
}

void RootSignatureBuilder::add_descriptor(D3D12_ROOT_PARAMETER_TYPE desc_type,
                                          const std::string &name,
                                          uint32_t shader_register,
                                          uint32_t space)
{
    D3D12_ROOT_PARAMETER p = {};
    p.ParameterType = desc_type;
    p.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    p.Descriptor.ShaderRegister = shader_register;
    p.Descriptor.RegisterSpace = space;
    params.push_back(RootParam(p, name));
}

RootSignatureBuilder &RootSignatureBuilder::add_constants(const std::string &name,
                                                          uint32_t shader_register,
                                                          uint32_t num_vals,
                                                          uint32_t space)
{
    D3D12_ROOT_PARAMETER p = {};
    p.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    p.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    p.Constants.ShaderRegister = shader_register;
    p.Constants.RegisterSpace = space;
    p.Constants.Num32BitValues = num_vals;
    params.push_back(RootParam(p, name));
    return *this;
}

RootSignatureBuilder &RootSignatureBuilder::add_srv(const std::string &name,
                                                    uint32_t shader_register,
                                                    uint32_t space)
{
    add_descriptor(D3D12_ROOT_PARAMETER_TYPE_SRV, name, shader_register, space);
    return *this;
}
RootSignatureBuilder &RootSignatureBuilder::add_uav(const std::string &name,
                                                    uint32_t shader_register,
                                                    uint32_t space)
{
    add_descriptor(D3D12_ROOT_PARAMETER_TYPE_UAV, name, shader_register, space);
    return *this;
}
RootSignatureBuilder &RootSignatureBuilder::add_cbv(const std::string &name,
                                                    uint32_t shader_register,
                                                    uint32_t space)
{
    add_descriptor(D3D12_ROOT_PARAMETER_TYPE_CBV, name, shader_register, space);
    return *this;
}
/*
RootSignatureBuilder &RootSignatureBuilder::add_desc_heap(const std::string &name,
                                                          const DescriptorHeap &heap)
{
    params.push_back(RootParam(heap.root_param(), name));
    return *this;
}
*/

std::shared_ptr<RootSignature> RootSignatureBuilder::build(ID3D12Device *device)
{
    // Build the set of root parameters from the inputs
    // Pack constant values to the front, since we want to compact the shader record
    // to avoid a layout where we have something like the following:
    // [constant, pad]
    // [pointer]
    // [constant, pad]
    // since we could instead have done:
    // [constant, constant]
    // [pointer]
    std::stable_partition(params.begin(), params.end(), [](const RootParam &p) {
        return p.param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    });

    std::vector<D3D12_ROOT_PARAMETER> all_params;
    std::transform(params.begin(),
                   params.end(),
                   std::back_inserter(all_params),
                   [](const RootParam &p) { return p.param; });

    D3D12_ROOT_SIGNATURE_DESC root_desc = {0};
    root_desc.NumParameters = all_params.size();
    root_desc.pParameters = all_params.data();
    root_desc.Flags = flags;

    // Create the root signature from the descriptor
    ComPtr<ID3DBlob> signature_blob;
    ComPtr<ID3DBlob> err_blob;
    auto res = D3D12SerializeRootSignature(
        &root_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &err_blob);
    if (FAILED(res)) {
        std::cout << "Failed to serialize root signature: "
                  << static_cast<char *>(err_blob->GetBufferPointer()) << "\n";
        throw std::runtime_error("Failed to serialize root signature");
    }

    ComPtr<ID3D12RootSignature> signature;
    CHECK_ERR(device->CreateRootSignature(0,
                                          signature_blob->GetBufferPointer(),
                                          signature_blob->GetBufferSize(),
                                          IID_PPV_ARGS(&signature)));

    return std::make_shared<RootSignature>(flags, signature, params);
}

RootSignature::RootSignature(D3D12_ROOT_SIGNATURE_FLAGS flags,
                             Microsoft::WRL::ComPtr<ID3D12RootSignature> sig,
                             const std::vector<RootParam> &params)
    : flags(flags), sig(sig)
{
    total_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
    for (const auto &ip : params) {
        RootParam p = ip;
        p.offset = total_size;
        if (p.param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS) {
            // Constants must pad to a size multiple of 8 to align w/ the pointer
            // entries
            p.size = align_to(p.param.Constants.Num32BitValues * 4,
                              sizeof(D3D12_GPU_DESCRIPTOR_HANDLE));
        } else {
            p.size = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);
        }
        param_offsets[p.name] = p;
        total_size += p.size;
    }
}

size_t RootSignature::offset(const std::string &name) const
{
    auto fnd = param_offsets.find(name);
    if (fnd != param_offsets.end()) {
        return fnd->second.offset;
    } else {
        throw std::runtime_error("Request for offset to " + name +
                                 " which is not in the root signature");
        return std::numeric_limits<size_t>::max();
    }
}

size_t RootSignature::size(const std::string &name) const
{
    auto fnd = param_offsets.find(name);
    if (fnd != param_offsets.end()) {
        return fnd->second.size;
    } else {
        return std::numeric_limits<size_t>::max();
    }
}

size_t RootSignature::get_total_size() const
{
    return total_size;
}

ID3D12RootSignature *RootSignature::operator->()
{
    return get();
}

ID3D12RootSignature *RootSignature::get()
{
    return sig.Get();
}
}
}
