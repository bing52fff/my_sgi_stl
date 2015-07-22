#ifndef __STL_IPDEF_H
#define __STL_IPDEF_H
#include <iostream>

#define __IP ip
#define __IP_BEGIN	namespace ip {
#define __IP_END	};

#define ptrdiff_t int

#define __STL_TEMPLATE_NULL template <>

#define __STL_TRY	try
#define __STL_CATCH_ALL	catch(...)
#define __STL_RETHROW	throw
#define __STL_NOTHROW	throw()
#define __STL_UNWIND(action)	catch(...){action; throw;}

#endif	//ipdef.h
