#pragma once

#include <memory>
#include <unordered_map>
#include "ChameleonRTParserBaseVisitor.h"
#include "global_parameter_descriptor.h"
#include "shader_entry_point_descriptor.h"

namespace crtl {
/* Note: also will find global params and shader entry points, merging these
 * passes into a single variable resolution pass
 *
 * Pass through the program and resolve all variable access to global and SBT
 * parameters throughout the code so they can be rewritten later.
 * TODO: This pass will have to get a bit more complicated to support plain functions
 * that make access to SBT parameter data. Because this has to know which shader
 * entry point we're coming from to map the SBT parameters through. Within CRTL
 * (and CUDA, Metal, ISPC) they're treated as function parameters, but HLSL/GLSL
 * they're visible in the global scope. I'm not sure if you can pass an RWBuffer
 * (or in GLSL, a buffer desc) as a parameter to a function. That's something to try.
 *
 * Correction: it is actually possible to pass both buffer_reference types (in GLSL)
 * and StructuredBuffers (in HLSL) to functions as in/inout parameters. The buffer_reference
 * types in GLSL can all have the member use the same name so it's easy to change the code.
 *
 * I do want to track each variable access, not just parameters, since parameters don't
 * need to be rewritten in any special way. Only globals need some rewriting.
 */
class ResolverVisitor : public crtg::ChameleonRTParserBaseVisitor {
    struct VariableStatus {
        bool defined = false;
        bool read = false;

        std::shared_ptr<VariableDescriptor> var_desc;

        VariableStatus() = default;

        VariableStatus(const std::shared_ptr<VariableDescriptor> &var_desc);
    };

    /* Any user-defined struct types, provided by the struct find pass
     */
    std::unordered_map<std::string, std::shared_ptr<StructDescriptor>> *structs = nullptr;

    // Treated as a stack, but we need to access scopes by index as well
    // when resolving variables
    std::vector<std::unordered_map<std::string, VariableStatus>> scopes;

public:
    bool pass_error = false;

    // TODO: Resolving a global var reference will need to check globals, functions,
    // and entry_points

    /* The global parameters for the shader entry points, indexed by variable name
     */
    std::unordered_map<std::string, std::shared_ptr<GlobalParameterDescriptor>> global_params;

    /* The various shader entry points into the program (raygen, closest hit, any hit,
     * miss, compute)
     */
    std::unordered_map<std::string, std::shared_ptr<ShaderEntryPointDescriptor>> entry_points;

    /* The various plain shader functions in the code
     */
    std::unordered_map<std::string, std::shared_ptr<ShaderFunctionDescriptor>> functions;

    /* Any plain global variables in the code
     * TODO: should non-const globals be allowed? They won't behave as one might expect if
     * trying to access them in the raygen and a closest hit program for example.
     */
    std::unordered_map<std::string, std::shared_ptr<VariableDescriptor>> global_vars;

    /* Each variable access is resolved per-node to create a VariableAccessDescriptor
     * for that reference. This descriptor is fully resolved, pointing to the
     * VariableDescriptor for that reference
     * Accesses that need to be rewritten can then be changed in a subsequent pass
     * by looking up the VariableAccessDescriptor for a given node to find the variable
     * and at that time also resolving any struct member accesses
     * e.g.:
     * struct A {
     *    Buffer<float3> x;
     * };
     *
     * void main() {
     *    A a;
     *    float3 v = a.x[0];
     * }
     *
     * A VariableAccessDescriptor will be created for the a.x[0] node, which can
     * be rewritten for GLSL later to:
     *
     *    float3 v = a.x.data[0];
     */
    std::unordered_map<const antlr4::ParserRuleContext *, std::shared_ptr<VariableDescriptor>>
        variable_access;

    ResolverVisitor() = default;

    ResolverVisitor(
        std::unordered_map<std::string, std::shared_ptr<StructDescriptor>> &structs);

    // TODO: must resolve the type if it's a user struct
    virtual antlrcpp::Any visitVarDecl(crtg::ChameleonRTParser::VarDeclContext *ctx) override;

    virtual antlrcpp::Any visitBlock(crtg::ChameleonRTParser::BlockContext *ctx) override;

    virtual antlrcpp::Any visitFunctionCall(
        crtg::ChameleonRTParser::FunctionCallContext *ctx) override;

    virtual antlrcpp::Any visitFunctionDecl(
        crtg::ChameleonRTParser::FunctionDeclContext *ctx) override;

    virtual antlrcpp::Any visitGlobalParamDecl(
        crtg::ChameleonRTParser::GlobalParamDeclContext *ctx) override;

    // Various expressions
    virtual antlrcpp::Any visitPrimary(crtg::ChameleonRTParser::PrimaryContext *ctx) override;

    virtual antlrcpp::Any visitStructArray(
        crtg::ChameleonRTParser::StructArrayContext *ctx) override;

    virtual antlrcpp::Any visitAssign(crtg::ChameleonRTParser::AssignContext *ctx) override;

private:
    void begin_scope();

    void end_scope();

    void declare(const antlr4::Token *token, const std::shared_ptr<VariableDescriptor> &var);

    void define(const antlr4::Token *name);

    /* Resolve the variable, function, etc. referenced by the token and associate it with the
     * node. Returns the VariableDescriptor that was resolved
     */
    std::shared_ptr<VariableDescriptor> resolve_reference(antlr4::ParserRuleContext *node,
                                                          const antlr4::Token *name);

    /* Find the descriptor for the struct with the given name.
     * Will print an error, set pass_error and return null if the struct does not exist
     */
    std::shared_ptr<StructDescriptor> resolve_struct(const antlr4::Token *name);
};
}
