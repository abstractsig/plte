/*
 *
 * configure compiler for io build
 *
 */
#ifndef configure_io_build_H_
#define configure_io_build_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <alloca.h>

#define STVM 					0
#define USE_LIBC

//
// compiler specific stuff
//
#if defined ( __GNUC__ )
# define PACK_STRUCTURE			__attribute__((__packed__))
# define ALLOCATE_ALIGN(size)	__attribute__((__aligned__(size)))
# define UNUSED(v)				(void)(v)
# define SIZEOF(a)				(sizeof(a)/sizeof(a[0]))
# define OPTIMIZE_O2(fn)		fn __attribute__((optimize("-O2")));
#define INLINE_FUNCTION			static inline
  typedef float  float32_t;
  typedef double float64_t;
#else
# error "this compiler is not supported"
#endif

#define IO_GRAPHICS_FLOAT_IS_FLOAT32

#define MIN(a,b) 				(((a)<(b))?(a):(b))
#define plural(v)				(((v) > 1)? "s" : "")
#define KB(n)					((n) * 1024)

//
// a method of producing compile-time size assertions
//
// combine arguments (after expanding arguments)
#define GLUE(a,b) __GLUE(a,b)
#define __GLUE(a,b) a ## b
#define CVERIFY(expr, msg) typedef char GLUE (compiler_verify_, msg) [(expr) ? (+1) : (-1)]
#define COMPILER_VERIFY(exp) CVERIFY (exp, __LINE__)

#define EVENT_DATA 								const
#define DBP_FIELD1								14
#define DBP_FIELD2								20

#define IO_PRINT_BUFFER_SIZE					80
#define TEXT_ENCODING_INITIAL_SIZE			IO_PRINT_BUFFER_SIZE
#define TEXT_ENCODING_GROWTH_INCREMENT		IO_PRINT_BUFFER_SIZE
#define STB_SPRINTF_MIN							IO_PRINT_BUFFER_SIZE	// characters per callback

#define IO_PRINTF_SOCKET 						0

#define PERSISTANT_MEMORY_SECTION __attribute__ ((section(".io_config")))

#endif
/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2020 Gregor Bruce
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
