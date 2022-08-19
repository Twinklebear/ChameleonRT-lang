#include "type.h"
#include <cstring>
#include <stdexcept>

#include <iostream>

namespace crtl {
namespace ty {

Type::Type(const BaseType base_type) : base_type(base_type) {}

// Helper to parse the template type for Buffers and Textures
std::shared_ptr<Type> parse_template_element_type(const std::string &type_str)
{
    auto elem_str_start = type_str.find('<');
    auto elem_str_end = type_str.find('>');
    const std::string elem_str =
        type_str.substr(elem_str_start + 1, elem_str_end - elem_str_start - 1);
    return parse_type(elem_str);
}

void parse_vector_matrix_dimensionality(const std::string &type_str,
                                        uint32_t &dim_0,
                                        uint32_t &dim_1)
{
    const size_t len = type_str.size();
    if (!std::isdigit(type_str[len - 1])) {
        dim_0 = 0;
        dim_1 = 0;
        return;
    }

    // Matrices
    if (type_str[len - 2] == 'X') {
        dim_0 = type_str[len - 3] - '0';
        dim_1 = type_str[len - 1] - '0';
    } else {
        // Vectors
        dim_0 = type_str[len - 1] - '0';
        dim_1 = 0;
    }
}

std::shared_ptr<Type> parse_type(const std::string &type_str)
{
    if (type_str == "ACCELERATION_STRUCTURE") {
        return std::make_shared<AccelerationStructure>();
    } else if (type_str.find("BUFFER") != std::string::npos) {
        Access access = type_str[1] == 'W' ? Access::READ_WRITE : Access::READ_ONLY;
        auto element_type = parse_template_element_type(type_str);
        return std::make_shared<BufferView>(element_type, access);
    } else if (type_str.find("TEXTURE") != std::string::npos) {
        Access access = type_str[1] == 'W' ? Access::READ_WRITE : Access::READ_ONLY;
        auto element_type = parse_template_element_type(type_str);
        uint32_t dimensionality = type_str[access == Access::READ_WRITE ? 9 : 7] - '0';
        return std::make_shared<Texture>(element_type, access, dimensionality);
    }

    // It's either a matrix, vector or primitive type, so now try to read its
    // dimensionality to figure out which one
    uint32_t dim_0 = 0;
    uint32_t dim_1 = 0;
    parse_vector_matrix_dimensionality(type_str, dim_0, dim_1);
    if (type_str[0] == 'U') {
        // If both are non-zero it's a matrix
        if (dim_0 != 0 && dim_1 != 0) {
            return std::make_shared<Matrix>(PrimitiveType::UINT, dim_0, dim_1);
        } else if (dim_0 != 0) {
            // dim_0 != 0, it's a vector
            return std::make_shared<Vector>(PrimitiveType::UINT, dim_0);
        } else {
            // it's a single primitive
            return std::make_shared<Primitive>(PrimitiveType::UINT);
        }
    } else if (type_str[0] == 'I') {
        // If both are non-zero it's a matrix
        if (dim_0 != 0 && dim_1 != 0) {
            return std::make_shared<Matrix>(PrimitiveType::INT, dim_0, dim_1);
        } else if (dim_0 != 0) {
            // dim_0 != 0, it's a vector
            return std::make_shared<Vector>(PrimitiveType::INT, dim_0);
        } else {
            // it's a single primitive
            return std::make_shared<Primitive>(PrimitiveType::INT);
        }
    } else if (type_str[0] == 'F') {
        // If both are non-zero it's a matrix
        if (dim_0 != 0 && dim_1 != 0) {
            return std::make_shared<Matrix>(PrimitiveType::FLOAT, dim_0, dim_1);
        } else if (dim_0 != 0) {
            // dim_0 != 0, it's a vector
            return std::make_shared<Vector>(PrimitiveType::FLOAT, dim_0);
        } else {
            // it's a single primitive
            return std::make_shared<Primitive>(PrimitiveType::FLOAT);
        }
    } else if (type_str[0] == 'D') {
        // If both are non-zero it's a matrix
        if (dim_0 != 0 && dim_1 != 0) {
            return std::make_shared<Matrix>(PrimitiveType::DOUBLE, dim_0, dim_1);
        } else if (dim_0 != 0) {
            // dim_0 != 0, it's a vector
            return std::make_shared<Vector>(PrimitiveType::DOUBLE, dim_0);
        } else {
            // it's a single primitive
            return std::make_shared<Primitive>(PrimitiveType::DOUBLE);
        }
    }
}

Primitive::Primitive(const PrimitiveType primitive_type)
    : Type(BaseType::PRIMITIVE), primitive_type(primitive_type)
{
}

CRTL_DATA_TYPE Primitive::data_type() const
{
    switch (primitive_type) {
    case PrimitiveType::INT:
        return CRTL_DATA_TYPE_INT;
    case PrimitiveType::UINT:
        return CRTL_DATA_TYPE_UINT;
    case PrimitiveType::FLOAT:
        return CRTL_DATA_TYPE_FLOAT;
    case PrimitiveType::DOUBLE:
        return CRTL_DATA_TYPE_DOUBLE;
    default:
        return CRTL_DATA_TYPE_UNKNOWN;
    }
}

size_t Primitive::size() const
{
    switch (primitive_type) {
    case PrimitiveType::INT:
    case PrimitiveType::UINT:
    case PrimitiveType::FLOAT:
        return 4;
    case PrimitiveType::DOUBLE:
        return 8;
    default:
        return 0;
    }
}

Vector::Vector(const Primitive element_type, const uint32_t dimensionality)
    : Type(BaseType::VECTOR), element_type(element_type), dimensionality(dimensionality)
{
}

CRTL_DATA_TYPE Vector::data_type() const
{
    switch (element_type.primitive_type) {
    case PrimitiveType::INT:
        switch (dimensionality) {
        case 2:
            return CRTL_DATA_TYPE_INT2;
        case 3:
            return CRTL_DATA_TYPE_INT3;
        case 4:
            return CRTL_DATA_TYPE_INT4;
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    case PrimitiveType::UINT:
        switch (dimensionality) {
        case 2:
            return CRTL_DATA_TYPE_UINT2;
        case 3:
            return CRTL_DATA_TYPE_UINT3;
        case 4:
            return CRTL_DATA_TYPE_UINT4;
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    case PrimitiveType::FLOAT:
        switch (dimensionality) {
        case 2:
            return CRTL_DATA_TYPE_FLOAT2;
        case 3:
            return CRTL_DATA_TYPE_FLOAT3;
        case 4:
            return CRTL_DATA_TYPE_FLOAT4;
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    case PrimitiveType::DOUBLE:
        switch (dimensionality) {
        case 2:
            return CRTL_DATA_TYPE_DOUBLE2;
        case 3:
            return CRTL_DATA_TYPE_DOUBLE3;
        case 4:
            return CRTL_DATA_TYPE_DOUBLE4;
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    }
}

size_t Vector::size() const
{
    return dimensionality * element_type.size();
}

Matrix::Matrix(const Primitive element_type, const uint32_t dim_0, const uint32_t dim_1)
    : Type(BaseType::MATRIX), element_type(element_type), dim_0(dim_0), dim_1(dim_1)
{
}

CRTL_DATA_TYPE Matrix::data_type() const
{
    switch (element_type.primitive_type) {
    case PrimitiveType::INT:
        switch (dim_0) {
        case 1:
            switch (dim_1) {
            case 2:
                return CRTL_DATA_TYPE_INT1X2;
            case 3:
                return CRTL_DATA_TYPE_INT1X3;
            case 4:
                return CRTL_DATA_TYPE_INT1X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 2:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_INT2X1;
            case 2:
                return CRTL_DATA_TYPE_INT2X2;
            case 3:
                return CRTL_DATA_TYPE_INT2X3;
            case 4:
                return CRTL_DATA_TYPE_INT2X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 3:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_INT3X1;
            case 2:
                return CRTL_DATA_TYPE_INT3X2;
            case 3:
                return CRTL_DATA_TYPE_INT3X3;
            case 4:
                return CRTL_DATA_TYPE_INT3X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 4:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_INT4X1;
            case 2:
                return CRTL_DATA_TYPE_INT4X2;
            case 3:
                return CRTL_DATA_TYPE_INT4X3;
            case 4:
                return CRTL_DATA_TYPE_INT4X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    case PrimitiveType::UINT:
        switch (dim_0) {
        case 1:
            switch (dim_1) {
            case 2:
                return CRTL_DATA_TYPE_UINT1X2;
            case 3:
                return CRTL_DATA_TYPE_UINT1X3;
            case 4:
                return CRTL_DATA_TYPE_UINT1X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 2:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_UINT2X1;
            case 2:
                return CRTL_DATA_TYPE_UINT2X2;
            case 3:
                return CRTL_DATA_TYPE_UINT2X3;
            case 4:
                return CRTL_DATA_TYPE_UINT2X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 3:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_UINT3X1;
            case 2:
                return CRTL_DATA_TYPE_UINT3X2;
            case 3:
                return CRTL_DATA_TYPE_UINT3X3;
            case 4:
                return CRTL_DATA_TYPE_UINT3X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 4:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_UINT4X1;
            case 2:
                return CRTL_DATA_TYPE_UINT4X2;
            case 3:
                return CRTL_DATA_TYPE_UINT4X3;
            case 4:
                return CRTL_DATA_TYPE_UINT4X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    case PrimitiveType::FLOAT:
        switch (dim_0) {
        case 1:
            switch (dim_1) {
            case 2:
                return CRTL_DATA_TYPE_FLOAT1X2;
            case 3:
                return CRTL_DATA_TYPE_FLOAT1X3;
            case 4:
                return CRTL_DATA_TYPE_FLOAT1X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 2:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_FLOAT2X1;
            case 2:
                return CRTL_DATA_TYPE_FLOAT2X2;
            case 3:
                return CRTL_DATA_TYPE_FLOAT2X3;
            case 4:
                return CRTL_DATA_TYPE_FLOAT2X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 3:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_FLOAT3X1;
            case 2:
                return CRTL_DATA_TYPE_FLOAT3X2;
            case 3:
                return CRTL_DATA_TYPE_FLOAT3X3;
            case 4:
                return CRTL_DATA_TYPE_FLOAT3X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 4:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_FLOAT4X1;
            case 2:
                return CRTL_DATA_TYPE_FLOAT4X2;
            case 3:
                return CRTL_DATA_TYPE_FLOAT4X3;
            case 4:
                return CRTL_DATA_TYPE_FLOAT4X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    case PrimitiveType::DOUBLE:
        switch (dim_0) {
        case 1:
            switch (dim_1) {
            case 2:
                return CRTL_DATA_TYPE_DOUBLE1X2;
            case 3:
                return CRTL_DATA_TYPE_DOUBLE1X3;
            case 4:
                return CRTL_DATA_TYPE_DOUBLE1X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 2:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_DOUBLE2X1;
            case 2:
                return CRTL_DATA_TYPE_DOUBLE2X2;
            case 3:
                return CRTL_DATA_TYPE_DOUBLE2X3;
            case 4:
                return CRTL_DATA_TYPE_DOUBLE2X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 3:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_DOUBLE3X1;
            case 2:
                return CRTL_DATA_TYPE_DOUBLE3X2;
            case 3:
                return CRTL_DATA_TYPE_DOUBLE3X3;
            case 4:
                return CRTL_DATA_TYPE_DOUBLE3X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        case 4:
            switch (dim_1) {
            case 1:
                return CRTL_DATA_TYPE_DOUBLE4X1;
            case 2:
                return CRTL_DATA_TYPE_DOUBLE4X2;
            case 3:
                return CRTL_DATA_TYPE_DOUBLE4X3;
            case 4:
                return CRTL_DATA_TYPE_DOUBLE4X4;
            default:
                return CRTL_DATA_TYPE_UNKNOWN;
            }
        default:
            return CRTL_DATA_TYPE_UNKNOWN;
        }
    default:
        return CRTL_DATA_TYPE_UNKNOWN;
    }
}

size_t Matrix::size() const
{
    return dim_0 * dim_1 * element_type.size();
}

BufferView::BufferView(const std::shared_ptr<Type> &element_type, const Access &access)
    : Type(BaseType::BUFFER_VIEW), element_type(element_type), access(access)
{
}

CRTL_DATA_TYPE BufferView::data_type() const
{
    return access == Access::READ_ONLY ? CRTL_DATA_TYPE_BUFFER_VIEW
                                       : CRTL_DATA_TYPE_RWBUFFER_VIEW;
}

size_t BufferView::size() const
{
    // BufferView handle size
    return 8;
}

Texture::Texture(const std::shared_ptr<Type> texel_type,
                 const Access &access,
                 const uint32_t dimensionality)
    : Type(BaseType::TEXTURE),
      texel_type(texel_type),
      access(access),
      dimensionality(dimensionality)
{
}

CRTL_DATA_TYPE Texture::data_type() const
{
    return access == Access::READ_ONLY ? CRTL_DATA_TYPE_TEXTURE
                                       : CRTL_DATA_TYPE_RWTEXTURE;
}

CRTL_TEXTURE_TYPE Texture::texture_type() const
{
    switch (dimensionality) {
    case 1:
        return CRTL_TEXTURE_TYPE_1D;
    case 2:
        return CRTL_TEXTURE_TYPE_2D;
    case 3:
        return CRTL_TEXTURE_TYPE_3D;
    default:
        throw std::runtime_error("Invalid texture dimensionality");
        // Just to silence warning
        return CRTL_TEXTURE_TYPE_1D;
    }
}

size_t Texture::size() const
{
    // Texture sizes are opaque to us from the different APIs
    return 0;
}

AccelerationStructure::AccelerationStructure() : Type(BaseType::ACCELERATION_STRUCTURE) {}

CRTL_DATA_TYPE AccelerationStructure::data_type() const
{
    return CRTL_DATA_TYPE_ACCELERATION_STRUCTURE;
}

size_t AccelerationStructure::size() const
{
    // AccelerationStructure handle size
    return 8;
}

}
}
