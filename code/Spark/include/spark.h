// spark

// misc

// default do-nothing spark macros

// specifies a function should never be inlined by compiler
#ifndef SPARK_NEVER_INLINE
#define SPARK_NEVER_INLINE
#endif

// specify function should always be inlined by compiler
#ifndef SPARK_FORCE_INLINE
#define SPARK_FORCE_INLINE
#endif

// break if debugger is attached
#ifndef SPARK_DEBUGBREAK
#define SPARK_DEBUGBREAK(X)
#endif

#ifndef SPARK_ASSERT
#define SPARK_ASSERT(X)
#endif

#ifndef SPARK_VERIFY
#define SPARK_VERIFY(X) (void)(X);
#endif

#include "spark/error.h"
// api
#include "spark/enums.h"
#include "spark/node.h"
#include "spark/runtime.h"
#include "spark/codegen.h"
#include "spark/device_buffer.h"
// kernel authoring
#include "spark/meta.h"
#include "spark/rvalue.h"
#include "spark/lvalue.h"
#include "spark/properties.h"
#include "spark/constructors.h"
#include "spark/void.h"
#include "spark/scalar.h"
#include "spark/vector2.h"
#include "spark/vector4.h"
#include "spark/pointer.h"
#include "spark/types.h"
#include "spark/buffer.h"
#include "spark/kernel.h"
#include "spark/comment.h"
#include "spark/statement.h"
#include "spark/range.h"
#include "spark/functions.h"

