#include "VENetDefines.h"

#if defined(_WIN32) && !(defined(__GNUC__)  || defined(__GCCXML__)) && !defined(_VENET_LIB) && defined(_VENET_DLL)
#define VE_DLL_EXPORT __declspec(dllexport)
#else
#define VE_DLL_EXPORT  
#endif

#define STATIC_FACTORY_DECLARATIONS(x) static x* GetInstance(void); \
static void DestroyInstance( x *i);

#define STATIC_FACTORY_DEFINITIONS(x,y) x* x::GetInstance(void) {return VENet::OP_NEW<y>( _FILE_AND_LINE_ );} \
void x::DestroyInstance( x *i) {VENet::OP_DELETE(( y* ) i, _FILE_AND_LINE_);}
