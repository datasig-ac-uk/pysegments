//
// Created by user on 05/11/22.
//

#ifndef SEGMENTS_DYADIC_SEARCHER_H
#define SEGMENTS_DYADIC_SEARCHER_H

#include <deque>
#include <functional>
#include <map>
#include <utility>
#include <vector>

#include "segments.h"
#include "scaled_predicate.h"

namespace segments {



class dyadic_real_strict_less
{
public:

    bool operator()(const dyadic& lhs, const dyadic& rhs) const noexcept
    {
        auto max = std::max(lhs.n, rhs.n);
        return (lhs.k<<(max-lhs.n)) < (rhs.k<<(max-rhs.n));
    }

};

class dyadic_real_strict_greater
{
public:

    bool operator()(const dyadic& lhs, const dyadic& rhs) const noexcept
    {
        auto max = std::max(lhs.n, rhs.n);
        return (lhs.k<<(max-lhs.n)) > (rhs.k<<(max-rhs.n));
    }

};

class dyadic_searcher {
    using predicate_t = std::function<bool(const interval&)>;
    predicate_t m_predicate;
    std::map<dyadic, dyadic, dyadic_real_strict_greater> m_seen;
    depth_t m_max_depth;

protected:

    void expand_left(scaled_predicate& predicate, std::deque<dyadic_interval>& current) const;
    void expand_right(scaled_predicate& predicate, std::deque<dyadic_interval>& current) const;
    void expand(scaled_predicate& predicate, dyadic_interval found_interval);

public:

    dyadic_searcher(predicate_t predicate, depth_t max_depth)
        : m_predicate(std::move(predicate)), m_max_depth(max_depth)
    {}

private:

    scaled_predicate rescale_to_unit_interval(const interval& original);
    void get_next_dyadic(dyadic_interval& current) const;
    std::vector<interval> find_in_unit_interval(scaled_predicate& predicate);

public:

    std::vector<interval> operator()(const interval& original);


};

} // segments

#endif //SEGMENTS_DYADIC_SEARCHER_H
