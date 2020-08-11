

#include <deque>
#include <map>
#include <set>
#include <list>
#include <vector>

#include <libRDE/dyadic.h>
#include <libRDE/dyadic_interval.h>
#include <algorithm>

namespace segment {
	typedef basic_dyadic<long, long> dyadic;
    typedef basic_dyadic_interval<clopen, dyadic> dyadic_interval;

    constexpr auto intervaltype = dyadic_interval::intervaltype; //clopen
	//typedef dyadic_interval::dyadic_t dyadic;

	// real interval type info
	typedef basic_interval<intervaltype, double> interval;

	struct compare_dyadic_by_length {
		bool operator()
			(const dyadic_interval& a, const dyadic_interval& b) const
		{
			return a.n < b.n;
		}
	};


    void ExtendAtContainedEnd(std::set<dyadic_interval> &fragmented_interval, const dyadic::n_t& trim_tolerance, interval &arg, std::function<bool(const interval&)>);
    void ExtendAtOmittedEnd(std::set<dyadic_interval> &fragmented_interval, const dyadic::n_t& trim_tolerance, interval &arg, std::function<bool(const interval&)>);
    void Extend(std::set<dyadic_interval> &fragmented_interval, const dyadic::n_t& trim_tolerance, interval &arg, std::function<bool(const interval&)>);
	std::list<interval> segment(interval arg, std::function<bool(const interval&)>, const dyadic::n_t trim_tolerance, dyadic::n_t signal_tolerance);
	std::list<interval> segment(interval arg, std::function<bool(const interval&)>, const dyadic::n_t precision = 0);

}