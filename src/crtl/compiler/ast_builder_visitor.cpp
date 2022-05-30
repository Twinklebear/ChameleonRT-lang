#include "ast_builder_visitor.h"
#include "ast/declaration.h"
#include "ast/expression.h"
#include "ast/statement.h"
#include "ast_expr_builder_visitor.h"
#include "error_listener.h"

namespace crtl {

using namespace ast;

std::any ASTBuilderVisitor::visitTopLevelDeclaration(
    crtg::ChameleonRTParser::TopLevelDeclarationContext *ctx)
{
    std::shared_ptr<Node> n;
    auto child = visitChildren(ctx);
    if (child.has_value()) {
        // A bit of a pain, but there's a limited number of types of top-level declarations
        // we can have so do this cast up to the generic Node type here
        if (child.type() == typeid(std::shared_ptr<decl::EntryPoint>)) {
            n = std::any_cast<std::shared_ptr<decl::EntryPoint>>(child);
        } else if (child.type() == typeid(std::shared_ptr<decl::Function>)) {
            n = std::any_cast<std::shared_ptr<decl::Function>>(child);
        } else if (child.type() == typeid(std::shared_ptr<decl::Struct>)) {
            n = std::any_cast<std::shared_ptr<decl::Struct>>(child);
        } else if (child.type() == typeid(std::shared_ptr<stmt::VariableDeclaration>)) {
            n = std::any_cast<std::shared_ptr<stmt::VariableDeclaration>>(child);
        } else if (child.type() == typeid(std::shared_ptr<decl::GlobalParam>)) {
            n = std::any_cast<std::shared_ptr<decl::GlobalParam>>(child);
        }
    }

    if (n) {
        ast->top_level_decls.push_back(n);
    } else {
        // report_error(ctx->getStart(), "Unhandled/unrecognized top level declaration!");
        report_warning(ctx->getStart(),
                       "Unhandled/unrecognized top level declaration! TODO make error later");
    }
    return std::any();
}

std::any ASTBuilderVisitor::visitFunctionDecl(
    crtg::ChameleonRTParser::FunctionDeclContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    std::string name = ctx->IDENTIFIER()->getText();

    std::vector<std::shared_ptr<decl::Variable>> params;
    if (ctx->parameterList()) {
        // TODO: Now that ANTLR4 is using std::any this could change to not return the shared
        // ptr wrapper anymore
        params = *std::any_cast<std::shared_ptr<std::vector<std::shared_ptr<decl::Variable>>>>(
            visitParameterList(ctx->parameterList()));
    }

    auto block = std::any_cast<std::shared_ptr<stmt::Block>>(visit(ctx->block()));

    // Shader entry points just have the entry point type and no return value
    if (ctx->entryPointType()) {
        auto entry_pt_type_ctx = ctx->entryPointType();
        auto entry_pt_type = ty::EntryPointType::INVALID;
        if (entry_pt_type_ctx->RAY_GEN()) {
            entry_pt_type = ty::EntryPointType::RAY_GEN;
        } else if (entry_pt_type_ctx->CLOSEST_HIT()) {
            entry_pt_type = ty::EntryPointType::CLOSEST_HIT;
        } else if (entry_pt_type_ctx->ANY_HIT()) {
            entry_pt_type = ty::EntryPointType::ANY_HIT;
        } else if (entry_pt_type_ctx->INTERSECTION()) {
            entry_pt_type = ty::EntryPointType::INTERSECTION;
        } else if (entry_pt_type_ctx->MISS()) {
            entry_pt_type = ty::EntryPointType::MISS;
        } else if (entry_pt_type_ctx->COMPUTE()) {
            entry_pt_type = ty::EntryPointType::COMPUTE;
        } else {
            report_error(entry_pt_type_ctx->getStart(),
                         "Invalid entry point type " + entry_pt_type_ctx->getText());
            return std::any();
        }
        return std::make_shared<decl::EntryPoint>(name, token, params, entry_pt_type, block);
    }

    // Regular functions have a return type
    auto return_type =
        std::any_cast<std::shared_ptr<ty::Type>>(visitTypeName(ctx->typeName()));
    return std::make_shared<decl::Function>(name, token, params, block, return_type);
}

std::any ASTBuilderVisitor::visitStructDecl(crtg::ChameleonRTParser::StructDeclContext *ctx)
{
    const std::string name = ctx->IDENTIFIER()->getText();

    std::vector<std::shared_ptr<decl::StructMember>> struct_members;
    auto struct_member_decls = ctx->structMember();
    for (auto &m : struct_member_decls) {
        struct_members.push_back(
            std::any_cast<std::shared_ptr<decl::StructMember>>(visitStructMember(m)));
    }
    return std::make_shared<decl::Struct>(
        name, ctx->IDENTIFIER()->getSymbol(), struct_members);
}

std::any ASTBuilderVisitor::visitStructMember(
    crtg::ChameleonRTParser::StructMemberContext *ctx)
{
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = std::any_cast<std::shared_ptr<ty::Type>>(visitTypeName(ctx->typeName()));

    return std::make_shared<decl::StructMember>(name, ctx->IDENTIFIER()->getSymbol(), type);
}

std::any ASTBuilderVisitor::visitParameterList(
    crtg::ChameleonRTParser::ParameterListContext *ctx)
{
    // Non trivial/POD types need to be wrapped in a shared ptr to work around std::any
    // limitations
    auto parameters = std::make_shared<std::vector<std::shared_ptr<decl::Variable>>>();
    auto param_list = ctx->parameter();
    for (auto *p : param_list) {
        auto res = std::any_cast<std::shared_ptr<decl::Variable>>(visitParameter(p));
        parameters->push_back(res);
    }
    return parameters;
}

std::any ASTBuilderVisitor::visitParameter(crtg::ChameleonRTParser::ParameterContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = std::any_cast<std::shared_ptr<ty::Type>>(visitTypeName(ctx->typeName()));
    type->modifiers = parse_modifiers(token, ctx->modifier());
    return std::make_shared<decl::Variable>(name, token, type);
}

std::any ASTBuilderVisitor::visitBlock(crtg::ChameleonRTParser::BlockContext *ctx)
{
    std::vector<std::shared_ptr<stmt::Statement>> statements;
    auto ctx_stmts = ctx->statement();
    for (auto &s : ctx_stmts) {
        auto res = visit(s);
        if (!res.has_value()) {
            report_warning(s->getStart(), "TODO WILL: Unimplemented statement -> AST mapping");
        } else {
            if (res.type() == typeid(std::shared_ptr<stmt::VariableDeclaration>)) {
                // varDeclStmt can be in top level and blocks, so it returns itself as its
                // child type, not the parent Statement class
                auto var_decl_stmt =
                    std::any_cast<std::shared_ptr<stmt::VariableDeclaration>>(res);
                statements.push_back(
                    std::dynamic_pointer_cast<stmt::Statement>(var_decl_stmt));
            } else if (res.type() == typeid(std::shared_ptr<stmt::Block>)) {
                // Similar case for block, where it returns as a block instead of a statement
                // to make some other code a bit easier
                auto block_stmt = std::any_cast<std::shared_ptr<stmt::Block>>(res);
                statements.push_back(std::dynamic_pointer_cast<stmt::Statement>(block_stmt));
            } else {
                if (res.type() != typeid(std::shared_ptr<stmt::Statement>)) {
                    report_error(
                        s->getStart(),
                        "Expecting statement for block statements but got non-statement!");
                }
                statements.push_back(std::any_cast<std::shared_ptr<stmt::Statement>>(res));
            }
        }
    }
    return std::make_shared<stmt::Block>(ctx->getStart(), statements);
}

std::any ASTBuilderVisitor::visitVarDecl(crtg::ChameleonRTParser::VarDeclContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = std::any_cast<std::shared_ptr<ty::Type>>(visitTypeName(ctx->typeName()));
    if (ctx->CONST()) {
        type->modifiers.insert(ty::Modifier::CONST);
    }
    std::shared_ptr<expr::Expression> initializer;
    if (ctx->expr()) {
        // Temporarily filter out unimplemented parts of the AST visitor
        auto init_res_tmp = visit(ctx->expr());
        if (!init_res_tmp.has_value()) {
            report_warning(ctx->expr()->getStart(), "TODO WILL: Parse initializer expression");
        } else {
            initializer = std::any_cast<std::shared_ptr<expr::Expression>>(init_res_tmp);
        }
    }
    return std::make_shared<decl::Variable>(name, token, type, initializer);
}

std::any ASTBuilderVisitor::visitVarDeclStmt(crtg::ChameleonRTParser::VarDeclStmtContext *ctx)
{
    auto decl = std::any_cast<std::shared_ptr<decl::Variable>>(visitVarDecl(ctx->varDecl()));
    return std::make_shared<stmt::VariableDeclaration>(ctx->getStart(), decl);
}

std::any ASTBuilderVisitor::visitGlobalParamDecl(
    crtg::ChameleonRTParser::GlobalParamDeclContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = std::any_cast<std::shared_ptr<ty::Type>>(visitTypeName(ctx->typeName()));
    if (ctx->CONST()) {
        type->modifiers.insert(ty::Modifier::CONST);
    }
    return std::make_shared<decl::GlobalParam>(name, token, type);
}

std::any ASTBuilderVisitor::visitIfStmt(crtg::ChameleonRTParser::IfStmtContext *ctx)
{
    auto condition = std::any_cast<std::shared_ptr<expr::Expression>>(visit(ctx->expr()));
    auto branches = ctx->statement();
    // There will always at least be an if branch
    auto if_branch = std::any_cast<std::shared_ptr<stmt::Statement>>(visit(branches[0]));
    std::shared_ptr<stmt::Statement> else_branch;
    if (branches.size() == 2) {
        else_branch = std::any_cast<std::shared_ptr<stmt::Statement>>(visit(branches[1]));
    }
    return std::dynamic_pointer_cast<stmt::Statement>(
        std::make_shared<stmt::IfElse>(ctx->getStart(), condition, if_branch, else_branch));
}

std::any ASTBuilderVisitor::visitWhileStmt(crtg::ChameleonRTParser::WhileStmtContext *ctx)
{
    return std::any();
}

std::any ASTBuilderVisitor::visitForStmt(crtg::ChameleonRTParser::ForStmtContext *ctx)
{
    return std::any();
}

std::any ASTBuilderVisitor::visitReturnStmt(crtg::ChameleonRTParser::ReturnStmtContext *ctx)
{
    return std::any();
}

std::any ASTBuilderVisitor::visitExprStmt(crtg::ChameleonRTParser::ExprStmtContext *ctx)
{
    ASTExprBuilderVisitor expr_visitor;
    // TODO: Is some error handling needed here for malformed or invalid expressions?
    // Most errors would be checked for and handled in a later pass (types, etc)
    auto expr =
        std::any_cast<std::shared_ptr<expr::Expression>>(expr_visitor.visit(ctx->expr()));
    return std::dynamic_pointer_cast<stmt::Statement>(
        std::make_shared<stmt::Expression>(ctx->getStart(), expr));
}

std::any ASTBuilderVisitor::visitTypeName(crtg::ChameleonRTParser::TypeNameContext *ctx)
{
    std::vector<std::shared_ptr<ty::Type>> template_parameters;
    if (ctx->templateParameters()) {
        // Note: must return shared_ptr wrapper to work around antlrcpp::Any limitation
        // TODO: Now that ANTLR4 C++ uses std::any this workaround is no longer needed
        template_parameters =
            *std::any_cast<std::shared_ptr<std::vector<std::shared_ptr<ty::Type>>>>(
                visitTemplateParameters(ctx->templateParameters()));
    }

    if (template_parameters.size() > 1) {
        // TODO: Haven't implemented template struct parsing/mapping in AST
        report_error(ctx->templateParameters()->getStart(),
                     "Error: Unsupported number of template parameters");
        return std::any();
    }

    if (ctx->IDENTIFIER()) {
        // TODO: template structs
        if (!template_parameters.empty()) {
            report_error(ctx->templateParameters()->getStart(),
                         "Error TODO: template structs");
        }
        // TODO: Here it would be good to re-use type's we've already allocated so that it's
        // easier to see two structs have the same type.
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Struct>(ctx->IDENTIFIER()->getText()));
    }

    if (ctx->TEXTURE()) {
        const std::string dimensionality_str = ctx->TEXTURE()->getText().substr(7, 1);
        const size_t dimensionality = std::stoi(dimensionality_str);

        if (template_parameters[0]->base_type != ty::BaseType::PRIMITIVE &&
            template_parameters[0]->base_type != ty::BaseType::VECTOR) {
            report_error(ctx->TEXTURE()->getSymbol(),
                         "Invalid type parameter for Texture: '" +
                             template_parameters[0]->to_string() +
                             "', texture types must be primitive or vector types");
        }

        return std::dynamic_pointer_cast<ty::Type>(std::make_shared<ty::Texture>(
            template_parameters[0], ty::Access::READ_ONLY, dimensionality));
    }

    if (ctx->RWTEXTURE()) {
        const std::string dimensionality_str = ctx->RWTEXTURE()->getText().substr(9, 1);
        const size_t dimensionality = std::stoi(dimensionality_str);

        if (template_parameters[0]->base_type != ty::BaseType::PRIMITIVE &&
            template_parameters[0]->base_type != ty::BaseType::VECTOR) {
            report_error(ctx->TEXTURE()->getSymbol(),
                         "Invalid type parameter for RWTexture: '" +
                             template_parameters[0]->to_string() +
                             "', texture types must be primitive or vector types");
        }

        return std::dynamic_pointer_cast<ty::Type>(std::make_shared<ty::Texture>(
            template_parameters[0], ty::Access::READ_WRITE, dimensionality));
    }

    if (ctx->BUFFER()) {
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Buffer>(template_parameters[0], ty::Access::READ_ONLY));
    }

    if (ctx->RWBUFFER()) {
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Buffer>(template_parameters[0], ty::Access::READ_WRITE));
    }

    if (ctx->ACCELERATION_STRUCTURE()) {
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::AccelerationStructure>());
    }

    if (ctx->RAY()) {
        return std::dynamic_pointer_cast<ty::Type>(std::make_shared<ty::Ray>());
    }

    // Now handle all the primitive types
    if (ctx->VOID()) {
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Primitive>(ty::PrimitiveType::VOID));
    }

    const std::string type_str = ctx->getText();
    if (type_str.starts_with("bool")) {
        auto primitive_type = std::make_shared<ty::Primitive>(ty::PrimitiveType::BOOL);
        if (ctx->BOOL()) {
            return std::dynamic_pointer_cast<ty::Type>(primitive_type);
        }

        const size_t dimension_0 = std::stoi(type_str.substr(4, 1));
        if (ctx->BOOL1() || ctx->BOOL2() || ctx->BOOL3() || ctx->BOOL4()) {
            return std::dynamic_pointer_cast<ty::Type>(
                std::make_shared<ty::Vector>(primitive_type, dimension_0));
        }

        const size_t dimension_1 = std::stoi(type_str.substr(6, 1));
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Matrix>(primitive_type, dimension_0, dimension_1));
    }

    if (type_str.starts_with("int")) {
        auto primitive_type = std::make_shared<ty::Primitive>(ty::PrimitiveType::INT);
        if (ctx->INT()) {
            return std::dynamic_pointer_cast<ty::Type>(primitive_type);
        }

        const size_t dimension_0 = std::stoi(type_str.substr(3, 1));
        if (ctx->INT1() || ctx->INT2() || ctx->INT3() || ctx->INT4()) {
            return std::dynamic_pointer_cast<ty::Type>(
                std::make_shared<ty::Vector>(primitive_type, dimension_0));
        }

        const size_t dimension_1 = std::stoi(type_str.substr(5, 1));
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Matrix>(primitive_type, dimension_0, dimension_1));
    }

    if (type_str.starts_with("uint")) {
        auto primitive_type = std::make_shared<ty::Primitive>(ty::PrimitiveType::UINT);
        if (ctx->UINT()) {
            return std::dynamic_pointer_cast<ty::Type>(primitive_type);
        }

        const size_t dimension_0 = std::stoi(type_str.substr(4, 1));
        if (ctx->UINT1() || ctx->UINT2() || ctx->UINT3() || ctx->UINT4()) {
            return std::dynamic_pointer_cast<ty::Type>(
                std::make_shared<ty::Vector>(primitive_type, dimension_0));
        }

        const size_t dimension_1 = std::stoi(type_str.substr(6, 1));
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Matrix>(primitive_type, dimension_0, dimension_1));
    }

    if (type_str.starts_with("float")) {
        auto primitive_type = std::make_shared<ty::Primitive>(ty::PrimitiveType::FLOAT);
        if (ctx->FLOAT()) {
            return std::dynamic_pointer_cast<ty::Type>(primitive_type);
        }

        const size_t dimension_0 = std::stoi(type_str.substr(5, 1));
        if (ctx->FLOAT1() || ctx->FLOAT2() || ctx->FLOAT3() || ctx->FLOAT4()) {
            return std::dynamic_pointer_cast<ty::Type>(
                std::make_shared<ty::Vector>(primitive_type, dimension_0));
        }

        const size_t dimension_1 = std::stoi(type_str.substr(7, 1));
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Matrix>(primitive_type, dimension_0, dimension_1));
    }

    if (type_str.starts_with("double")) {
        auto primitive_type = std::make_shared<ty::Primitive>(ty::PrimitiveType::DOUBLE);
        if (ctx->DOUBLE()) {
            return std::dynamic_pointer_cast<ty::Type>(primitive_type);
        }

        const size_t dimension_0 = std::stoi(type_str.substr(6, 1));
        if (ctx->DOUBLE1() || ctx->DOUBLE2() || ctx->DOUBLE3() || ctx->DOUBLE4()) {
            return std::dynamic_pointer_cast<ty::Type>(
                std::make_shared<ty::Vector>(primitive_type, dimension_0));
        }

        const size_t dimension_1 = std::stoi(type_str.substr(8, 1));
        return std::dynamic_pointer_cast<ty::Type>(
            std::make_shared<ty::Matrix>(primitive_type, dimension_0, dimension_1));
    }

    report_error(ctx->getStart(), "Unhandled type string " + ctx->getText());
    return std::any();
}

std::any ASTBuilderVisitor::visitTemplateParameters(
    crtg::ChameleonRTParser::TemplateParametersContext *ctx)
{
    auto template_params = std::make_shared<std::vector<std::shared_ptr<ty::Type>>>();
    auto type_list = ctx->typeName();
    for (auto &t : type_list) {
        template_params->push_back(std::any_cast<std::shared_ptr<ty::Type>>(visitTypeName(t)));
    }
    return template_params;
}

std::set<ty::Modifier> ASTBuilderVisitor::parse_modifiers(
    antlr4::Token *token,
    const std::vector<crtg::ChameleonRTParser::ModifierContext *> &modifier_list)
{
    std::set<ty::Modifier> modifiers;
    for (auto *m : modifier_list) {
        if (m->CONST()) {
            modifiers.insert(ty::Modifier::CONST);
        } else if (m->IN()) {
            modifiers.insert(ty::Modifier::IN);
        } else if (m->OUT()) {
            modifiers.insert(ty::Modifier::OUT);
        } else if (m->IN_OUT()) {
            modifiers.insert(ty::Modifier::IN_OUT);
        }
    }
    if (modifiers.size() != modifier_list.size()) {
        report_error(token, "Redundant modifiers found in modifier list");
    }
    if (modifiers.contains(ty::Modifier::CONST) &&
        (modifiers.contains(ty::Modifier::OUT) || modifiers.contains(ty::Modifier::IN_OUT))) {
        report_error(token, "Invalid modifier set: const found with out or inout");
    }
    if (modifiers.contains(ty::Modifier::IN_OUT) &&
        (modifiers.contains(ty::Modifier::IN) || modifiers.contains(ty::Modifier::OUT))) {
        report_error(token, "Invalid modifier set: redundant use of in or out with inout");
    }

    return modifiers;
}

std::any ASTBuilderVisitor::visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitCall(crtg::ChameleonRTParser::CallContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitStructArray(crtg::ChameleonRTParser::StructArrayContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitMult(crtg::ChameleonRTParser::MultContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitDiv(crtg::ChameleonRTParser::DivContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitComparison(crtg::ChameleonRTParser::ComparisonContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitEquality(crtg::ChameleonRTParser::EqualityContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitLogicAnd(crtg::ChameleonRTParser::LogicAndContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitAssign(crtg::ChameleonRTParser::AssignContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitParens(crtg::ChameleonRTParser::ParensContext *ctx)
{
    return visit_expr(ctx);
}
std::any ASTBuilderVisitor::visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx)
{
    return visit_expr(ctx);
}

std::any ASTBuilderVisitor::visit_expr(crtg::ChameleonRTParser::ExprContext *ctx)
{
    ASTExprBuilderVisitor expr_visitor;
    return expr_visitor.visit(ctx);
}
}
