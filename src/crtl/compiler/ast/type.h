#pragma once

#include <cstdint>
#include <set>
#include "node.h"

namespace crtl {
namespace ast {
namespace ty {

enum class BaseType {
    PRIMITIVE,
    VECTOR,
    MATRIX,
    STRUCT,
    FUNCTION,
    ENTRY_POINT,
    // Other built in types that we need to know about in the
    // language to rewrite for each API
    BUFFER,
    TEXTURE,
    ACCELERATION_STRUCTURE,
    RAY,
    INVALID,
};

std::string to_string(const BaseType &bt);

enum class EntryPointType {
    RAY_GEN,
    CLOSEST_HIT,
    ANY_HIT,
    INTERSECTION,
    MISS,
    COMPUTE,
    // TODO: Callable shaders?
    INVALID
};

std::string to_string(const EntryPointType &et);

enum class PrimitiveType {
    BOOL,
    INT,
    UINT,
    FLOAT,
    DOUBLE,
    VOID,
};

std::string to_string(const PrimitiveType &pt);

enum class Access {
    READ_ONLY,
    READ_WRITE,
};

enum class Modifier {
    CONST,
    IN,
    OUT,
    IN_OUT,
    INVALID,
};

std::string to_string(const Modifier &m);

class Type {
public:
    BaseType base_type = BaseType::INVALID;
    std::set<Modifier> modifiers;

    Type(const BaseType &base_type);

    Type(const BaseType &base_type, const std::set<Modifier> &modifiers);

    virtual ~Type() = default;

    virtual bool is_template() const;

    bool is_const() const;

    bool is_pointer_like() const;

    bool is_builtin() const;

    virtual const std::string to_string() const = 0;
};

class Primitive : public Type {
public:
    PrimitiveType type_id;

    Primitive(const PrimitiveType type_id);

    Primitive(const PrimitiveType type_id, const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

class Vector : public Type {
public:
    std::shared_ptr<Primitive> element_type;
    uint32_t dimensionality = 0;

    Vector(const std::shared_ptr<Primitive> &element_type, const uint32_t dimensionality);

    Vector(const std::shared_ptr<Primitive> &element_type,
           const uint32_t n_elements,
           const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

class Matrix : public Type {
public:
    std::shared_ptr<Primitive> element_type;
    uint32_t dim_0 = 0;
    uint32_t dim_1 = 0;

    Matrix(const std::shared_ptr<Primitive> &element_type,
           const uint32_t dim_0,
           const uint32_t dim_1);

    Matrix(const std::shared_ptr<Primitive> &element_type,
           const uint32_t dim_0,
           const uint32_t dim_1,
           const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

// TODO: Struct can be templated
class Struct : public Type {
public:
    std::string name;

    Struct(const std::string &name);

    Struct(const std::string &name, const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

class Function : public Type {
public:
    std::vector<std::shared_ptr<Type>> parameters;
    std::shared_ptr<Type> return_type;

    Function(const std::vector<std::shared_ptr<Type>> &parameters,
             const std::shared_ptr<Type> &return_type);

    const std::string to_string() const override;
};

class EntryPoint : public Type {
public:
    std::vector<std::shared_ptr<Type>> parameters;
    EntryPointType entry_point_type;

    EntryPoint(const std::vector<std::shared_ptr<Type>> &parameters,
               const EntryPointType type);

    const std::string to_string() const override;
};

class Template : public Type {
public:
    // Currently template parameters can only be Primitive or Vector types
    std::vector<std::shared_ptr<Type>> template_parameters;

    Template();

    bool is_template() const override;

    const std::string to_string() const override;
};

class Buffer : public Template {
public:
    Access access;

    Buffer(const std::shared_ptr<Type> &element_type, const Access &access);

    Buffer(const std::shared_ptr<Type> &element_type,
           const Access &access,
           const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

class Texture : public Template {
public:
    Access access;
    uint32_t dimensionality = 0;

    Texture(const std::shared_ptr<Type> &texture_type,
            const Access &access,
            const uint32_t dimensionality);

    Texture(const std::shared_ptr<Type> &texture_type,
            const Access &access,
            const uint32_t dimensionality,
            const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

class AccelerationStructure : public Type {
public:
    AccelerationStructure();

    AccelerationStructure(const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

class Ray : public Type {
public:
    Ray();

    Ray(const std::set<Modifier> &modifiers);

    const std::string to_string() const override;
};

}
}
}

