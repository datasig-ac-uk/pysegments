//
// Created by sam on 12/6/23.
//

#ifndef EXPANDING_SEARCHER_H
#define EXPANDING_SEARCHER_H

#include "segments.h"
#include <vector>
#include <deque>
#include <list>

namespace segments {

class ExpandingSearcher {
public:
    std::list<interval> m_search_components;
    std::vector<interval> m_found;
    std::vector<dyadic_interval> m_forward_expansion;
    std::vector<dyadic_interval> m_backward_expansion;
    depth_t m_trim_tol;
    depth_t m_signal_tol;

    using component_iterator = typename std::list<interval>::iterator;

    ExpandingSearcher(depth_t trim_tol, depth_t signal_tol)
        : m_trim_tol(trim_tol),
          m_signal_tol(signal_tol)
    {
        m_forward_expansion.reserve(10);
        m_backward_expansion.reserve(10);
    }



    bool expand(component_iterator component, const predicate_t& predicate);


    void search_interval(const interval& ivl, const predicate_t& predicate);


    std::vector<interval> result() && noexcept { return std::move(m_found); }
};

} // segments

#endif //EXPANDING_SEARCHER_H
