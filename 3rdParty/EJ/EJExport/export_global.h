#ifndef COMMON_EXPORT_H
#define COMMON_EXPORT_H
#define CM_TO_TWIP 0.566929098
#define IMAGE_RATIO 0.03
#define TEXT_ASPECT_RATIO 2

#include <QtCore/qglobal.h>

#if defined(EXPORT_LIBRARY)
#  define EXPORTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define EXPORTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // COMMON_EXPORT_H
