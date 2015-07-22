#ifndef __MEMORY_H
#define __MEMORY_H

#include "stl_construct.h"
#include "stl_alloc.h"
#include "stl_uninitialized.h"

using __IP::__malloc_alloc_template;
using __IP::malloc_alloc;
using __IP::alloc;
using __IP::__default_alloc_template;
using __IP::simple_alloc;

using __IP::construct;
using __IP::destroy;

using __IP::uninitialized_fill;
using __IP::uninitialized_fill_n;
using __IP::uninitialized_copy;
#endif	//memory.h
