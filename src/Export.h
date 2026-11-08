#pragma once

#include <QtGlobal>

// Symbol visibility for the MufpenCore / MufpenUi shared libraries.
//
// The generated namespace .def exports project functions. Q_OBJECT classes
// additionally carry explicit class-level visibility for staticMetaObject and
// cross-boundary QObject::connect / QMetaObject references. Qt/STL helper
// instantiations are not part of the DLL interface.
// Under a STATIC MUFPEN_LIB_TYPE build (MUFPEN_SHARED_LIBS undefined) the
// macros expand to nothing.
#if !defined(MUFPEN_SHARED_LIBS)
#define MUFPEN_CORE_EXPORT
#define MUFPEN_UI_EXPORT
#elif defined(MUFPEN_CORE_LIBRARY)
#define MUFPEN_CORE_EXPORT Q_DECL_EXPORT
#define MUFPEN_UI_EXPORT Q_DECL_IMPORT
#elif defined(MUFPEN_UI_LIBRARY)
#define MUFPEN_CORE_EXPORT Q_DECL_IMPORT
#define MUFPEN_UI_EXPORT Q_DECL_EXPORT
#else
#define MUFPEN_CORE_EXPORT Q_DECL_IMPORT
#define MUFPEN_UI_EXPORT Q_DECL_IMPORT
#endif
