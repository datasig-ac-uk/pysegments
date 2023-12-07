//
// Created by user on 14/10/22.
//

#ifndef SEGMENTS_SEGMENTS_H
#define SEGMENTS_SEGMENTS_H


#include <vector>

#include "dyadic.h"
#include "dyadic_interval.h"

namespace segments {

using depth_t = int;
using mult_t = int;
using interval = basic_interval<clopen, double>;
using dyadic = basic_dyadic<mult_t, depth_t>;
using dyadic_interval = basic_dyadic_interval<clopen, dyadic>;

using predicate_t = std::function<bool(const interval&)>;


std::vector<interval> segment(interval arg, const predicate_t& predicate, depth_t signal_tolerance, depth_t trim_tolerance=0);


} // namespace segments

#endif //SEGMENTS_SEGMENTS_H
