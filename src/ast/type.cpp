#include "type.h"
#include <algorithm>

namespace crtl {
namespace ast {
namespace ty {

std::string to_string(const BaseType &bt)
{
    switch (bt) {
    case BaseType::PRIMITIVE:
        return "PRIMITIVE";
    case BaseType::VECTOR:
        return "VECTOR";
    case BaseType::MATRIX:
        return "MATRIX";
    case BaseType::STRUCT:
        return "STRUCT";
    case BaseType::FUNCTION:
        return "FUNCTION";
    case BaseType::ENTRY_POINT:
        return "ENTRY_POINT";
    case BaseType::BUFFER:
        return "BUFFER";
    case BaseType::TEXTURE:
        return "TEXTURE";
    case BaseType::ACCELERATION_STRUCTURE:
        return "ACCELERATION_STRUCTURE";
    case BaseType::RAY:
        return "RAY";
    default:
        return "INVALID";
    }
}

std::string to_string(const EntryPointType &et)
{
    switch (et) {
    case EntryPointType::RAY_GEN:
        return "RAY_GEN";
    case EntryPointType::CLOSEST_HIT:
        return "CLOSEST_HIT";
    case EntryPointType::ANY_HIT:
        return "ANY_HIT";
    case EntryPointType::INTERSECTION:
        return "INTERSECTION";
    case EntryPointType::MISS:
        return "MISS";
    case EntryPointType::COMPUTE:
        return "COMPUTE";
    case EntryPointType::INVALID:
        return "INVALID";
    }
    return "INVALID";
}

std::string to_string(const PrimitiveType &pt)
{
    switch (pt) {
    case PrimitiveType::BOOL:
        return "BOOL";
    case PrimitiveType::INT:
        return "INT";
    case PrimitiveType::UINT:
        return "UINT";
    case PrimitiveType::FLOAT:
        return "FLOAT";
    case PrimitiveType::DOUBLE:
        return "DOUBLE";
    case PrimitiveType::VOID:
        return "VOID";
    }
    return "INVALID";
}

std::string to_string(const Modifier &m)
{
    switch (m) {
    case Modifier::CONST:
        return "CONST";
    case Modifier::IN:
        return "IN";
    case Modifier::OUT:
        return "OUT";
    case Modifier::IN_OUT:
        return "IN_OUT";
    default:
        return "INVALID";
    }
}

Type::Type(const BaseType &base_type) : base_type(base_type) {}

Type::Type(const BaseType &base_type, const std::set<Modifier> &modifiers)
    : base_type(base_type), modifiers(modifiers)
{
}

bool Type::is_template() const
{
    return false;
}

bool Type::is_const() const
{
    return modifiers.contains(Modifier::CONST);
}

bool Type::is_pointer_like() const
{
    return modifiers.contains(Modifier::IN) || modifiers.contains(Modifier::OUT) ||
           modifiers.contains(Modifier::IN_OUT);
}

Primitive::Primitive(const PrimitiveType type_id) : Type(BaseType::PRIMITIVE), type_id(type_id)
{
}

Primitive::Primitive(const PrimitiveType type_id, const std::set<Modifier> &modifiers)
    : Type(BaseType::PRIMITIVE, modifiers), type_id(type_id)
{
}

const std::string Primitive::to_string() const
{
    return ty::to_string(type_id);
}

Vector::Vector(const std::shared_ptr<Primitive> &element_type, const size_t dimensionality)
    : Type(BaseType::VECTOR), element_type(element_type), dimensionality(dimensionality)
{
}

Vector::Vector(const std::shared_ptr<Primitive> &element_type,
               const size_t dimensionality,
               const std::set<Modifier> &modifiers)
    : Type(BaseType::VECTOR, modifiers),
      element_type(element_type),
      dimensionality(dimensionality)
{
}

const std::string Vector::to_string() const
{
    return element_type->to_string() + std::to_string(dimensionality);
}

Matrix::Matrix(const std::shared_ptr<Primitive> &element_type,
               const size_t dim_0,
               const size_t dim_1)
    : Type(BaseType::MATRIX), element_type(element_type), dim_0(dim_0), dim_1(dim_1)
{
}

Matrix::Matrix(const std::shared_ptr<Primitive> &element_type,
               const size_t dim_0,
               const size_t dim_1,
               const std::set<Modifier> &modifiers)
    : Type(BaseType::MATRIX, modifiers), element_type(element_type), dim_0(dim_0), dim_1(dim_1)
{
}

const std::string Matrix::to_string() const
{
    return element_type->to_string() + std::to_string(dim_0) + "X" + std::to_string(dim_1);
}

Struct::Struct(const std::string &name) : Type(BaseType::STRUCT), name(name) {}

Struct::Struct(const std::string &name, const std::set<Modifier> &modifiers)
    : Type(BaseType::STRUCT, modifiers), name(name)
{
}

const std::string Struct::to_string() const
{
    return name;
}

Function::Function(const std::vector<std::shared_ptr<Type>> &parameters,
                   const std::shared_ptr<Type> &return_type)
    : Type(BaseType::FUNCTION), parameters(parameters), return_type(return_type)
{
}

const std::string Function::to_string() const
{
    std::string str = return_type->to_string() + "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        str += parameters[i]->to_string();
        if (i + 1 < parameters.size()) {
            str += ", ";
        }
    }
    str += ")";
    return str;
}

EntryPoint::EntryPoint(const std::vector<std::shared_ptr<Type>> &parameters,
                       const EntryPointType type)
    : Type(BaseType::ENTRY_POINT), parameters(parameters), entry_point_type(type)
{
}

const std::string EntryPoint::to_string() const
{
    std::string str = ty::to_string(entry_point_type) + "(";
    for (size_t i = 0; i < parameters.size(); ++i) {
        str += parameters[i]->to_string();
        if (i + 1 < parameters.size()) {
            str += ", ";
        }
    }
    str += ")";
    return str;
}

Template::Template() : Type(BaseType::INVALID) {}

bool Template::is_template() const
{
    return true;
}

const std::string Template::to_string() const
{
    std::string str;
    for (size_t i = 0; i < template_parameters.size(); ++i) {
        str += template_parameters[i]->to_string();
        if (i + 1 < template_parameters.size()) {
            str += ", ";
        }
    }
    return str;
}

Buffer::Buffer(const std::shared_ptr<Type> &element_type, const Access &access)
    : access(access)

{
    base_type = BaseType::BUFFER;
    template_parameters.push_back(element_type);
}

Buffer::Buffer(const std::shared_ptr<Type> &element_type,
               const Access &access,
               const std::set<Modifier> &mods)
    : access(access)

{
    base_type = BaseType::BUFFER;
    modifiers = mods;
    template_parameters.push_back(element_type);
}

const std::string Buffer::to_string() const
{
    std::string str = "BUFFER<";
    if (access == Access::READ_WRITE) {
        str = "RWBUFFER<";
    }
    str += Template::to_string() + ">";
    return str;
}

Texture::Texture(const std::shared_ptr<Type> &element_type,
                 const Access &access,
                 const size_t dimensionality)
    : access(access), dimensionality(dimensionality)

{
    base_type = BaseType::BUFFER;
    template_parameters.push_back(element_type);
}

Texture::Texture(const std::shared_ptr<Type> &element_type,
                 const Access &access,
                 const size_t dimensionality,
                 const std::set<Modifier> &mods)
    : access(access), dimensionality(dimensionality)

{
    base_type = BaseType::BUFFER;
    modifiers = mods;
    template_parameters.push_back(element_type);
}

const std::string Texture::to_string() const
{
    std::string str = "TEXTURE";
    if (access == Access::READ_WRITE) {
        str = "RWTEXTURE";
    }
    str += std::to_string(dimensionality) + "D<" + Template::to_string() + ">";
    return str;
}

AccelerationStructure::AccelerationStructure() : Type(BaseType::ACCELERATION_STRUCTURE) {}

AccelerationStructure::AccelerationStructure(const std::set<Modifier> &modifiers)
    : Type(BaseType::ACCELERATION_STRUCTURE, modifiers)
{
}

const std::string AccelerationStructure::to_string() const
{
    return "ACCELERATION_STRUCTURE";
}

Ray::Ray() : Type(BaseType::RAY) {}

Ray::Ray(const std::set<Modifier> &modifiers) : Type(BaseType::RAY, modifiers) {}

const std::string Ray::to_string() const
{
    return "RAY";
}
}
}
}
