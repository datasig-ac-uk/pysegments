//
// Created by user on 07/11/22.
//

#include "scaled_predicate.h"

namespace segments {

interval scaled_predicate::unscale(const interval& scaled_interval) const
{
    return {
            scaled_interval.inf()*m_scale+m_shift,
            scaled_interval.sup()*m_scale+m_shift
    };
}
bool scaled_predicate::operator()(const interval& itvl) const
{
    return m_predicate(unscale(itvl));
}
bool scaled_predicate::operator()(dyadic_interval di) const
{
    return operator()(
            interval{static_cast<double>(di.inf()),
                     static_cast<double>(di.sup())}
    );
}

} // segments
