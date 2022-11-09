/* *************************************************************

Copyright 2010-16 Terry Lyons, Stephen Buckley, Djalil Chafai,
Greg Gyurk� and Arend Janssen. Substantially modified by Terry
Lyons and Hao Ni.

Distributed under the terms of the GNU General Public License,
Version 3. (See accompanying file License.txt)

************************************************************* */

#pragma once

#ifndef __DYADIC__
#define __DYADIC__

#include <cmath> // ldexp() frexp() floor()
#include <limits> // numerical_limits
#include <cassert> // assert
#include <algorithm> // upper_bound
#include <iostream>

// Great care is needed when this type of code is used with negative numbers
// and to control overflow
// the original 2010 code did not do this

// WARNING % is not the modulo operator and -3%2 == -1

// WARNING if E1 has a signed type AND non-negative value, AND E1 � 2^E2
// is representable in the result type, then that is the resulting value of E1<<E2;
// otherwise, the behavior is undefined.

#ifndef DEFAULT_DYADIC_INTERVAL_TYPE
#define DEFAULT_DYADIC_INTERVAL_TYPE clopen
#endif

#ifndef DEFAULT_DYADIC_K_TYPE
#define DEFAULT_DYADIC_K_TYPE int
#endif

#ifndef DEFAULT_DYADIC_N_TYPE
#define DEFAULT_DYADIC_N_TYPE int
#endif

/// A dyadic number.

/// class to hold a dyadic number with constructors from
/// doubles, dyadic, and more
template<class K_T, class N_T>
class basic_dyadic
{
public:
	/// the intrinsic data
	typedef K_T k_t;
	typedef N_T n_t;
	/// The numerator - the numerical value is k * 2^-n)
	k_t k;
	/// 2^n is the denominator.
	n_t n;

	/// helper functions

	/// computes moduli correctly and in a sign independent way
	static k_t mod(k_t a, k_t b)
	{
		k_t r = a % b;
		return ((r < 0) ? r + abs(b) : r);
	}

	/// multiplies a signed integer k by 2^n throwing a debug exception if n is negative,  too big, or if there is an overflow
	static k_t shift(k_t k, n_t n)
	{
		k_t pwr = int_two_to_int_power(n);
		k_t ans = k * pwr;
		// assert no overflow
		assert((k == 0 || ans / k == pwr));
		return ans;
	}

	/// Compute positive powers of two throwing a debug exception if outside standard defined behavior
	static k_t int_two_to_int_power(n_t exponent)
	{
		assert(exponent < (n_t)std::numeric_limits<k_t>::digits);
		assert(exponent >= 0);
		return (k_t(1) << exponent);
	}

	/// Compiler generated copy constructor

	/// Default constructor
	basic_dyadic(void)
		: k(0)
		, n(0)
	{
	}

	/// Constructor from numerator and log2 denominator
	basic_dyadic(const k_t k1, const n_t n1)
		: k(k1)
		, n(n1)
	{
	}

	~basic_dyadic(void)
	{
	}

	/// Dyadic comparison
	bool static dyadic_equals(const basic_dyadic& lhs, const basic_dyadic& rhs)
	{
		return (lhs.k == rhs.k && lhs.n == rhs.n);
	}

	/// Rational comparison without unnecessary overflow
	bool static rational_equals(const basic_dyadic& lhs, const basic_dyadic& rhs)
	{
		k_t ratio;
		if (lhs.k % rhs.k == 0 && (ratio = (lhs.k / rhs.k)) >= 1)
		{
			k_t rel_tolerance = lhs.n - rhs.n;
			if (rel_tolerance < 0) return false;
			return ratio == int_two_to_int_power(rel_tolerance);
		}
		if (rhs.k % lhs.k == 0 && (ratio = (rhs.k / lhs.k)) >= 1)
		{
			k_t rel_tolerance = rhs.n - lhs.n;
			if (rel_tolerance < 0) return false;
			return ratio == int_two_to_int_power(rel_tolerance);
		}
		return false;
	}

	/// Move from k/2^n -> (k + Arg)/2^n.
	basic_dyadic & move_forward(const k_t Arg)
	{
		auto safeaddition = [](k_t a, k_t b) -> bool {return !(a > 0 && b > std::numeric_limits<k_t>::max() - a) && !(a < 0 && b < std::numeric_limits<k_t>::min() - a); };
		assert(safeaddition(k, Arg));
		k += Arg;
		return *this;
	}

	/// The next dyadic prefix.
	basic_dyadic & operator ++ ()
	{
		++k;
		return *this;
	}

	/// The next dyadic postfix.
	basic_dyadic operator ++ (int)
	{
		basic_dyadic temp = *this;
		++*this;
		return temp;
	}

	/// The previous dyadic prefix.
	basic_dyadic & operator -- ()
	{
		--k;
		return *this;
	}

	/// The previous dyadic postfix.
	basic_dyadic operator -- (int)
	{
		basic_dyadic temp = *this;
		--*this;
		return temp;
	}

		public:
		/// Rewrites a dyadic (k,n) with the largest representable N <= resolution so that there 
		/// is an integer K such that K*2^(-N) == k*2^(-n) 
		/// and returns true if N == resolution. It does no rounding!!
		bool rebase( n_t resolution = std::numeric_limits<n_t>::lowest())
	{
			//std::cout << "rebase " << *this << " from " << n << " to " << resolution;
			//auto& pr = [=]() {std::cout << " " << *this << " " << n << "\n"; };
	    // k==0 is special
		if (k == 0){
			n = resolution; 
			//pr();
			return true;
		}
		// already an integer relative to resolution - no cancellation performed
		if (resolution >= n || k == 0) {
			k = shift(k, resolution - n); 
			n = resolution;
			//pr();
			return true;
		}
		
		// beware overflow of (n - resolution)

		// k != 0 so the lowest cancelled form of this dyadic will always occur at a resolution > n - std::numeric_limits<k_t>::digits
		if (n >= std::numeric_limits<k_t>::digits + resolution)
			resolution = (n - std::numeric_limits<k_t>::digits) + 1;
		n_t rel_resolution{ n - resolution };

		// some integers 0-63
		static const n_t ints[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66 };

		// tests divisibility by a power of 2
		auto k_isdivisible = [this](n_t m) -> bool {
			auto ans = (k % int_two_to_int_power(m));
			return 0 == ans;
		};

		// special notion of compare
		auto compare = [&](n_t m1, n_t m2) -> bool {
			auto x = k_isdivisible(rel_resolution - m2);
			return x;
		};

		{
			// starting at relative resolution find the first n in decreasing order so that 2^n divides k
			// 2^0 always divides k so the action stops
			n_t offset = rel_resolution - (n_t)(std::upper_bound(ints, ints + rel_resolution, -22, compare) - ints);
			k /= int_two_to_int_power(offset);
			n -= offset;
			//pr();
			return resolution == n;
		}
	}

private:

	/// The operator equals is a challenging - in some ways one wants equals if k==k' and n==n' in others if k2^n == k'2^n' so we delete this function
	/// equals is not available; use dyadic_equals or rational_equals as appropriate.
	bool operator== (const basic_dyadic & Arg) = delete;

	// Numerical operators
public: // rde_lib2\DynamicallyConstructedPath.h(114)
	/// Conversion to double - note there can be loss of precision
	operator double() const
	{
		return ldexp((double)k, -n);
	}
private:
	/// Rational Comparison operators (Note these ignore the interaction of numerator and denominator)
	bool operator < (const basic_dyadic & Arg) const
	{
		return (n <= Arg.n) ? (k < shift(Arg.k, (Arg.n - n))) : shift(k, (n - Arg.n)) < Arg.k;
	}
	bool operator > (const basic_dyadic & Arg) const
	{
		return (n <= Arg.n) ? (k > shift(Arg.k, (Arg.n - n))) : shift(k, (n - Arg.n)) > Arg.k;
	}
	bool operator <= (const basic_dyadic & Arg) const
	{
		return (n <= Arg.n) ? (k <= shift(Arg.k, (Arg.n - n))) : shift(k, (n - Arg.n)) <= Arg.k;
	}
	bool operator >= (const basic_dyadic & Arg) const
	{
		return (n <= Arg.n) ? (k >= shift(Arg.k, (Arg.n - n))) : shift(k, (n - Arg.n)) >= Arg.k;
	}
};
typedef basic_dyadic<DEFAULT_DYADIC_K_TYPE, DEFAULT_DYADIC_N_TYPE> dyadic;
#endif // __DYADIC__
