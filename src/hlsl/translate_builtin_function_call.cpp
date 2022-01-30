#include "translate_builtin_function_call.h"

namespace crtl {
namespace hlsl {
using namespace ast;
std::string translate_builtin_function_call(ast::expr::FunctionCall *call,
                                            ast::decl::Function *callee)
{
    if (callee->get_text() == "ray_index") {
        return "DispatchRaysIndex().xy";
    }
    return "";
}
}
}
