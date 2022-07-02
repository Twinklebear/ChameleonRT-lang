#pragma once

#include "crtl_device.h"

#ifdef __cplusplus
namespace crtl_rhi {
struct Queue : APIObject {};
}
typedef crtl_rhi::Queue *CRTLQueue;
#else
typedef CRTLAPIObject CRTLQueue;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Create a new queue to submit commands to the GPU
 */
CRTL_RHI_EXPORT CRTLQueue crtl_new_queue(CRTLDevice device);

// TODO: Also need a command encoder type API to push use of indirect command
// buffers, this should have API for buffer copies, texture copies, barriers, etc.
// then queue takes and runs sets of command buffers. Similar to WebGPU essentially

#ifdef __cplusplus
}
#endif
