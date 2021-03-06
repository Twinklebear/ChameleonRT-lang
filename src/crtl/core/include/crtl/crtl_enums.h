#pragma once

enum CRTL_DATA_TYPE {
    CRTL_DATA_TYPE_UNKNOWN,
    CRTL_DATA_TYPE_BOOL,
    CRTL_DATA_TYPE_BOOL1,
    CRTL_DATA_TYPE_BOOL2,
    CRTL_DATA_TYPE_BOOL3,
    CRTL_DATA_TYPE_BOOL4,
    CRTL_DATA_TYPE_BOOL1X1,
    CRTL_DATA_TYPE_BOOL2X1,
    CRTL_DATA_TYPE_BOOL3X1,
    CRTL_DATA_TYPE_BOOL4X1,
    CRTL_DATA_TYPE_BOOL1X2,
    CRTL_DATA_TYPE_BOOL2X2,
    CRTL_DATA_TYPE_BOOL3X2,
    CRTL_DATA_TYPE_BOOL4X2,
    CRTL_DATA_TYPE_BOOL1X3,
    CRTL_DATA_TYPE_BOOL2X3,
    CRTL_DATA_TYPE_BOOL3X3,
    CRTL_DATA_TYPE_BOOL4X3,
    CRTL_DATA_TYPE_BOOL1X4,
    CRTL_DATA_TYPE_BOOL2X4,
    CRTL_DATA_TYPE_BOOL3X4,
    CRTL_DATA_TYPE_BOOL4X4,
    CRTL_DATA_TYPE_INT,
    CRTL_DATA_TYPE_INT1,
    CRTL_DATA_TYPE_INT2,
    CRTL_DATA_TYPE_INT3,
    CRTL_DATA_TYPE_INT4,
    CRTL_DATA_TYPE_INT1X1,
    CRTL_DATA_TYPE_INT2X1,
    CRTL_DATA_TYPE_INT3X1,
    CRTL_DATA_TYPE_INT4X1,
    CRTL_DATA_TYPE_INT1X2,
    CRTL_DATA_TYPE_INT2X2,
    CRTL_DATA_TYPE_INT3X2,
    CRTL_DATA_TYPE_INT4X2,
    CRTL_DATA_TYPE_INT1X3,
    CRTL_DATA_TYPE_INT2X3,
    CRTL_DATA_TYPE_INT3X3,
    CRTL_DATA_TYPE_INT4X3,
    CRTL_DATA_TYPE_INT1X4,
    CRTL_DATA_TYPE_INT2X4,
    CRTL_DATA_TYPE_INT3X4,
    CRTL_DATA_TYPE_INT4X4,
    CRTL_DATA_TYPE_UINT,
    CRTL_DATA_TYPE_UINT1,
    CRTL_DATA_TYPE_UINT2,
    CRTL_DATA_TYPE_UINT3,
    CRTL_DATA_TYPE_UINT4,
    CRTL_DATA_TYPE_UINT1X1,
    CRTL_DATA_TYPE_UINT2X1,
    CRTL_DATA_TYPE_UINT3X1,
    CRTL_DATA_TYPE_UINT4X1,
    CRTL_DATA_TYPE_UINT1X2,
    CRTL_DATA_TYPE_UINT2X2,
    CRTL_DATA_TYPE_UINT3X2,
    CRTL_DATA_TYPE_UINT4X2,
    CRTL_DATA_TYPE_UINT1X3,
    CRTL_DATA_TYPE_UINT2X3,
    CRTL_DATA_TYPE_UINT3X3,
    CRTL_DATA_TYPE_UINT4X3,
    CRTL_DATA_TYPE_UINT1X4,
    CRTL_DATA_TYPE_UINT2X4,
    CRTL_DATA_TYPE_UINT3X4,
    CRTL_DATA_TYPE_UINT4X4,
    CRTL_DATA_TYPE_FLOAT,
    CRTL_DATA_TYPE_FLOAT1,
    CRTL_DATA_TYPE_FLOAT2,
    CRTL_DATA_TYPE_FLOAT3,
    CRTL_DATA_TYPE_FLOAT4,
    CRTL_DATA_TYPE_FLOAT1X1,
    CRTL_DATA_TYPE_FLOAT2X1,
    CRTL_DATA_TYPE_FLOAT3X1,
    CRTL_DATA_TYPE_FLOAT4X1,
    CRTL_DATA_TYPE_FLOAT1X2,
    CRTL_DATA_TYPE_FLOAT2X2,
    CRTL_DATA_TYPE_FLOAT3X2,
    CRTL_DATA_TYPE_FLOAT4X2,
    CRTL_DATA_TYPE_FLOAT1X3,
    CRTL_DATA_TYPE_FLOAT2X3,
    CRTL_DATA_TYPE_FLOAT3X3,
    CRTL_DATA_TYPE_FLOAT4X3,
    CRTL_DATA_TYPE_FLOAT1X4,
    CRTL_DATA_TYPE_FLOAT2X4,
    CRTL_DATA_TYPE_FLOAT3X4,
    CRTL_DATA_TYPE_FLOAT4X4,
    CRTL_DATA_TYPE_DOUBLE,
    CRTL_DATA_TYPE_DOUBLE1,
    CRTL_DATA_TYPE_DOUBLE2,
    CRTL_DATA_TYPE_DOUBLE3,
    CRTL_DATA_TYPE_DOUBLE4,
    CRTL_DATA_TYPE_DOUBLE1X1,
    CRTL_DATA_TYPE_DOUBLE2X1,
    CRTL_DATA_TYPE_DOUBLE3X1,
    CRTL_DATA_TYPE_DOUBLE4X1,
    CRTL_DATA_TYPE_DOUBLE1X2,
    CRTL_DATA_TYPE_DOUBLE2X2,
    CRTL_DATA_TYPE_DOUBLE3X2,
    CRTL_DATA_TYPE_DOUBLE4X2,
    CRTL_DATA_TYPE_DOUBLE1X3,
    CRTL_DATA_TYPE_DOUBLE2X3,
    CRTL_DATA_TYPE_DOUBLE3X3,
    CRTL_DATA_TYPE_DOUBLE4X3,
    CRTL_DATA_TYPE_DOUBLE1X4,
    CRTL_DATA_TYPE_DOUBLE2X4,
    CRTL_DATA_TYPE_DOUBLE3X4,
    CRTL_DATA_TYPE_DOUBLE4X4,
    // TODO: For params, buffer should be bufferview
    CRTL_DATA_TYPE_BUFFER,
    CRTL_DATA_TYPE_RWBUFFER,
    CRTL_DATA_TYPE_TEXTURE,
    CRTL_DATA_TYPE_RWTEXTURE,
    CRTL_DATA_TYPE_ACCELERATION_STRUCTURE,
    CRTL_DATA_TYPE_RAY,
    CRTL_DATA_TYPE_STRUCT,
};

enum CRTL_DEVICE_API {
    CRTL_DEVICE_API_UNKNOWN,
    CRTL_DEVICE_API_DX12,
    CRTL_DEVICE_API_VULKAN,
    CRTL_DEVICE_API_METAL,
    CRTL_DEVICE_API_OPTIX,
    CRTL_DEVICE_API_EMBREE
};

/* The MemorySpace defines where a buffer's memory is located.
 * - Device: memory is on the GPU and not visible to the CPU (TODO: Maybe with resizable
 * BAR, should still make it CPU visible?)
 *
 * - Upload: memory is on the CPU and is visible to both the GPU and CPU.
 *
 * - Readback: memory is on the GPU and readable from the CPU
 */
enum CRTL_MEMORY_SPACE {
    CRTL_MEMORY_SPACE_DEVICE,
    CRTL_MEMORY_SPACE_UPLOAD,
    CRTL_MEMORY_SPACE_READBACK,
};

enum CRTL_TEXTURE_TYPE {
    CRTL_TEXTURE_TYPE_1D,
    CRTL_TEXTURE_TYPE_2D,
    CRTL_TEXTURE_TYPE_3D,
    // TODO: texture arrays, cubemaps, etc?
};

enum CRTL_IMAGE_FORMAT {
    CRTL_IMAGE_FORMAT_R8G8B8A8_UNORM,
    CRTL_IMAGE_FORMAT_R8G8B8A8_UNORM_SRGB,
    CRTL_IMAGE_FORMAT_R32G32B32A32_FLOAT,
    // TODO: fill in the rest of the texture types
};

/* Buffer usage modes that buffers can be created supporting
 * The set of modes can be bitwise OR'd together
 */
enum CRTL_BUFFER_USAGE {
    CRTL_BUFFER_USAGE_COPY_SRC = 0x00000001,
    CRTL_BUFFER_USAGE_COPY_DST = 0x00000002,
    CRTL_BUFFER_USAGE_SHADER_READ = 0x00000004,
    CRTL_BUFFER_USAGE_SHADER_READ_WRITE = 0x00000008,
    CRTL_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT = 0x00000010,
    // TODO: Expose the AS storage buffer and SBT storage buffer?
};

/* Image usage modes that images can be created supporting
 * The set of modes can be bitwise OR'd together
 */
enum CRTL_IMAGE_USAGE {
    CRTL_IMAGE_USAGE_COPY_SRC = 0x00000001,
    CRTL_IMAGE_USAGE_COPY_DST = 0x00000002,
    CRTL_IMAGE_USAGE_SHADER_SAMPLED = 0x00000004,
    CRTL_IMAGE_USAGE_SHADER_READ_WRITE = 0x00000008,
};

enum CRTL_BARRIER_STAGE {
    CRTL_BARRIER_STAGE_RAYTRACING = 0x00000001,
    CRTL_BARRIER_STAGE_COPY = 0x00000002,
    CRTL_BARRIER_STAGE_BUILD_ACCELERATION_STRUCTURE = 0x00000004,
    CRTL_BARRIER_STAGE_ALL = 0xffffffff,
};

enum CRTL_BARRIER_ACCESS {
    CRTL_BARRIER_ACCESS_NONE = 0x0,
    CRTL_BARRIER_ACCESS_SHADER_READ = 0x00000001,
    CRTL_BARRIER_ACCESS_SHADER_WRITE = 0x00000002,
    CRTL_BARRIER_ACCESS_COPY_DST = 0x00000004,
    CRTL_BARRIER_ACCESS_COPY_SRC = 0x00000004,
    CRTL_BARRIER_ACCESS_ALL = 0xffffffff,
};

enum CRTL_GEOMETRY_FLAG {
    CRTL_GEOMETRY_FLAG_NONE = 0,
    CRTL_GEOMETRY_FLAG_ENABLE_ANY_HIT = 0x1,
};

enum CRTL_INSTANCE_FLAG {
    CRTL_INSTANGE_FLAG_NONE = 0,
    CRTL_INSTANCE_FLAG_ENABLE_ANY_HIT = 0x1,
    CRTL_INSTANCE_FLAG_DISABLE_ANY_HIT = 0x2,
};

enum CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG {
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE = 0,
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE = 0x1,
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION = 0x2,
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE = 0x4,
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD = 0x8,
    CRTL_ACCELERATION_STRUCTURE_BUILD_FLAG_MINIMIZE_MEMORY = 0x8,
};

enum CRTL_ERROR {
    CRTL_ERROR_NONE = 0,
    CRTL_ERROR_BACKEND_LOADING_FAILED,
    CRTL_ERROR_OBJECT_NULL,
    CRTL_ERROR_INVALID_OBJECT,
    CRTL_ERROR_UNKNOWN = 0xffffffff
};
