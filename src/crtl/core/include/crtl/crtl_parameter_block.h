#pragma once

#include "crtl_buffer.h"
#include "crtl_core.h"
#include "crtl_device.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct ParameterBlock : APIObject {};
struct ShaderRecordParameterBlock : ParameterBlock {};
struct GlobalParameterBlock : ParameterBlock {};
}
typedef crtl_rhi::ParameterBlock *CRTLParameterBlock;
typedef crtl_rhi::ShaderRecordParameterBlock *CRTLShaderRecordParameterBlock;
typedef crtl_rhi::GlobalParameterBlock *CRTLGlobalParameterBlock;
#else
typedef CRTLAPIObject CRTLParameterBlock;
typedef CRTLAPIObject CRTLShaderRecordParameterBlock;
typedef CRTLAPIObject CRTLGlobalParameterBlock;
#endif

#ifdef __cplusplus
extern "C" {
#endif

CRTL_EXPORT CRTL_ERROR crtl_set_parameter(CRTLDevice device,
                                          CRTLParameterBlock parameter_block,
                                          const char *name,
                                          CRTL_DATA_TYPE data_type,
                                          void *parameter);

#ifdef __cplusplus
}
#endif
