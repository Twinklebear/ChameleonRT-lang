// TODO: Note: Each entry point would be output to separate files with some
// repetition of the shared types in the top of the file in each

// Shared definitions:
struct HitInfo {
    float3 color;
};

struct CameraParams {
    // Constants for global params would be packed into a UBO or SBO
    vec3 cam_pos;
    vec3 cam_du;
    vec3 cam_dv;
    vec3 cam_dir_top_left;
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

// Raygen and global params can be put into a uniform or storage buffer for GLSL,
// and similar in HLSL to make these into globally accessible parameters.
// Also generate a struct that corresponds to the global params to make it
// easier to set them.
// There can be multiple global params, and they can be a mix of constants and
// AS's, textures, buffers.
// in SceneParams scene;
layout(binding = 0, set = 0) uniform accelerationStructureEXT scene_scene;
layout(binding = 1, set = 0) uniform image2D scene_image;

// in CameraParams camera;
layout(binding = 2, set = 0, scalar) uniform CameraParams_b2_s0
{
    CameraParams camera;
};

const int x = 5;

// Shared buffer reference types
layout(buffer_reference, buffer_reference_align = 8, scalar) buffer Buffer_uint3
{
    uvec3 data[];
};

layout(buffer_reference, buffer_reference_align = 8, scalar) buffer Buffer_float3
{
    vec3 data[];
};

// And the ray payloads which must be globally visible here
layout(location = 0) rayPayloadInEXT HitInfo hit_info;

// A test standalone shader function that takes a buffer
vec3 get_value(in Buffer_float3 b, const int i)
{
    return b.data[i];
}

// --- end shared definitions -----

// ------
// Output raygen shader file
// ------
layout(shaderRecordEXT, std430) buffer RayGen_SBT_Data
{
    uint RayGen_params_num_lights;
};

void main()
{
    uvec2 pixel = gl_LaunchIDEXT.xy;
    Ray ray;
    traceRayEXT(scene_scene,
                gl_RayFlagsOpaqueEXT,
                0xff,
                0,
                1,
                0,
                ray.origin,
                ray.t_min,
                ray.dir,
                ray.t_max,
                0);

    // Note: I think a conversion is needed here to ivec2 for imageStore
    imageStore(scene_image, pixel, hit_info.color);
}

// ------
// Output hit shader file
// ------
layout(shaderRecordEXT, std430) buffer ClosestHit_SBT_Data
{
    Buffer_uint3 params_indices;
    Buffer_float3 params_positions;
};

void main()
{
    ClosestHitParams params;
    params.indices = indices;
    params.positions = positions;

    const uvec3 idx = params.indices[gl_PrimitiveID];
    const vec3 va = get_value(params.positions, idx.x);
    const vec3 vb = get_value(params.positions, idx.y);
    const vec3 vc = get_value(params.positions, idx.z);
    const vec3 normal = normalize(cross(vb - va, vc - va));
    hit_info.color = (normal + 1.f) * 0.5f;
}

// ------
// Output miss shader file
// ------
layout(shaderRecordEXT, std430) buffer Miss_SBT_Data
{
    vec3 params_background_color;
};

void main()
{
    MissParams params;
    params.background_color = params_background_color;

    hit_info.color = params.background_color;
}

