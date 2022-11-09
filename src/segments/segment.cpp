//
// Created by sam on 08/11/22.
//

#include "segments.h"

#include "dyadic_searcher.h"

using namespace segments;


std::vector<interval> segments::segment(interval arg, predicate_t predicate, depth_t max_depth)
{
    dyadic_searcher search(predicate, max_depth);
    return search(arg);
}
