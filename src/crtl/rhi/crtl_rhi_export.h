
#ifndef CRTL_RHI_EXPORT_H
#define CRTL_RHI_EXPORT_H

#ifdef CRTL_RHI_STATIC_DEFINE
#define CRTL_RHI_EXPORT
#define CRTL_RHI_NO_EXPORT
#else
#ifndef CRTL_RHI_EXPORT
#ifdef crtl_rhi_EXPORTS
/* We are building this library */
#define CRTL_RHI_EXPORT __declspec(dllexport)
#else
/* We are using this library */
#define CRTL_RHI_EXPORT __declspec(dllimport)
#endif
#endif

#ifndef CRTL_RHI_NO_EXPORT
#define CRTL_RHI_NO_EXPORT
#endif
#endif

#ifndef CRTL_RHI_DEPRECATED
#define CRTL_RHI_DEPRECATED __declspec(deprecated)
#endif

#ifndef CRTL_RHI_DEPRECATED_EXPORT
#define CRTL_RHI_DEPRECATED_EXPORT CRTL_RHI_EXPORT CRTL_RHI_DEPRECATED
#endif

#ifndef CRTL_RHI_DEPRECATED_NO_EXPORT
#define CRTL_RHI_DEPRECATED_NO_EXPORT CRTL_RHI_NO_EXPORT CRTL_RHI_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef CRTL_RHI_NO_DEPRECATED
#define CRTL_RHI_NO_DEPRECATED
#endif
#endif

#endif /* CRTL_RHI_EXPORT_H */
