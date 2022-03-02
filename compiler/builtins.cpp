#include "builtins.h"

namespace crtl {
std::vector<std::shared_ptr<ast::decl::Declaration>> get_builtin_decls()
{
    using namespace ast;
    std::vector<std::shared_ptr<decl::Declaration>> builtins;

    // ray_index
    {
        auto ret_type = std::make_shared<ty::Vector>(
            std::make_shared<ty::Primitive>(ty::PrimitiveType::UINT), 2);
        auto decl = std::make_shared<decl::Function>(
            "ray_index", std::vector<std::shared_ptr<decl::Variable>>(), ret_type);
        builtins.push_back(std::dynamic_pointer_cast<decl::Declaration>(decl));
    }

    return builtins;
}
}
