//
// Created by sam on 08/11/22.
//

#include <csignal>

#include "segments.h"

#include "expanding_searcher.h"

using namespace segments;




std::vector<interval>
segments::segment(interval arg, const predicate_t& predicate, depth_t signal_tolerance, depth_t trim_tolerance)
{
    if (trim_tolerance < signal_tolerance)
    {
        trim_tolerance = signal_tolerance;
    }

    ExpandingSearcher searcher(trim_tolerance, signal_tolerance);
    searcher.search_interval(arg, predicate);

    return std::move(searcher).result();
}
