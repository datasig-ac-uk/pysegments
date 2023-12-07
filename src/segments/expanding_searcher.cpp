//
// Created by sam on 12/6/23.
//

#include "expanding_searcher.h"


using namespace segments;


namespace
{
    inline void expand_left_discrete(std::vector<dyadic_interval>& result, const dyadic_interval& base,
                                     const predicate_t& predicate, double lower_bound, depth_t trim_tol)
    {
        auto di = base;
        --di;
        if (base.aligned() && di.inf() >= lower_bound && predicate(di))
        {
            result.push_back(di);
            --di;
        }

        while (di.n < trim_tol)
        {
            dyadic_interval left(di);
            left.shrink_interval_left();
            di.shrink_interval_right();

            if (di.inf() >= lower_bound && predicate(di))
            {
                result.push_back(di);
                di = left;
            }
        }
    }


    inline void expand_right_discrete(std::vector<dyadic_interval>& result, const predicate_t& predicate,
                                      double upper_bound, depth_t trim_tol)
    {
        auto di = result.back();
        bool is_aligned = di.aligned();
        ++di;

        if (!is_aligned && di.sup() <= upper_bound && predicate(di) )
        {
            result.push_back(di);
            ++di;
        }

        while (di.n < trim_tol)
        {
            dyadic_interval right(di);
            right.shrink_interval_right();
            di.shrink_interval_left();

            if (di.sup() <= upper_bound && predicate(di))
            {
                result.push_back(di);
                di = right;
            }
        }
    }
}

bool ExpandingSearcher::expand(component_iterator component, const predicate_t& predicate)
{
    const auto old_inf = component->inf();
    const auto old_sup = component->sup();
    const auto& low_base = m_forward_expansion.front();
    if (old_inf < low_base.inf())
    {
        expand_left_discrete(m_backward_expansion, low_base, predicate, component->inf(), m_trim_tol);
    }
    const auto& high_base = m_forward_expansion.back();
    if (high_base.sup() < old_sup)
    {
        expand_right_discrete(m_forward_expansion, predicate, component->sup(), m_trim_tol);
    }

    const auto new_inf = std::max(static_cast<double>(
                                      (m_backward_expansion.empty()
                                           ? m_forward_expansion.front().inf()
                                           : m_backward_expansion.back().inf())), old_inf);
    const auto new_sup = std::min(static_cast<double>(m_forward_expansion.back().sup()), old_sup);

    m_forward_expansion.clear();
    m_backward_expansion.clear();
    // m_search_components.emplace_back(old_inf, new_inf);
    // m_search_components.emplace_back(new_sup, old_sup);

    m_found.emplace_back(new_inf, new_sup);

    if (new_sup != old_sup)
    {
        *component = {new_sup, old_sup};
    }
    else
    {
        return false;
    }

    if (new_inf != old_inf)
    {
        m_search_components.insert(component, {old_inf, new_inf});
    }

    return true;
}


void ExpandingSearcher::search_interval(const interval& ivl, const predicate_t& predicate)
{
    m_found.clear();
    m_search_components.clear();
    m_search_components.push_back(ivl);

    dyadic_interval di_it(ivl.inf(), 0);
    dyadic_interval di_end(ivl.sup(), 0);

    {
        /*
         * The first layer needs special attention since it is possible for there
         * to be multiple adjacent dyadic intervals for which the predicate is true.
         */
        m_forward_expansion.clear();
        m_backward_expansion.clear();
        auto component = m_search_components.begin();
        for (; di_it < di_end; ++di_it)
        {
            if (predicate(di_it))
            {
                m_forward_expansion.push_back(di_it);
            }
            else if (!m_forward_expansion.empty())
            {
                if (!expand(component, predicate))
                {
                    component = m_search_components.erase(component);
                    break;
                }
                di_it = dyadic_interval(component->inf(), 0);
            }
        }
        if (!m_forward_expansion.empty())
        {
            if (!expand(component, predicate))
            {
                component = m_search_components.erase(component);
            }
        }
    }

    for (depth_t current_depth = 1; current_depth <= m_signal_tol && !m_search_components.empty(); ++current_depth)
    {
        for (auto component = m_search_components.begin(); component != m_search_components.end(); ++component)
        {
            di_it = dyadic_interval(component->inf(), current_depth);
            di_end = dyadic_interval(component->sup(), current_depth);

            for (; di_it < di_end; ++di_it)
            {
                if (predicate(di_it))
                {
                    m_forward_expansion.push_back(di_it);
                    if (!expand(component, predicate))
                    {
                        component = m_search_components.erase(component);
                        break;
                    }

                    di_it = dyadic_interval(component->inf(), current_depth)--;
                }
            }
        }
    }
}
