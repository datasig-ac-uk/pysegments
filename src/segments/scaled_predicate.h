//
// Created by user on 07/11/22.
//

#ifndef SEGMENTS_SCALED_PREDICATE_H
#define SEGMENTS_SCALED_PREDICATE_H


#include <functional>

#include "segments.h"

namespace segments {

class scaled_predicate
{
    using predicate_t = std::function<bool(const interval&)>;
    predicate_t& m_predicate;
    double m_shift;
    double m_scale;

public:

    scaled_predicate(predicate_t& predicate, double shift, double scale)
            : m_predicate(predicate), m_shift(shift), m_scale(scale)
    {}

    interval unscale(const interval&) const;

    bool operator()(const interval& itvl) const;
    bool operator()(dyadic_interval di) const;

};

} // segments

#endif //SEGMENTS_SCALED_PREDICATE_H
