/* *************************************************************

Copyright 2010-16 Terry Lyons, Stephen Buckley, Djalil Chafai,
Greg Gyurkó and Arend Janssen. Substantially modified by Terry
Lyons and Hao Ni.

Distributed under the terms of the GNU General Public License,
Version 3. (See accompanying file License.txt)

************************************************************* */
#ifndef __DYADIC_INTERVAL__
#define __DYADIC_INTERVAL__
#include <algorithm>
#include <functional>
#include <exception>
#include <deque>
#include <list>
#include <iostream>
#include "dyadic.h"
#include <cmath>

/// this enum names the two types of half open interval
enum IntervalType : int16_t {
	clopen = '[)', // includes its leftmost value but not its rightmost value
	opencl = '(]'  // includes its rightmost value but not its leftmost value
};

/// a comparison operator that always has the contained element as the least element in the interval
/// for a clopen interval this is the standard less operator, for opencl it is reversed
template <IntervalType intervaltype>
struct Compare
{
	template <class T>
	bool operator() (T lhs, T rhs)const
	{
		return (intervaltype == clopen) ? lhs < rhs : lhs > rhs;
	};
};

template<IntervalType intervaltype, class SCALAR>
class basic_interval;

// A basic dyadic interval I is an interval |m/2^n,(m+1)/2^n| where
// m is a signed integer of type dyadic::k_t
// n is a signed integer of type dyadic::n_t
// and |x,y| = [x,y) or (x, y] according to the intervaltype

/// A basic_dyadic_interval is differentiated by type and tagged internally as
/// clopen or opencl. Individual intervals are represented internally by the dyadic
/// end value they contain and this tag. The template generates different types for
/// clopen and opencl intervals and they do not communicate.
template<IntervalType intervaltype, class DYADIC>
class basic_dyadic_interval;

// DEFINITION: a dyadic interval is associated to a real interval if its
// contained endpoint is contained in the real interval

/// dyadic intervals are said to represent
/// the interval I := |inf,sup| to a tolerance n if
///
/// they adjacent and disjoint
/// they are all of the same type as I
///           consequently their union J is an interval of the same type as I
///
/// I\J is a single interval (possibly empty) that that does no contain the endpoint of I
/// that I contains and is of length less than 2^(-tolerance)
///
/// J\I is a single interval (possibly empty) that is of length is less than 2^(-tolerance)
/// J does not contain the endpoint of I that I does not contain
///
/// the dyadic intervals representing I are maximal, in the sense that the union of two dyadic intervals in the
/// representation is never a dyadic interval, and no dyadic interval has length less than 2^(-tolerance)
///            this partition into intervals is unique, and possibly empty.
///
/// CRUCIALLY, if a real interval is partitioned into clopen intervals then sending these
/// partitioning intervals to dyadic intervals will produce a consistent dyadic partition.
/// This requires that small intervals get translated to the empty interval. In general
/// a real clopen interval will contain a dyadic interval only if it contains its contained endpoint.
/// The interval type is determined by the template argument intervaltype.

template<IntervalType intervaltype, class DYADIC>
std::deque<basic_dyadic_interval<intervaltype, DYADIC> > to_dyadic_intervals(double inf, double sup, int tolerance, IntervalType);

/// A dyadic interval
template<IntervalType interval_t, class DYADIC>
class basic_dyadic_interval : DYADIC // use private derivation to avoid spurious conversions etc.
{
public:
	static const IntervalType intervaltype{ interval_t };
	static const IntervalType reverseintervaltype{ (interval_t == opencl) ? clopen : opencl };
	typedef DYADIC dyadic_t;
	typedef typename dyadic_t::k_t k_t;
	typedef typename dyadic_t::n_t n_t;
private:

	typedef Compare<interval_t> compare;
	const compare cmp;

public:
	static const k_t unit{ (interval_t == clopen) ? 1 : -1 };
	dyadic_t excluded_end() const { return dyadic_t(k + unit, n); }
	dyadic_t included_end() const { return *this; }

	/// the interval of the opposite orientation that differs from this interval only in the excluded endpoint
	basic_dyadic_interval<reverseintervaltype, dyadic_t> reversed() const { return{ excluded_end() }; }

	/// and a function object that compares using the conventional order on these reversed intervals
	struct rcompare
	{
		bool operator() (const basic_dyadic_interval& lhs, const basic_dyadic_interval& rhs)  const { return lhs.reversed() < rhs.reversed(); }
	};

public:
	using dyadic_t::k;
	using dyadic_t::n;
	dyadic_t inf() const { return (interval_t == clopen) ? included_end() : excluded_end(); }
	dyadic_t sup() const { return (interval_t == clopen) ? excluded_end() : included_end(); }

	// do not use these since they do not respect the ordering - instead, add or subtract unit.
	using dyadic_t::operator ++;
	using dyadic_t::operator --;

	/// Every interval [k/2^n,(k+unit)/2^n) and can be constructed from dyadic k/2^n
	/// If an interval is ever constructed on a number type (including dyadic) then
	/// it will always contain that number.
	/// The width of the interval is 2^(-n) and is reflected by the resolution of the dyadic.

public:
	basic_dyadic_interval(dyadic_t s)
		: dyadic_t(s), cmp{}
	{
	}
	basic_dyadic_interval(k_t k1, n_t n1)
		: basic_dyadic_interval(dyadic_t(k1, n1))
	{
	}

	/// the dyadic interval of length 2^-(resolution) containing the dyadic value arg
	basic_dyadic_interval(dyadic_t arg, n_t resolution) : cmp{}
	{
		bool isint = arg.rebase(resolution); // set in greatest denominator so k is integer and denom <= 2^resolution
		if (!isint)
			// remove fractional part (in correct direction)
		{
			k_t k1 = arg.k;
			arg.k = unit * (k1*unit - dyadic_t::mod((k1*unit), dyadic_t::int_two_to_int_power(arg.n - resolution)));
			isint = arg.rebase(resolution);
		}
		assert(isint);
		dyadic_t::operator = (arg);
	}

	/// the dyadic interval of length 2^-(resolution) containing the numerical value arg
	/// will overflow if 2^(resolution) * arg cannot be represented as a k_t integer
	basic_dyadic_interval(double arg, n_t resolution) : cmp{}
	{
		dyadic_t out;
		auto rescaled_arg = ldexp(arg, resolution);
		assert(double(std::numeric_limits<k_t>::max()) > abs(rescaled_arg));
		switch (interval_t)
		{
		case opencl: out = dyadic_t{ (k_t)ceil(rescaled_arg), resolution }; break;
		case clopen: out = dyadic_t{ (k_t)floor(rescaled_arg), resolution }; break;
		}
		dyadic_t::operator = (out);
	}

	/// the dyadic interval of length 2^-(resolution) containing the numerical value arg
	/// with the largest resolution so that 2^(resolution) * arg can be represented as a k_t integer
	explicit basic_dyadic_interval(double arg) : cmp{}
	{
		double temp = abs(arg) / double(std::numeric_limits<k_t>::max());
		if (temp == 0)
			operator=(basic_dyadic_interval(0, std::numeric_limits<k_t>::max()));
		else {
			assert(
				ceil(log2(temp)) < double(std::numeric_limits<k_t>::max())
				&&
				ceil(log2(temp)) > double(std::numeric_limits<k_t>::min())
			);
			operator=(basic_dyadic_interval(arg, -(k_t)ceil(log2(temp))));
		}
	}

	/// Default constructor
	/// the unit interval [0, 1) (clopen) or (-1, 0] (opencl) is constructed
	basic_dyadic_interval(void)
		: basic_dyadic_interval(k_t(0), n_t(0))
	{
	}
	basic_dyadic_interval& operator=(const basic_dyadic_interval& rhs) {
		k = rhs.k;
		n = rhs.n;
		return *this;
	}

	/// Default destructor.
	~basic_dyadic_interval(void)
	{
	}

	/// Flips to the complementary interval within the enlarged interval and returns a reference.
	basic_dyadic_interval & flip_interval(void)
	{
		(k % 2) ? k -= unit : k += unit;
		return *this;
	}

	/// Returns true if contains the same endpoint as its parent
	bool aligned(void) const
	{
		basic_dyadic_interval parent{ *this, n - 1 }, child{ parent, n };
		return operator==(child);
	}

	/// Steps down a number n (default 1) of levels in the dyadic framework without
	/// changing the left endpoint of the interval.
	basic_dyadic_interval & shrink_interval_left(void)
	{
		return *this = (interval_t == clopen) ? shrink_to_contained_end() : shrink_to_omitted_end();
	}

	basic_dyadic_interval & shrink_interval_left(n_t k)
	{
		assert(k >= 0);
		for (; k > 0; k--) shrink_interval_left();
		return *this;
	}

	basic_dyadic_interval & shrink_interval_right(n_t k)
	{
		assert(k >= 0);
		for (; k > 0; k--) shrink_interval_right();
		return *this;
	}

	// translate the interval without length change
	basic_dyadic_interval shift_back(n_t arg = 1) const
	{
		basic_dyadic_interval temp(*this);
		temp.k -= unit * arg;
		return temp;
	}

	basic_dyadic_interval shift_fwd(n_t arg = 1) const
	{
		basic_dyadic_interval temp(*this);
		temp.k += unit * arg;
		return temp;
	}


	basic_dyadic_interval shrink_to_contained_end(n_t arg = 1) const
	{
		return basic_dyadic_interval{ *this, n + arg };
	}

	basic_dyadic_interval shrink_to_omitted_end(void) const
	{
		return shrink_to_contained_end().flip_interval();
	}

	/// Steps down 1 level in the dyadic framework without changing the numerical sup of the interval.
	basic_dyadic_interval & shrink_interval_right(void)
	{
		return (*this) = (interval_t == opencl) ? shrink_to_contained_end() : shrink_to_omitted_end();
	}

	/// Steps up a number n (default 1) of levels in the dyadic framework giving the unique dyadic
	/// interval at that level to contain the current interval.
	basic_dyadic_interval & expand_interval(k_t Arg = 1)
	{
		return operator = (basic_dyadic_interval<interval_t, dyadic_t>(included_end(), n - Arg));
	}

	/// total comparison operator for dyadic intervals
	/// the tree is explored down and across leaf wise starting at the included end
	/// the order is therefore different for clopen and opencl intervals.
	bool operator < (const basic_dyadic_interval & Arg) const
	{
		// tree leaf order
		basic_dyadic_interval lhs{ *this };
		basic_dyadic_interval rhs{ Arg };

		if (lhs.n > rhs.n)
		{
			// lhs shorter
			rhs.k = dyadic_t::shift(rhs.k, lhs.n - rhs.n);
			return unit*(rhs.k - lhs.k) > 0;
		}
		else
		{
			// lhs longer or equal
			lhs.k = dyadic_t::shift(lhs.k, rhs.n - lhs.n);
			return (lhs.n == rhs.n) ? unit*(rhs.k - lhs.k) > 0 : unit*(rhs.k - lhs.k) >= 0;
		}
	}

	/// Not-equal operator for dyadic intervals.
	bool operator != (const basic_dyadic_interval & Arg) const
	{
		return (k != Arg.k || n != Arg.n);
	}

	/// Are-equal operator for dyadic intervals
	bool operator == (const basic_dyadic_interval & Arg) const
	{
		return (k == Arg.k && n == Arg.n);
	}

	/// Contains operator for dyadic intervals
	bool contains(const basic_dyadic_interval & Arg) const
	{
		bool ans;
		// Arg must be shorter or equal to be contained
		if (Arg.n >= n)
			// remove fractional part (in correct direction) and compare
			ans = (dyadic_t::shift(k, (Arg.n - n)) == unit * (Arg.k*unit - dyadic_t::mod((Arg.k*unit), dyadic_t::int_two_to_int_power(Arg.n - n))));
		else
			ans = false;
		return ans;
	}

	/// There is no obvious inclusion with [inf,sup) and this choice is made to ensure
	/// that if the real intervals form a partition so do the dyadic intervals.

	/// Finds the unique dyadic interval with denominator precision containing Arg in [k/2^n,(k+1)/2^n).
	static basic_dyadic_interval dyadic_bracket(const double Arg, const int AbsolutePrecision = 0)
	{
		basic_dyadic_interval temp;
		temp.n = AbsolutePrecision;
		temp.k = (int)floor(ldexp(Arg, AbsolutePrecision));
		return temp;
	}
private:
/*
	/// Explicit constructors that when given a dyadic or double and an interval type returns the closest dyadic with n = tolerance
	/// whose value is just larger ( (] case) or smaller ( [) case) or equal to that of the dyadic or double and so that the relevant
	/// interval containing the new dyadic contains the numerical value of the constructing number;
	/// 0 returns 0 and n = tolerance
	static basic_dyadic  make_basic_dyadic(const double arg, const n_t tolerance)
	{
		auto rescaled_arg = ldexp(arg, tolerance);
		assert(double(std::numeric_limits<k_t>::max()) > abs(rescaled_arg));

		switch (interval_t)
		{
		case opencl: k = (k_t)ceil(rescaled_arg); n = tolerance; return;
		case clopen: k = (k_t)floor(rescaled_arg); n = tolerance; return;
		}
	}

	/// Explicit constructors that when given a dyadic or double and an interval type returns the closest dyadic with n = tolerance
	/// whose value is just larger ( (] case) or smaller ( [) case) or equal to that of the dyadic or double and so that the relevant
	/// interval containing the new dyadic contains the numerical value of the constructing number;
	/// 0 returns 0 and n = tolerance
	static basic_dyadic make_basic_dyadic(const basic_dyadic arg, const n_t tolerance)
	{
		n_t rel_tolerance = tolerance - arg.n;
		if (rel_tolerance >= 0)
		{
			k = shift(arg.k, rel_tolerance);
			n = tolerance;
		}
		else
		{
			auto ceil_positive = [](k_t k, n_t tolerance)->K_T {
				k_t twototolerance = int_two_to_int_power(tolerance);
				return (k / twototolerance) + ((k % twototolerance) ? 1 : 0);
			};
			auto floor_positive = [](k_t k, n_t tolerance) -> K_T {
				k_t twototolerance = int_two_to_int_power(tolerance);
				return (k / twototolerance);
			};

			switch (interval_t)
			{
			case opencl: //'(]' should return ceil
			{
				if (arg.k > 0) k = ceil_positive(arg.k, -rel_tolerance);
				else k = -floor_positive(-arg.k, -rel_tolerance);
			}
			break;
			case clopen: //'[)'
			{
				if (arg.k > 0) k = floor_positive(arg.k, -rel_tolerance);
				else k = -ceil_positive(-arg.k, -rel_tolerance);
			}
			break;
			}
			n = tolerance;
		}
	}
*/
	/// Outputs a dyadic interval in the form [double, double) to an std::ostream.
	friend inline std::ostream& operator << (std::ostream & os, const basic_dyadic_interval & rhs)
	{
		switch (rhs.intervaltype)
		{
		case clopen:	os << "[" << (double)rhs.inf() << ", " << (double)rhs.sup() << ")"; break;
		case opencl:	os << "(" << (double)rhs.inf() << ", " << (double)rhs.sup() << "]"; break;
		}
		return os;
	}

public:
	friend
		std::deque< basic_dyadic_interval >
		to_dyadic_intervals<interval_t, dyadic_t>(double inf, double sup, int tolerance, IntervalType);
};

template<IntervalType intervaltype, class SCALAR>
class basic_interval;

template<IntervalType Intervaltype, class DYADIC = dyadic>
std::deque<basic_dyadic_interval<Intervaltype, DYADIC> >
to_dyadic_intervals(double inf, double sup, int tolerance, IntervalType unused)
{
	constexpr IntervalType intervaltype = Intervaltype;
	typedef basic_dyadic_interval<intervaltype, DYADIC>  di;
	typedef DYADIC  dyadic_t;

	auto store_move = [](std::list<di> &intervals, di &begin)
	{
		intervals.push_back(begin.shrink_to_omitted_end());
		begin.k += di::unit;
	};

	auto store_ = [](std::list<di> &intervals, typename std::list<di>::iterator p, di end) -> typename std::list<di>::iterator
	{
		p = intervals.insert(p, end.shrink_to_contained_end());
		return p;
	};

	basic_interval <intervaltype, double> real{ inf, sup };

	di begin{ real.included_end(), tolerance }; // a dyadic interval with tolerance  containing the included end
	di end{ real.excluded_end(), tolerance }; // a dyadic interval with tolerance containing the excluded end

	std::list<di> intervals;
	for (; !begin.contains(end); )
	{
		auto next{ begin };
		next = next.expand_interval();
		if (!begin.aligned()) { store_move(intervals, next); };
		begin = next;
	}
	// expanding end will always stay in begin until it equals begin
	auto p = intervals.end();
	for (auto next{ end }; begin.contains(next.expand_interval()); )
	{
		if (!end.aligned()) { p = store_(intervals, p, next); }
		end = next;
	}

	if (intervaltype == opencl) intervals.reverse();

	return std::deque<basic_dyadic_interval<intervaltype, DYADIC> >(intervals.begin(), intervals.end());
}
// default
typedef basic_dyadic_interval<DEFAULT_DYADIC_INTERVAL_TYPE, dyadic> dyadic_interval;

////////////////////////////////////////////////////////////////////////////////////////
/// A basic_interval is a real interval differentiated by type and tagged internally as
/// clopen or opencl. Individual intervals are represented internally by their sup, inf,
/// and this tag. The template generates different types for clopen and opencl intervals
/// and they do not communicate.
template<IntervalType Intervaltype, class SCALAR>
class basic_interval {
public:
	typedef SCALAR scalar_t;
	static const IntervalType intervaltype{ Intervaltype };
	typedef Compare<intervaltype> compare;
	const compare cmp;

	scalar_t excluded_end() const { return (intervaltype == clopen) ? m_sup : m_inf; }
	scalar_t included_end() const { return (intervaltype == clopen) ? m_inf : m_sup; }
private:
	//scalar_t m_included_end = (intervaltype == clopen) ? m_sup : m_inf;
public:
	scalar_t inf() const { return m_inf; }
	scalar_t sup() const { return m_sup; }

	// tests to see if arg is in the interval
	bool contains(const scalar_t& arg) const { return (cmp(included_end(), arg) && cmp(arg, excluded_end())) || arg == included_end(); }

	// tests to see if the interval contains the included end of another interval
	// in a partition of an interval by intervals a new interval is associated
	// with at most one element of the partition
	bool is_associated(basic_interval arg) const { return contains(arg.included_end()); }

	// tests to see if one basic interval contains another of the same type
	bool contains(const basic_interval& arg) const 
	{ return (is_associated(arg) && ! arg.contains(excluded_end())); }

	// constructors
	basic_interval(std::pair < scalar_t, scalar_t > arg) :
		m_inf{ std::min(arg.first, arg.second) },
		m_sup{ std::max(arg.first, arg.second) },
		cmp {}
	{}
	basic_interval(scalar_t first, scalar_t second) :
		basic_interval{ std::make_pair(first, second) }
	{}
	template <class DYADIC>
	basic_interval(basic_dyadic_interval<intervaltype, DYADIC> arg)
		: basic_interval((scalar_t)arg.included_end(), (scalar_t)arg.excluded_end())
	{
	};

	//Assignment function, use with caution
	basic_interval& operator=(const basic_interval& other)
	{
		
		m_sup = other.sup();
		m_inf = other.inf();
		return *this;
	}

private:
	// Removed constness of these private members to allow for updating
	scalar_t m_inf;
	scalar_t m_sup;

	/// Outputs an interval to std::ostream in recognisable form.
	friend inline std::ostream& operator << (std::ostream & os, const basic_interval & rhs)
	{
		switch (rhs.intervaltype)
		{
		case clopen:	os << "[" << rhs.inf() << ", " << rhs.sup() << ")"; break;
		case opencl:	os << "(" << rhs.inf() << ", " << rhs.sup() << "]"; break;
		}
		return os;
	}
};
///////////////////////////////////////////////////////////////////////
#endif // __DYADIC_INTERVAL__
