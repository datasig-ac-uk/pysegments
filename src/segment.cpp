
#include "segment.h"

namespace segment {
	// dyadic interval type info
	

	void ExtendAtContainedEnd(std::set<dyadic_interval> &fragmented_interval, const dyadic::n_t& trim_tolerance, interval &arg, std::function<bool(const interval&)> in_character)
	{
		dyadic_interval front = *(fragmented_interval.begin());
		while (front.n <= trim_tolerance)
		{
			// try to expand fragmented interval in the included end direction
			while
				(
					front.n <= trim_tolerance
					&&
					(!arg.contains(interval(front.shift_back())) ||
						!in_character(
							interval(front.shift_back().included_end(),
							(*fragmented_interval.rbegin()).excluded_end()))
						)
					)
				front = front.shrink_to_contained_end();
			front = front.shift_back();
			if (front.n > trim_tolerance) return;
			else {
				fragmented_interval.insert(front);
			}
		}
	}

	void ExtendAtOmittedEnd(std::set<dyadic_interval> &fragmented_interval, const dyadic::n_t& trim_tolerance, interval &arg, std::function<bool(const interval&)> in_character)
	{
		dyadic_interval back = *(fragmented_interval.rbegin());
		back = back.shift_fwd();
		while (back.n <= trim_tolerance)
		{
			// try to expand fragmented interval in omitted end direction
			while
				(
					back.n <= trim_tolerance
					&&
					(!arg.contains(interval(back)) ||
						!in_character(
							interval((*fragmented_interval.begin()).included_end(), back.excluded_end())))
					)
				back = back.shrink_to_contained_end();

			if (back.n > trim_tolerance)
				return;
			else
			{
				fragmented_interval.insert(back);
				back = back.shift_fwd();
			}
		}
	}

	void Extend(std::set<dyadic_interval> &fragmented_interval, const dyadic::n_t& trim_tolerance, interval &arg, std::function<bool(const interval&)> in_character)
	{
		ExtendAtContainedEnd(fragmented_interval, trim_tolerance, arg, in_character);
		ExtendAtOmittedEnd(fragmented_interval, trim_tolerance, arg, in_character);
	}

	std::list<interval> segment(interval arg, std::function<bool(const interval&)> in_character, const dyadic::n_t trim_tolerance, dyadic::n_t signal_tolerance)
	{
		signal_tolerance = std::min(signal_tolerance, trim_tolerance);
		std::list<interval> answer;
		dyadic_interval central;
		std::set<dyadic_interval> fragmented_interval;
		{
			// break interval into components and arrange the intervals in decreasing order
			std::multiset<dyadic_interval, compare_dyadic_by_length> partition;
			{
				for (auto i : to_dyadic_intervals<intervaltype, dyadic>(arg.inf(), arg.sup(), signal_tolerance, intervaltype))
					partition.insert(i);
			}
			// find longest dyadic interval in interval that is contained in a character
			auto p = partition.begin();
			while ((p->n <= signal_tolerance) && !in_character(*p))
			{

				const dyadic_interval front = *p;
				partition.erase(p);
				partition.emplace(dyadic_interval(front).shrink_to_contained_end());
				partition.emplace(dyadic_interval(front).shrink_to_omitted_end());
				p = partition.begin();
			}

			if (p->n > signal_tolerance)
				// only extraneous data found
				return answer;
			else
				// character found
				central = *p;
		}

		// meaningful data found

		// maximize the support of the symbol
		fragmented_interval.insert(central);
		Extend(fragmented_interval, trim_tolerance, arg, in_character);

		// insert in answer
		answer.emplace_back(interval((*fragmented_interval.begin()).included_end(), (*fragmented_interval.rbegin()).excluded_end()));

		// identify left and right complements of the supporting interval
		interval argbegin(arg.included_end(), (*fragmented_interval.begin()).included_end());
		interval argend((*fragmented_interval.rbegin()).excluded_end(), arg.excluded_end());

		// segment these complimentary intervals and move the content into the answer
		answer.splice(answer.begin(), segment(argbegin, in_character, trim_tolerance, signal_tolerance));
		answer.splice(answer.end(), segment(argend, in_character, trim_tolerance, signal_tolerance));

		// all done
		return answer;
	}

	std::list<interval> segment(interval arg, std::function<bool(const interval&)> in_character, const dyadic::n_t precision)
	{
		return segment(arg, in_character, precision, precision);
	}
}