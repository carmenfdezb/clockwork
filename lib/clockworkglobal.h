#ifndef CLOCKWORKGLOBAL_H
#define CLOCKWORKGLOBAL_H

#include <QtGlobal>

#ifdef CLOCKWORK_BUILD_LIBRARY
#define CLOCKWORK_EXPORT Q_DECL_EXPORT
#else
#define CLOCKWORK_EXPORT Q_DECL_IMPORT
#endif

#endif // CLOCKWORKGLOBAL_H