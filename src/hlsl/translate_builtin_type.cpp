#include "translate_builtin_type.h"

namespace crtl {
namespace hlsl {
using namespace ast;

std::string translate_modifiers(const std::set<ast::ty::Modifier> &modifiers)
{
    std::string str;
    if (modifiers.contains(ty::Modifier::CONST)) {
        str += "const ";
    }
    if (modifiers.contains(ty::Modifier::IN_OUT) ||
        (modifiers.contains(ty::Modifier::IN) && modifiers.contains(ty::Modifier::OUT))) {
        str += "inout ";
    } else if (modifiers.contains(ty::Modifier::IN)) {
        str += "in ";
    } else if (modifiers.contains(ty::Modifier::OUT)) {
        str += "out ";
    }
    return str;
}

std::string translate_primitive_type(const std::shared_ptr<ast::ty::Primitive> &type)
{
    switch (type->type_id) {
    case ty::PrimitiveType::BOOL:
        return "bool";
    case ty::PrimitiveType::INT:
        return "int";
    case ty::PrimitiveType::UINT:
        return "uint";
    case ty::PrimitiveType::FLOAT:
        return "float";
    case ty::PrimitiveType::DOUBLE:
        return "double";
    case ty::PrimitiveType::VOID:
        return "void";
    }
}

std::string translate_vector_type(const std::shared_ptr<ast::ty::Vector> &type)
{
    return translate_primitive_type(type->element_type) + std::to_string(type->dimensionality);
}

std::string translate_matrix_type(const std::shared_ptr<ast::ty::Matrix> &type)
{
    return translate_primitive_type(type->element_type) + std::to_string(type->dim_0) + "x" +
           std::to_string(type->dim_1);
}

std::string translate_buffer_type(const std::shared_ptr<ast::ty::Buffer> &type)
{
    std::string element_str = "";
    if (type->template_parameters[0]->is_builtin()) {
        element_str = translate_builtin_type(type->template_parameters[0]);
    } else {
        // If it's not a built in type it must be a struct
        const auto struct_type =
            std::dynamic_pointer_cast<ty::Struct>(type->template_parameters[0]);
        element_str = struct_type->name;
    }
    if (type->access == ty::Access::READ_WRITE) {
        return "RWStructuredBuffer<" + element_str + ">";
    }
    return "StructuredBuffer<" + element_str + ">";
}

std::string translate_texture_type(const std::shared_ptr<ast::ty::Texture> &type)
{
    // Textures only support built in primitives and vectors as their element type
    const std::string element_str = translate_builtin_type(type->template_parameters[0]);
    if (type->access == ty::Access::READ_WRITE) {
        return "RWTexture" + std::to_string(type->dimensionality) + "D<" + element_str + ">";
    }
    return "Texture" + std::to_string(type->dimensionality) + "D<" + element_str + ">";
}

std::string translate_builtin_type(const std::shared_ptr<ast::ty::Type> &type)
{
    std::string type_str;
    switch (type->base_type) {
    case ty::BaseType::PRIMITIVE:
        type_str = translate_primitive_type(std::dynamic_pointer_cast<ty::Primitive>(type));
        break;
    case ty::BaseType::VECTOR:
        type_str = translate_vector_type(std::dynamic_pointer_cast<ty::Vector>(type));
        break;
    case ty::BaseType::MATRIX:
        type_str = translate_matrix_type(std::dynamic_pointer_cast<ty::Matrix>(type));
        break;
    case ty::BaseType::BUFFER:
        type_str = translate_buffer_type(std::dynamic_pointer_cast<ty::Buffer>(type));
        break;
    case ty::BaseType::TEXTURE:
        type_str = translate_texture_type(std::dynamic_pointer_cast<ty::Texture>(type));
        break;
        // TODO: Samplers
    case ty::BaseType::ACCELERATION_STRUCTURE:
        type_str = "RaytracingAccelerationStructure";
        break;
    case ty::BaseType::RAY:
        type_str = "RayDesc";
        break;
    default:
        throw std::runtime_error(
            "Error: Unsupported/untranslateable type passed to translate_builtin_type: '" +
            type->to_string() + "'");
    }
    return type_str;
}
}
}
