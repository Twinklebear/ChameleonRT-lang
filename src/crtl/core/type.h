#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "crtl/crtl.h"

namespace crtl {
namespace ty {
// Note a huge fan of the inheritance setup + allocations pushed by this design,
// but need to work on general types set at runtime

// Similar to CRTL AST side, but we only care about types that can be sent as parameters
enum class BaseType {
    INVALID,
    PRIMITIVE,
    VECTOR,
    MATRIX,
    // TODO: struct is more challenging to handle b/c of alignment differences..
    // STRUCT,
    BUFFER_VIEW,
    TEXTURE,
    ACCELERATION_STRUCTURE,
};

enum class PrimitiveType {
    INVALID,
    // TODO: Maybe don't support bool as a shader parameter type?
    // also alignment issues C++ vs shader lang, it will just be a 4-byte size object in
    // the shader
    // BOOL,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
};

enum class Access {
    READ_ONLY,
    READ_WRITE,
};

class CRTL_EXPORT Type {
public:
    BaseType base_type = BaseType::INVALID;

    Type(const BaseType base_type);

    virtual ~Type() = default;

    virtual CRTL_DATA_TYPE data_type() const = 0;

    // Get the size in bytes of the type
    virtual size_t size() const = 0;
};

// Parse the type info out of the compiler JSON type string output
std::shared_ptr<Type> CRTL_EXPORT parse_type(const std::string &type_str);

class CRTL_EXPORT Primitive : public Type {
public:
    PrimitiveType primitive_type;

    Primitive(const PrimitiveType primitive_type);

    CRTL_DATA_TYPE data_type() const override;

    size_t size() const override;
};

class CRTL_EXPORT Vector : public Type {
public:
    Primitive element_type = PrimitiveType::INVALID;
    uint32_t dimensionality = 0;

    Vector(const Primitive element_type, const uint32_t dimensionality);

    CRTL_DATA_TYPE data_type() const override;

    size_t size() const override;
};

class CRTL_EXPORT Matrix : public Type {
public:
    Primitive element_type = PrimitiveType::INVALID;
    uint32_t dim_0 = 0;
    uint32_t dim_1 = 0;

    Matrix(const Primitive element_type, const uint32_t dim_0, const uint32_t dim_1);

    CRTL_DATA_TYPE data_type() const override;

    size_t size() const override;
};

class CRTL_EXPORT BufferView : public Type {
public:
    // TODO: Maybe some struct in buffer support? Handling the alignment may be tricky,
    // could use the scalar buffer in Vulkan and maybe byte address buffer in DXR?
    std::shared_ptr<Type> element_type;
    Access access;

    BufferView(const std::shared_ptr<Type> &element_type, const Access &access);

    CRTL_DATA_TYPE data_type() const override;

    size_t size() const override;
};

class CRTL_EXPORT Texture : public Type {
public:
    std::shared_ptr<Type> texel_type;
    Access access;
    uint32_t dimensionality = 0;

    Texture(const std::shared_ptr<Type> texel_type,
            const Access &access,
            const uint32_t dimensionality);

    CRTL_DATA_TYPE data_type() const override;

    CRTL_TEXTURE_TYPE texture_type() const;

    size_t size() const override;
};

class CRTL_EXPORT AccelerationStructure : public Type {
public:
    AccelerationStructure();

    CRTL_DATA_TYPE data_type() const override;

    size_t size() const override;
};

}
}
