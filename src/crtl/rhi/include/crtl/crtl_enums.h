#pragma once

enum CRTL_DATA_TYPE {
    CRTL_TYPE_BOOL,
    CRTL_TYPE_BOOL1,
    CRTL_TYPE_BOOL2,
    CRTL_TYPE_BOOL3,
    CRTL_TYPE_BOOL4,
    CRTL_TYPE_BOOL1X1,
    CRTL_TYPE_BOOL2X1,
    CRTL_TYPE_BOOL3X1,
    CRTL_TYPE_BOOL4X1,
    CRTL_TYPE_BOOL1X2,
    CRTL_TYPE_BOOL2X2,
    CRTL_TYPE_BOOL3X2,
    CRTL_TYPE_BOOL4X2,
    CRTL_TYPE_BOOL1X3,
    CRTL_TYPE_BOOL2X3,
    CRTL_TYPE_BOOL3X3,
    CRTL_TYPE_BOOL4X3,
    CRTL_TYPE_BOOL1X4,
    CRTL_TYPE_BOOL2X4,
    CRTL_TYPE_BOOL3X4,
    CRTL_TYPE_BOOL4X4,
    CRTL_TYPE_INT,
    CRTL_TYPE_INT1,
    CRTL_TYPE_INT2,
    CRTL_TYPE_INT3,
    CRTL_TYPE_INT4,
    CRTL_TYPE_INT1X1,
    CRTL_TYPE_INT2X1,
    CRTL_TYPE_INT3X1,
    CRTL_TYPE_INT4X1,
    CRTL_TYPE_INT1X2,
    CRTL_TYPE_INT2X2,
    CRTL_TYPE_INT3X2,
    CRTL_TYPE_INT4X2,
    CRTL_TYPE_INT1X3,
    CRTL_TYPE_INT2X3,
    CRTL_TYPE_INT3X3,
    CRTL_TYPE_INT4X3,
    CRTL_TYPE_INT1X4,
    CRTL_TYPE_INT2X4,
    CRTL_TYPE_INT3X4,
    CRTL_TYPE_INT4X4,
    CRTL_TYPE_UINT,
    CRTL_TYPE_UINT1,
    CRTL_TYPE_UINT2,
    CRTL_TYPE_UINT3,
    CRTL_TYPE_UINT4,
    CRTL_TYPE_UINT1X1,
    CRTL_TYPE_UINT2X1,
    CRTL_TYPE_UINT3X1,
    CRTL_TYPE_UINT4X1,
    CRTL_TYPE_UINT1X2,
    CRTL_TYPE_UINT2X2,
    CRTL_TYPE_UINT3X2,
    CRTL_TYPE_UINT4X2,
    CRTL_TYPE_UINT1X3,
    CRTL_TYPE_UINT2X3,
    CRTL_TYPE_UINT3X3,
    CRTL_TYPE_UINT4X3,
    CRTL_TYPE_UINT1X4,
    CRTL_TYPE_UINT2X4,
    CRTL_TYPE_UINT3X4,
    CRTL_TYPE_UINT4X4,
    CRTL_TYPE_FLOAT,
    CRTL_TYPE_FLOAT1,
    CRTL_TYPE_FLOAT2,
    CRTL_TYPE_FLOAT3,
    CRTL_TYPE_FLOAT4,
    CRTL_TYPE_FLOAT1X1,
    CRTL_TYPE_FLOAT2X1,
    CRTL_TYPE_FLOAT3X1,
    CRTL_TYPE_FLOAT4X1,
    CRTL_TYPE_FLOAT1X2,
    CRTL_TYPE_FLOAT2X2,
    CRTL_TYPE_FLOAT3X2,
    CRTL_TYPE_FLOAT4X2,
    CRTL_TYPE_FLOAT1X3,
    CRTL_TYPE_FLOAT2X3,
    CRTL_TYPE_FLOAT3X3,
    CRTL_TYPE_FLOAT4X3,
    CRTL_TYPE_FLOAT1X4,
    CRTL_TYPE_FLOAT2X4,
    CRTL_TYPE_FLOAT3X4,
    CRTL_TYPE_FLOAT4X4,
    CRTL_TYPE_DOUBLE,
    CRTL_TYPE_DOUBLE1,
    CRTL_TYPE_DOUBLE2,
    CRTL_TYPE_DOUBLE3,
    CRTL_TYPE_DOUBLE4,
    CRTL_TYPE_DOUBLE1X1,
    CRTL_TYPE_DOUBLE2X1,
    CRTL_TYPE_DOUBLE3X1,
    CRTL_TYPE_DOUBLE4X1,
    CRTL_TYPE_DOUBLE1X2,
    CRTL_TYPE_DOUBLE2X2,
    CRTL_TYPE_DOUBLE3X2,
    CRTL_TYPE_DOUBLE4X2,
    CRTL_TYPE_DOUBLE1X3,
    CRTL_TYPE_DOUBLE2X3,
    CRTL_TYPE_DOUBLE3X3,
    CRTL_TYPE_DOUBLE4X3,
    CRTL_TYPE_DOUBLE1X4,
    CRTL_TYPE_DOUBLE2X4,
    CRTL_TYPE_DOUBLE3X4,
    CRTL_TYPE_DOUBLE4X4,
    // TODO: For params, buffer should be bufferview
    CRTL_TYPE_BUFFER,
    CRTL_TYPE_RWBUFFER,
    CRTL_TYPE_TEXTURE,
    CRTL_TYPE_RWTEXTURE,
    CRTL_TYPE_ACCELERATION_STRUCTURE,
    CRTL_TYPE_RAY,
    CRTL_TYPE_STRUCT,
    CRTL_TYPE_UNKNOWN
};

enum DEVICE_API { DX12, VULKAN, METAL, OPTIX, EMBREE, UNKNOWN };

/* The MemorySpace defines where a buffer's memory is located.
 * - Device: memory is on the GPU and not visible to the CPU (TODO: Maybe with resizable
 * BAR, should still make it CPU visible?)
 *
 * - Upload: memory is on the CPU and is visible to both the GPU and CPU.
 *
 * - Readback: memory is on the GPU and readable from the CPU
 */
enum CRTL_MEMORY_SPACE {
    CRTL_MEMORY_DEVICE,
    CRTL_MEMORY_UPLOAD,
    CRTL_MEMORY_READBACK,
};

enum CRTL_TEXTURE_TYPE {
    CRTL_TEXTURE_1D,
    CRTL_TEXTURE_2D,
    CRTL_TEXTURE_3D,
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
    CRTL_BUFFER_COPY_SRC = 0x00000001,
    CRTL_BUFFER_COPY_DST = 0x00000002,
    CRTL_BUFFER_SHADER_READ = 0x00000004,
    CRTL_BUFFER_SHADER_READ_WRITE = 0x00000008,
    CRTL_BUFFER_ACCELERATION_STRUCTURE_BUILD_INPUT = 0x00000010,
    // TODO: Expose the AS storage buffer and SBT storage buffer?
};

/* Image usage modes that images can be created supporting
 * The set of modes can be bitwise OR'd together
 */
enum CRTL_IMAGE_USAGE {
    CRTL_IMAGE_COPY_SRC = 0x00000001,
    CRTL_IMAGE_COPY_DST = 0x00000002,
    CRTL_IMAGE_SHADER_SAMPLED = 0x00000004,
    CRTL_IMAGE_SHADER_READ_WRITE = 0x00000008,
};

enum CRTL_ERROR { CRTL_ERROR_NONE = 0, CRTL_ERROR_UNKNOWN = 0xffffffff };
