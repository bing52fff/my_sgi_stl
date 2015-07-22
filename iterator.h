#ifndef __ITERATOR_H
#define __ITERATOR_H

#include "stl_iterator.h"
#include "type_traits.h"
#include "ipdef.h"

using __IP::input_iterator_tag;
using __IP::output_iterator_tag;
using __IP::forward_iterator_tag;
using __IP::bidirectional_iterator_tag;
using __IP::random_access_iterator_tag;

using __IP::iterator_traits;
using __IP::iterator;
using __IP::value_type;
using __IP::distance_type;
using __IP::reverse_iterator;

using __IP::__type_traits;
using __IP::__true_type;
using __IP::__false_type;

#endif	//iterator.h
