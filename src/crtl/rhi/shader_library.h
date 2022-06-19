#pragma once

#include <memory>
#include <string>
#include <vector>
#include "shader_entry_point.h"

namespace crtl {

/* A ShaderLibrary stores the entire set of entry points that may be called within a given
 * RayTracingPipeline.
 *
 * A future TODO is to allow this library to be created from multiple source files and perform
 * some form of name-based linking for global parameters and etc.
 */
class ShaderLibrary {
public:
    virtual ~ShaderLibrary() = default;

    // TODO: Maybe a free-function that takes the shader lib shared ptr to keep it alive?
    // May need to ref data from it for setting params that are pushed to be globals
    virtual std::shared_ptr<ShaderEntryPoint> entry_point(const std::string &name) = 0;

    // TODO: Here API to get global param layout
};
}
