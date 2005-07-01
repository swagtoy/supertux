/**
 * WARNING: This file is automatically generated from:
 *  'src/scripting/wrapper.interface.hpp'
 * DO NOT CHANGE
 */
#ifndef __supertux_WRAPPER_H__
#define __supertux_WRAPPER_H__

#include "wrapper_util.hpp"

extern WrappedFunction supertux_global_functions[];
extern WrappedClass supertux_classes[];
extern WrappedConstant<int> supertux_int_constants[];
extern WrappedConstant<float> supertux_float_constants[];
extern WrappedConstant<const char*> supertux_string_constants[];

static inline void register_supertux_wrapper(HSQUIRRELVM v)
{
    register_functions(v, supertux_global_functions);
    register_classes(v, supertux_classes);
    register_constants(v, supertux_int_constants);
    register_constants(v, supertux_float_constants);
    register_constants(v, supertux_string_constants);
}

#endif

