#pragma once

#include <stddef.h>
#include "crtl_enums.h"
#include "crtl_rhi_export.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct APIObject {};
}
typedef crtl_rhi::APIObject *CRTLAPIObject;
#else
typedef void *CRTLAPIObject;
#endif
