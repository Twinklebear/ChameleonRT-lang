#include "ast_builder_visitor.h"
#include "ast/declaration.h"
#include "ast/expression.h"
#include "ast/statement.h"
#include "ast_expr_builder_visitor.h"
#include "error_listener.h"

namespace crtl {

using namespace ast;

antlrcpp::Any ASTBuilderVisitor::visitTopLevelDeclaration(
    crtg::ChameleonRTParser::TopLevelDeclarationContext *ctx)
{
    std::shared_ptr<Node> n;
    auto child = visitChildren(ctx);
    if (child.isNotNull()) {
        // A bit of a pain, but there's a limited number of types of top-level declarations
        // we can have so do this cast up to the generic Node type here
        if (child.is<std::shared_ptr<decl::EntryPoint>>()) {
            n = child.as<std::shared_ptr<decl::EntryPoint>>();
        } else if (child.is<std::shared_ptr<decl::Function>>()) {
            n = child.as<std::shared_ptr<decl::Function>>();
        } else if (child.is<std::shared_ptr<decl::Struct>>()) {
            n = child.as<std::shared_ptr<decl::Struct>>();
        } else if (child.is<std::shared_ptr<stmt::VariableDeclaration>>()) {
            n = child.as<std::shared_ptr<stmt::VariableDeclaration>>();
        } else if (child.is<std::shared_ptr<decl::GlobalParam>>()) {
            n = child.as<std::shared_ptr<decl::GlobalParam>>();
        }
    }

    if (n) {
        ast->top_level_decls.push_back(n);
    } else {
        // report_error(ctx->getStart(), "Unhandled/unrecognized top level declaration!");
        report_warning(ctx->getStart(),
                       "Unhandled/unrecognized top level declaration! TODO make error later");
    }
    return antlrcpp::Any();
}

antlrcpp::Any ASTBuilderVisitor::visitFunctionDecl(
    crtg::ChameleonRTParser::FunctionDeclContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    std::string name = ctx->IDENTIFIER()->getText();

    std::vector<std::shared_ptr<decl::Variable>> params;
    if (ctx->parameterList()) {
        params = *visitParameterList(ctx->parameterList())
                      .as<std::shared_ptr<std::vector<std::shared_ptr<decl::Variable>>>>();
    }

    auto block = visit(ctx->block()).as<std::shared_ptr<stmt::Block>>();

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
            return antlrcpp::Any();
        }
        return std::make_shared<decl::EntryPoint>(name, token, params, entry_pt_type, block);
    }

    // Regular functions have a return type
    auto return_type = visitTypeName(ctx->typeName()).as<std::shared_ptr<ty::Type>>();
    return std::make_shared<decl::Function>(name, token, params, block, return_type);
}

antlrcpp::Any ASTBuilderVisitor::visitStructDecl(
    crtg::ChameleonRTParser::StructDeclContext *ctx)
{
    const std::string name = ctx->IDENTIFIER()->getText();

    std::vector<std::shared_ptr<decl::StructMember>> struct_members;
    auto struct_member_decls = ctx->structMember();
    for (auto &m : struct_member_decls) {
        struct_members.push_back(
            visitStructMember(m).as<std::shared_ptr<decl::StructMember>>());
    }
    return std::make_shared<decl::Struct>(
        name, ctx->IDENTIFIER()->getSymbol(), struct_members);
}

antlrcpp::Any ASTBuilderVisitor::visitStructMember(
    crtg::ChameleonRTParser::StructMemberContext *ctx)
{
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = visitTypeName(ctx->typeName()).as<std::shared_ptr<ty::Type>>();

    return std::make_shared<decl::StructMember>(name, ctx->IDENTIFIER()->getSymbol(), type);
}

antlrcpp::Any ASTBuilderVisitor::visitParameterList(
    crtg::ChameleonRTParser::ParameterListContext *ctx)
{
    // Non trivial/POD types need to be wrapped in a shared ptr to work around antlrcpp::Any
    // limitations
    auto parameters = std::make_shared<std::vector<std::shared_ptr<decl::Variable>>>();
    auto param_list = ctx->parameter();
    for (auto *p : param_list) {
        auto res = visitParameter(p).as<std::shared_ptr<decl::Variable>>();
        parameters->push_back(res);
    }
    return parameters;
}

antlrcpp::Any ASTBuilderVisitor::visitParameter(crtg::ChameleonRTParser::ParameterContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = visitTypeName(ctx->typeName()).as<std::shared_ptr<ty::Type>>();
    type->modifiers = parse_modifiers(token, ctx->modifier());
    return std::make_shared<decl::Variable>(name, token, type);
}

antlrcpp::Any ASTBuilderVisitor::visitBlock(crtg::ChameleonRTParser::BlockContext *ctx)
{
    std::vector<std::shared_ptr<stmt::Statement>> statements;
    auto ctx_stmts = ctx->statement();
    for (auto &s : ctx_stmts) {
        auto res = visit(s);
        if (res.isNull()) {
            report_warning(s->getStart(), "TODO WILL: Unimplemented statement -> AST mapping");
        } else {
            if (res.is<std::shared_ptr<stmt::VariableDeclaration>>()) {
                // varDeclStmt can be in top level and blocks, so it returns itself as its
                // child type, not the parent Statement class
                auto var_decl_stmt = res.as<std::shared_ptr<stmt::VariableDeclaration>>();
                statements.push_back(
                    std::dynamic_pointer_cast<stmt::Statement>(var_decl_stmt));
            } else if (res.is<std::shared_ptr<stmt::Block>>()) {
                // Similar case for block, where it returns as a block instead of a statement
                // to make some other code a bit easier
                auto block_stmt = res.as<std::shared_ptr<stmt::Block>>();
                statements.push_back(std::dynamic_pointer_cast<stmt::Statement>(block_stmt));
            } else {
                if (!res.is<std::shared_ptr<stmt::Statement>>()) {
                    report_error(
                        s->getStart(),
                        "Expecting statement for block statements but got non-statement!");
                }
                statements.push_back(res.as<std::shared_ptr<stmt::Statement>>());
            }
        }
    }
    return std::make_shared<stmt::Block>(ctx->getStart(), statements);
}

antlrcpp::Any ASTBuilderVisitor::visitVarDecl(crtg::ChameleonRTParser::VarDeclContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = visitTypeName(ctx->typeName()).as<std::shared_ptr<ty::Type>>();
    if (ctx->CONST()) {
        type->modifiers.insert(ty::Modifier::CONST);
    }
    std::shared_ptr<expr::Expression> initializer;
    if (ctx->expr()) {
        // Temporarily filter out unimplemented parts of the AST visitor
        auto init_res_tmp = visit(ctx->expr());
        if (init_res_tmp.isNull()) {
            report_warning(ctx->expr()->getStart(), "TODO WILL: Parse initializer expression");
        } else {
            initializer = init_res_tmp.as<std::shared_ptr<expr::Expression>>();
        }
    }
    return std::make_shared<decl::Variable>(name, token, type, initializer);
}

antlrcpp::Any ASTBuilderVisitor::visitVarDeclStmt(
    crtg::ChameleonRTParser::VarDeclStmtContext *ctx)
{
    auto decl = visitVarDecl(ctx->varDecl()).as<std::shared_ptr<decl::Variable>>();
    return std::make_shared<stmt::VariableDeclaration>(ctx->getStart(), decl);
}

antlrcpp::Any ASTBuilderVisitor::visitGlobalParamDecl(
    crtg::ChameleonRTParser::GlobalParamDeclContext *ctx)
{
    antlr4::Token *token = ctx->IDENTIFIER()->getSymbol();
    const std::string name = ctx->IDENTIFIER()->getText();
    auto type = visitTypeName(ctx->typeName()).as<std::shared_ptr<ty::Type>>();
    if (ctx->CONST()) {
        type->modifiers.insert(ty::Modifier::CONST);
    }
    return std::make_shared<decl::GlobalParam>(name, token, type);
}

antlrcpp::Any ASTBuilderVisitor::visitIfStmt(crtg::ChameleonRTParser::IfStmtContext *ctx)
{
    auto condition = visit(ctx->expr()).as<std::shared_ptr<expr::Expression>>();
    auto branches = ctx->statement();
    // There will always at least be an if branch
    auto if_branch = visit(branches[0]).as<std::shared_ptr<stmt::Statement>>();
    std::shared_ptr<stmt::Statement> else_branch;
    if (branches.size() == 2) {
        else_branch = visit(branches[1]).as<std::shared_ptr<stmt::Statement>>();
    }
    return std::dynamic_pointer_cast<stmt::Statement>(
        std::make_shared<stmt::IfElse>(ctx->getStart(), condition, if_branch, else_branch));
}

antlrcpp::Any ASTBuilderVisitor::visitWhileStmt(crtg::ChameleonRTParser::WhileStmtContext *ctx)
{
    return antlrcpp::Any();
}

antlrcpp::Any ASTBuilderVisitor::visitForStmt(crtg::ChameleonRTParser::ForStmtContext *ctx)
{
    return antlrcpp::Any();
}

antlrcpp::Any ASTBuilderVisitor::visitReturnStmt(
    crtg::ChameleonRTParser::ReturnStmtContext *ctx)
{
    return antlrcpp::Any();
}

antlrcpp::Any ASTBuilderVisitor::visitExprStmt(crtg::ChameleonRTParser::ExprStmtContext *ctx)
{
    ASTExprBuilderVisitor expr_visitor;
    // TODO: Is some error handling needed here for malformed or invalid expressions?
    // Most errors would be checked for and handled in a later pass (types, etc)
    auto expr = expr_visitor.visit(ctx->expr()).as<std::shared_ptr<expr::Expression>>();
    return std::dynamic_pointer_cast<stmt::Statement>(
        std::make_shared<stmt::Expression>(ctx->getStart(), expr));
}

antlrcpp::Any ASTBuilderVisitor::visitTypeName(crtg::ChameleonRTParser::TypeNameContext *ctx)
{
    std::vector<std::shared_ptr<ty::Type>> template_parameters;
    if (ctx->templateParameters()) {
        // Note: must return shared_ptr wrapper to work around antlrcpp::Any limitation
        template_parameters =
            *visitTemplateParameters(ctx->templateParameters())
                 .as<std::shared_ptr<std::vector<std::shared_ptr<ty::Type>>>>();
    }

    if (template_parameters.size() > 1) {
        // TODO: Haven't implemented template struct parsing/mapping in AST
        report_error(ctx->templateParameters()->getStart(),
                     "Error: Unsupported number of template parameters");
        return antlrcpp::Any();
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
    return antlrcpp::Any();
}

antlrcpp::Any ASTBuilderVisitor::visitTemplateParameters(
    crtg::ChameleonRTParser::TemplateParametersContext *ctx)
{
    auto template_params = std::make_shared<std::vector<std::shared_ptr<ty::Type>>>();
    auto type_list = ctx->typeName();
    for (auto &t : type_list) {
        template_params->push_back(visitTypeName(t).as<std::shared_ptr<ty::Type>>());
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

antlrcpp::Any ASTBuilderVisitor::visitUnary(crtg::ChameleonRTParser::UnaryContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitCall(crtg::ChameleonRTParser::CallContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitStructArray(
    crtg::ChameleonRTParser::StructArrayContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitMult(crtg::ChameleonRTParser::MultContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitDiv(crtg::ChameleonRTParser::DivContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitAddSub(crtg::ChameleonRTParser::AddSubContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitComparison(
    crtg::ChameleonRTParser::ComparisonContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitEquality(crtg::ChameleonRTParser::EqualityContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitLogicAnd(crtg::ChameleonRTParser::LogicAndContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitLogicOr(crtg::ChameleonRTParser::LogicOrContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitAssign(crtg::ChameleonRTParser::AssignContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitParens(crtg::ChameleonRTParser::ParensContext *ctx)
{
    return visit_expr(ctx);
}
antlrcpp::Any ASTBuilderVisitor::visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx)
{
    return visit_expr(ctx);
}

antlrcpp::Any ASTBuilderVisitor::visit_expr(crtg::ChameleonRTParser::ExprContext *ctx)
{
    ASTExprBuilderVisitor expr_visitor;
    return expr_visitor.visit(ctx);
}
}
