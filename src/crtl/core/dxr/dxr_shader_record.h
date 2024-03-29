#pragma once

#include <string>
#include <vector>
#include "api_object.h"
#include "crtl_dxr_export.h"
#include "dxr_root_signature.h"
#include "dxr_shader_entry_point.h"
#include "dxr_utils.h"

namespace crtl {
namespace dxr {

class ShaderRecordParameterBlock;

class CRTL_DXR_EXPORT ShaderRecord : public APIObject {
protected:
    std::string shader_record_name;

    std::shared_ptr<ShaderRecordParameterBlock> bound_parameter_block;

public:
    virtual ~ShaderRecord() = default;

    // TODO: Some method should be provided here to get the local root signature and root
    // signature association object for a shader record renderables/pipelines etc. this
    // will also provide info to the parameter block.
    // Note that the layout doesn't need to match the order they appear in the shader
    // record list, constants should be packed up front to minimize padding
    // so different constants accessed by different entry points in a hit group can be put
    // together, and the local root signature ordered to map them to the right registers
    // TODO: Later on do not use JSON for this
    // What it needs is a parameter descriptor, the block takes a list of these in a map
    // by their name so that set param can map to the right location
    // or really, a vector of the param info + a map of name -> index

    virtual const phmap::flat_hash_map<std::string, ShaderParameterDesc>
        &get_parameter_info() const = 0;

    virtual const RootSignature *get_root_signature() const = 0;

    size_t get_parameter_block_size() const;

    void set_parameter_block(
        const std::shared_ptr<ShaderRecordParameterBlock> &parameter_block);
};

// TODO WILL: Hit Group records will be changed in the language/compiler/API to
// better express the SBT -> shaders mapping
class CRTL_DXR_EXPORT HitGroupRecord : public ShaderRecord {
    // TODO: Maybe make it atomic? for some thread safe support of creating records/etc?
    // IDs are tracked per record type rather than globally
    inline static uint32_t next_record_id = 0;

    std::shared_ptr<ShaderEntryPoint> closest_hit, any_hit, intersection;

    D3D12_HIT_GROUP_TYPE hg_type;

public:
    // Intersection and any hit are optional, can pass nullptr to not have them
    // TODO: After code syntax for HG changes this will not take a bunch of entry points
    // but a "hit group definition" or somethign that has the root sig/params/etc.
    HitGroupRecord(const std::shared_ptr<ShaderEntryPoint> &closest_hit,
                   const std::shared_ptr<ShaderEntryPoint> &intersection,
                   const std::shared_ptr<ShaderEntryPoint> &any_hit);

    virtual const phmap::flat_hash_map<std::string, ShaderParameterDesc>
        &get_parameter_info() const override;

    virtual const RootSignature *get_root_signature() const override;
};

class CRTL_DXR_EXPORT MissRecord : public ShaderRecord {
    inline static uint32_t next_record_id = 0;

    std::shared_ptr<ShaderEntryPoint> entry_point;

public:
    MissRecord(const std::shared_ptr<ShaderEntryPoint> &entry_point);

    virtual const phmap::flat_hash_map<std::string, ShaderParameterDesc>
        &get_parameter_info() const override;

    virtual const RootSignature *get_root_signature() const override;
};

class CRTL_DXR_EXPORT RaygenRecord : public ShaderRecord {
    inline static uint32_t next_record_id = 0;

    std::shared_ptr<ShaderEntryPoint> entry_point;

public:
    RaygenRecord(const std::shared_ptr<ShaderEntryPoint> &entry_point);

    virtual const phmap::flat_hash_map<std::string, ShaderParameterDesc>
        &get_parameter_info() const override;

    virtual const RootSignature *get_root_signature() const override;
};
}
}
