//
// Created by user on 05/11/22.
//

#include "dyadic_searcher.h"
#include <cassert>

namespace segments {

void dyadic_searcher::expand_left(scaled_predicate& predicate, std::deque<dyadic_interval>& current) const
{
    auto di = current.front();
    bool is_aligned = di.aligned();
    --di;  // moving to the left

    if (is_aligned) {
        // It's possible that the interval contains two dyadics of starting
        // length if and only if the left hand is not aligned and the right
        // hand is aligned.
        if (predicate(di)) {
            current.push_front(di);
            --di;
        }
    }
    /*
     * At this stage, we should be in the situation where predicate(di) is false.
     * So we should split the interval in half, and check the right hand interval.
     */
    while (di.n<m_max_depth) {
        dyadic_interval left(di);
        left.shrink_interval_left();
        di.shrink_interval_right();

        // Check new di
        if (predicate(di)) {
            current.push_front(di);

            // If the right hand child is good then move to the left
            // and repeat. Note left cannot be good if right is good.
            di = left;
        }

        /*
         * If di was not good here then we continue with di and discard
         * left, since it cannot be part of the same interval.
         */
    }

}
void dyadic_searcher::expand_right(scaled_predicate& predicate, std::deque<dyadic_interval>& current) const
{
    auto di = current.back();
    /*
     * We don't need to check if our neighbour at the same level is good here
     * because in the main search we're moving right to left, so we would have
     * found that interval first and expanded into this one during expand_left.
     */
    ++di;  // moving to the right.

    while (di.n<m_max_depth) {
        dyadic_interval right(di);
        right.shrink_interval_right();
        di.shrink_interval_left();

        // Check current di
        if (predicate(di)) {
            current.push_back(di);

            /*
             * If the current di is included, then we shuffle to the right
             * and start looking there.
             */
            di = right;
        }
        /*
         * If di is not good then we continue searching here and discard
         * right, since that cannot be part of the same interval.
         */
    }

}
void dyadic_searcher::expand(scaled_predicate& predicate, dyadic_interval found_interval)
{
    std::deque<dyadic_interval> found{found_interval};
    expand_left(predicate, found);
    expand_right(predicate, found);

    m_seen[found.back().sup()] = found.front().inf();
}

scaled_predicate dyadic_searcher::rescale_to_unit_interval(const interval& original)
{
    auto a = original.inf();
    auto b = original.sup();

    assert(b>a);  // a degenerate interval is going to be a pain

    /*
     * The transformation of original onto the unit interval is given by
     *
     * x -> (x - a)/(b - a).
     *
     * The scaled_predicate needs to know about the inverse map which is
     * give by
     *
     * y -> a + y*(b-a)
     */

    return {m_predicate, a, (b-a)};
}

void dyadic_searcher::get_next_dyadic(dyadic_interval& current) const
{
    dyadic_real_strict_less diless;
    dyadic_real_strict_greater digreater;

    for (const auto& pair : m_seen) {
        /*
         * Seen pairs are of the form (sup, inf) rather than (inf, sup).
         * Check current.inf >= pair.inf, and if so that current.inf <= pair.sup.
         * In this case, we can skip to the first interval I with I.sup < pair.inf.
         */
        if (!diless(current.inf(), pair.second)) {
            /*
             * Current is equal or right of pair.inf, so we need to check
             * if it overlaps.
             */
            if (!digreater(current.inf(), pair.first)) {

                if (current.n<pair.second.n) {
                    /*
                     * First case, when pair.inf has higher resolution than current.
                     * This case is much more likely to be the case.
                     * In this case, first get the parent of pair.inf in the
                     * resolution of current. Then find the interval to the left.
                     */
                    current.k = (pair.second.k >> (pair.second.n-current.n));
                }
                else if (current.n>pair.second.n) {
                    /*
                     * Second case, when pair.inf has a lower resolution than current.
                     * This case is unlikely but still possible.
                     * In this case, rebase pair.info to the resolution of current,
                     * and then decrement.
                     */
                    current.k = (pair.second.k << (current.n-pair.second.n));
                }
                else {
                    /*
                     * Third case, when pair.inf and current have the same resolution.
                     * Very unlikely, but still possible.
                     * In this case, just decrement pair.inf.
                     */
                    current.k = pair.second.k;
                }
            }
            else {
                /*
                 * If it doesn't overlap then, because of the ordering in
                 * m_seen, we can stop looking here.
                 */
                break;
            }
        }
    }

    /*
     * If we got here then either next < pair.inf for all of the pairs we've seen
     * or, next >= pair.sup for all of the intervals.
     */
    --current.k;
}

std::vector<interval> dyadic_searcher::find_in_unit_interval(scaled_predicate& predicate)
{
//    assert(!predicate(dyadic_interval(0, 0)));
    m_seen.clear();

    for (depth_t current_depth = 1; current_depth<=m_max_depth; ++current_depth) {
        // Starting interval for this depth is [(2^d-1)/2^d, 2^d/2^d)
        dyadic_interval check_current{(mult_t(1) << current_depth), current_depth};
        get_next_dyadic(check_current);

        // We're iterating from right to left, so the end point is 0.
        while (check_current.k>=0) {
            bool is_good = predicate(check_current);
            if (is_good) {
                /*
                 * Current interval is good, expand this as far as allows
                 * and put the beginning and end of the found interval in
                 * the seen map.
                 */
                expand(predicate, check_current);
            }

            // Calculate the next interval
            get_next_dyadic(check_current);
        }

    }
    std::vector<interval> result;
    result.reserve(m_seen.size());
    for (const auto& pair : m_seen) {
        result.emplace_back(static_cast<double>(pair.second), static_cast<double>(pair.first));
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<interval> dyadic_searcher::operator()(const interval& original)
{
    if (m_predicate(original)) {
        // If the original interval is good, there is nothing to do.
        return {original};
    }

    auto predicate = rescale_to_unit_interval(original);
    auto found = find_in_unit_interval(predicate);

    std::vector<interval> result;
    result.reserve(found.size());
    for (const auto& itvl : found) {
        result.emplace_back(predicate.unscale(itvl));
    }
    return result;
}

} // segments
