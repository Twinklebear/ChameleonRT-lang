#pragma once

namespace crtl {
/* A ShaderEntryPoint is a single callable shader function that can be called during
 * rendering or as a compute shader (depending on the entry point type). E.g., a raygen
 * function, closesthit function, or compute kernel.
 */
class ShaderEntryPoint {
public:
    virtual ~ShaderEntryPoint() = default;

    // TODO: An API to get the layout for the parameters.
    // Some parameters will be pushed up to be global scope by necessity,
    // e.g. we cannot pass textures through the SBT
    // Alternatively: CRTL can enforce the same restriction on apps, textures and samplers
    // cannot be passed in the SBT. That would be easiest for a first prototype
};
}
