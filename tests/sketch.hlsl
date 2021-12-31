struct HitInfo {
    float3 color;
};

struct CameraParams {
    // Constants for global params would be packed into a UBO or SBO
    float3 cam_pos;
    float3 cam_du;
    float3 cam_dv;
    float3 cam_dir_top_left;
};

/*
struct SceneParams {
    // AS's, textures, buffers, would be passed through as global params
    AccelerationStructure scene;
    // It looks like HLSL doesn't distinguish writeonly/readonly vs. readwrite,
    // so it's simplest to adopt the HLSL naming here and do either RWTexture2D
    // or RWBuffer, meaning readwrite, or Texture2D, Buffer, meaning read only.
    RWTexture2D<float4> image;
};
*/

// SBT params will need some thought to support
/*
struct RayGenParams {
    uint num_lights;
};
*/

// Raygen and global params can be put into a uniform or storage buffer for GLSL,
// and similar in HLSL to make these into globally accessible parameters.
// Also generate a struct that corresponds to the global params to make it
// easier to set them.
// There can be multiple global params, and they can be a mix of constants and
// AS's, textures, buffers.
// in SceneParams scene;
RayTracingAccelerationStructure scene_scene : register(t0);
RWTexture2D<float4> scene_image : register(u0);

// in CameraParams camera;
cbuffer CameraParams_b0_s0 : register(b0)
{
    CameraParams camera;
}

const int x = 5;

// TODO change this to match small.hlsl
cbuffer RayGenParams : register(b0, space1)
{
    uint32_t RayGen_params_num_lights;
}

[shader("raygeneration")]
void RayGen() {
    uint2 pixel = DispatchRaysIndex().xy;
    RayDesc ray;
    // ray set up
    HitInfo hit_info;
    TraceRay(
        scene_scene, RAY_FLAG_FORCE_OPAQUE, 0xff, 0, 1, 0, ray, hit_info);
    scene_image[pixel] = hit_info.color;
}

// A test standalone shader function that takes a buffer
float3 get_value(in StructuredBuffer<float3> b, const int i)
{
    // TODO: would be better to not be so pessimistic about putting this marker in
    return b[NonUniformResourceIndex(i)];
}

// Params to closest_hit/any_hit/miss shaders would be put into SBT
// Need C struct generated to match these up for convenience in setting
// params on the host, need to know size this struct will be.
struct ClosestHitParams {
    StructuredBuffer<uint3> indices;
    StructuredBuffer<float3> positions;
};

StructuredBuffer<uint3> ClosestHit_hit_info_indices : register(t0, space2);
StructuredBuffer<float3> ClosestHit_hit_info_positions : register(t1, space2);

[shader("closesthit")]
void ClosestHit(inout HitInfo hit_info : SV_RayPayload, Attributes attribs) {
    ClosestHitParams params;
    params.indices = ClosestHit_hit_info_indices;
    params.positions = ClosestHit_hit_info_positions;

    const uint3 idx = params.indices[NonUniformResourceIndex(PrimitiveIndex())];
    const float3 va = get_value(params.positions, idx.x);
    const float3 vb = get_value(params.positions, idx.y);
    const float3 vc = get_value(params.positions, idx.z);
    const float3 normal = normalize(cross(vb - va, vc - va));
    hit_info.color = (normal + 1.f) * 0.5f;
}

struct MissParams {
    float3 background_color;
};

cbuffer MissConstants : register(b0, space3) {
    float3 Miss_params_background_color;
}

[shader("miss")]
void Miss(inout HitInfo hit_info : SV_RayPayload)
{
    MissParams params;
    params.background_color = Miss_params_background_color;

    hit_info.color = params.background_color;
}

