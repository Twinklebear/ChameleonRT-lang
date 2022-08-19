#include <algorithm>
#include <array>
#include <codecvt>
#include <locale>
#ifdef _WIN32
#include <intrin.h>
#elif not defined(__aarch64__)
#include <cpuid.h>
#endif
#include <glm/ext.hpp>
#include "util.h"

namespace crtl {

CRTL_EXPORT std::string pretty_print_count(const double count)
{
    const double giga = 1000000000;
    const double mega = 1000000;
    const double kilo = 1000;
    if (count > giga) {
        return std::to_string(count / giga) + " G";
    } else if (count > mega) {
        return std::to_string(count / mega) + " M";
    } else if (count > kilo) {
        return std::to_string(count / kilo) + " K";
    }
    return std::to_string(count);
}

CRTL_EXPORT uint64_t align_to(uint64_t val, uint64_t align)
{
    return ((val + align - 1) / align) * align;
}

CRTL_EXPORT void ortho_basis(glm::vec3 &v_x, glm::vec3 &v_y, const glm::vec3 &n)
{
    v_y = glm::vec3(0);

    if (n.x < 0.6f && n.x > -0.6f) {
        v_y.x = 1.f;
    } else if (n.y < 0.6f && n.y > -0.6f) {
        v_y.y = 1.f;
    } else if (n.z < 0.6f && n.z > -0.6f) {
        v_y.z = 1.f;
    } else {
        v_y.x = 1.f;
    }
    v_x = glm::normalize(glm::cross(v_y, n));
    v_y = glm::normalize(glm::cross(n, v_x));
}

CRTL_EXPORT void canonicalize_path(std::string &path)
{
    std::replace(path.begin(), path.end(), '\\', '/');
}

CRTL_EXPORT std::string get_file_extension(const std::string &fname)
{
    const size_t fnd = fname.find_last_of('.');
    if (fnd == std::string::npos) {
        return "";
    }
    return fname.substr(fnd + 1);
}

CRTL_EXPORT std::string get_cpu_brand()
{
#if defined(__APPLE__) && defined(__aarch64__)
    return "Apple M1";
#else
    std::string brand = "Unspecified";
    std::array<int32_t, 4> regs;
#ifdef _WIN32
    __cpuid(regs.data(), 0x80000000);
#else
    __cpuid(0x80000000, regs[0], regs[1], regs[2], regs[3]);
#endif
    if (regs[0] >= 0x80000004) {
        char b[64] = {0};
        for (int i = 0; i < 3; ++i) {
#ifdef _WIN32
            __cpuid(regs.data(), 0x80000000 + i + 2);
#else
            __cpuid(0x80000000 + i + 2, regs[0], regs[1], regs[2], regs[3]);
#endif
            std::memcpy(b + i * sizeof(regs), regs.data(), sizeof(regs));
        }
        brand = b;
    }
    return brand;
#endif
}

CRTL_EXPORT float srgb_to_linear(float x)
{
    if (x <= 0.04045f) {
        return x / 12.92f;
    }
    return std::pow((x + 0.055f) / 1.055f, 2.4);
}

CRTL_EXPORT float linear_to_srgb(float x)
{
    if (x <= 0.0031308f) {
        return 12.92f * x;
    }
    return 1.055f * pow(x, 1.f / 2.4f) - 0.055f;
}

CRTL_EXPORT float luminance(const glm::vec3 &c)
{
    return 0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z;
}

CRTL_EXPORT std::wstring utf8_to_utf16(const std::string &utf8)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8);
}

CRTL_EXPORT std::string utf16_to_utf8(const std::wstring &utf16)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(utf16);
}

CRTL_EXPORT size_t data_type_size(CRTL_DATA_TYPE type)
{
    // Note: Bool is same size as int for GPU
    switch (type) {
    case CRTL_DATA_TYPE_INT:
    case CRTL_DATA_TYPE_UINT:
    case CRTL_DATA_TYPE_FLOAT:
        return 4;

    case CRTL_DATA_TYPE_INT2:
    case CRTL_DATA_TYPE_INT2X1:
    case CRTL_DATA_TYPE_INT1X2:

    case CRTL_DATA_TYPE_UINT2:
    case CRTL_DATA_TYPE_UINT2X1:
    case CRTL_DATA_TYPE_UINT1X2:

    case CRTL_DATA_TYPE_FLOAT2:
    case CRTL_DATA_TYPE_FLOAT2X1:
    case CRTL_DATA_TYPE_FLOAT1X2:
        return 8;

    case CRTL_DATA_TYPE_INT3:
    case CRTL_DATA_TYPE_INT3X1:
    case CRTL_DATA_TYPE_INT1X3:

    case CRTL_DATA_TYPE_UINT3:
    case CRTL_DATA_TYPE_UINT3X1:
    case CRTL_DATA_TYPE_UINT1X3:

    case CRTL_DATA_TYPE_FLOAT3:
    case CRTL_DATA_TYPE_FLOAT3X1:
    case CRTL_DATA_TYPE_FLOAT1X3:
        return 12;

    case CRTL_DATA_TYPE_INT4:
    case CRTL_DATA_TYPE_INT4X1:
    case CRTL_DATA_TYPE_INT1X4:

    case CRTL_DATA_TYPE_UINT4:
    case CRTL_DATA_TYPE_UINT4X1:
    case CRTL_DATA_TYPE_UINT1X4:

    case CRTL_DATA_TYPE_FLOAT4:
    case CRTL_DATA_TYPE_FLOAT4X1:
    case CRTL_DATA_TYPE_FLOAT1X4:
        return 16;

    case CRTL_DATA_TYPE_INT2X2:
    case CRTL_DATA_TYPE_UINT2X2:
    case CRTL_DATA_TYPE_FLOAT2X2:
        return 16;

    case CRTL_DATA_TYPE_INT2X3:
    case CRTL_DATA_TYPE_INT3X2:

    case CRTL_DATA_TYPE_UINT2X3:
    case CRTL_DATA_TYPE_UINT3X2:

    case CRTL_DATA_TYPE_FLOAT2X3:
    case CRTL_DATA_TYPE_FLOAT3X2:
        return 24;

    case CRTL_DATA_TYPE_INT4X2:
    case CRTL_DATA_TYPE_INT2X4:

    case CRTL_DATA_TYPE_UINT4X2:
    case CRTL_DATA_TYPE_UINT2X4:

    case CRTL_DATA_TYPE_FLOAT4X2:
    case CRTL_DATA_TYPE_FLOAT2X4:
        return 32;

    case CRTL_DATA_TYPE_INT3X3:
    case CRTL_DATA_TYPE_UINT3X3:
    case CRTL_DATA_TYPE_FLOAT3X3:
        return 36;

    case CRTL_DATA_TYPE_INT4X3:
    case CRTL_DATA_TYPE_INT3X4:

    case CRTL_DATA_TYPE_UINT4X3:
    case CRTL_DATA_TYPE_UINT3X4:

    case CRTL_DATA_TYPE_FLOAT4X3:
    case CRTL_DATA_TYPE_FLOAT3X4:
        return 48;

    case CRTL_DATA_TYPE_INT4X4:
    case CRTL_DATA_TYPE_UINT4X4:
    case CRTL_DATA_TYPE_FLOAT4X4:
        return 64;

    case CRTL_DATA_TYPE_DOUBLE:
        return 8;

    case CRTL_DATA_TYPE_DOUBLE2:
    case CRTL_DATA_TYPE_DOUBLE2X1:
    case CRTL_DATA_TYPE_DOUBLE1X2:
        return 16;

    case CRTL_DATA_TYPE_DOUBLE3:
    case CRTL_DATA_TYPE_DOUBLE3X1:
    case CRTL_DATA_TYPE_DOUBLE1X3:
        return 24;

    case CRTL_DATA_TYPE_DOUBLE4:
    case CRTL_DATA_TYPE_DOUBLE4X1:
    case CRTL_DATA_TYPE_DOUBLE1X4:
        return 32;

    case CRTL_DATA_TYPE_DOUBLE2X2:
        return 32;

    case CRTL_DATA_TYPE_DOUBLE3X2:
    case CRTL_DATA_TYPE_DOUBLE2X3:
        return 48;

    case CRTL_DATA_TYPE_DOUBLE4X2:
    case CRTL_DATA_TYPE_DOUBLE2X4:
        return 64;

    case CRTL_DATA_TYPE_DOUBLE3X3:
        return 72;

    case CRTL_DATA_TYPE_DOUBLE4X3:
    case CRTL_DATA_TYPE_DOUBLE3X4:
        return 96;

    case CRTL_DATA_TYPE_DOUBLE4X4:
        return 128;

    case CRTL_DATA_TYPE_BUFFER:
    case CRTL_DATA_TYPE_RWBUFFER:
    case CRTL_DATA_TYPE_TEXTURE:
    case CRTL_DATA_TYPE_RWTEXTURE:
    case CRTL_DATA_TYPE_ACCELERATION_STRUCTURE:
        return 8;
    default:
        return 0;
    }
}
}
