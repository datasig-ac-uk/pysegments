//
// Created by user on 07/11/22.
//

#include "dyadic_searcher.h"

#include <cmath>
#include <unordered_map>
#include <iostream>

#include <gtest/gtest.h>

#include <boost/functional/hash.hpp>

using namespace segments;

struct interval_hash {
    std::size_t operator()(const interval& arg) const noexcept
    {
        boost::hash<std::pair<double, double>> dhash;
        return dhash({arg.inf(), arg.sup()});
    }
};

bool operator == (const interval& lhs, const interval& rhs) noexcept
{ return lhs.inf() == rhs.inf() && lhs.sup() == rhs.sup(); }


TEST(dyadic_search_tests, checks_all_intervals_negative_predicate)
{
    std::unordered_map<interval, int, interval_hash> hits;
    auto predicate = [&hits](const interval& itvl) {
        hits[itvl] += 1;
        return false;
    };

    dyadic_searcher search(predicate, 3);

    auto found = search(interval(0, 1));

    ASSERT_TRUE(found.empty());

    auto it = hits.find(interval(0, 1));
    ASSERT_NE(it, hits.end());
    ASSERT_EQ(it->second, 1) << it->first;

    for (depth_t d=1; d<=3; ++d) {
        for (mult_t k=0; k<(mult_t(1)<<d); ++k) {
            dyadic_interval di(k, d);
            interval idi(double(di.inf()), double(di.sup()));
            auto it = hits.find(idi);
            ASSERT_NE(it, hits.end());
            ASSERT_EQ(it->second, 1) << it->first;
        }
    }

}


TEST(dyadic_search_tests, check_interval_once_positive_predicate)
{
    std::unordered_map<interval, int, interval_hash> hits;
    auto predicate = [&hits](const interval& itvl) {
        hits[itvl] += 1;
        return true;
    };

    dyadic_searcher search(predicate, 3);
    auto found = search(interval(0, 1));

    ASSERT_EQ(found.size(), 1);
    ASSERT_EQ(found[0], interval(0, 1));

    ASSERT_EQ(hits.size(), 1);
    ASSERT_EQ(hits.begin()->first, found[0]);
    ASSERT_EQ(hits.begin()->second, 1);
}

TEST(dyadic_search_tests, check_025_075_middle_interval)
{
    std::unordered_map<interval, int, interval_hash> hits;
    auto predicate = [&hits](const interval& itvl) {
        hits[itvl] += 1;

        return itvl.inf() >= 0.25 && itvl.sup() <= 0.75;
    };

    dyadic_searcher search(predicate, 3);
    auto found = search(interval(0, 1));

    ASSERT_EQ(found.size(), 1);
    ASSERT_EQ(found[0], interval(0.25, 0.75));

    for (const auto& hit : hits) {
        ASSERT_LE(hit.second, 2) << hit.first;
    }
}

TEST(dyadic_search_tests, check_two_distinct_intervals)
{
    std::unordered_map<interval, int, interval_hash> hits;
    auto predicate = [&hits](const interval& itvl) {
        hits[itvl] += 1;

        return (itvl.inf() >= 0.1 && itvl.sup() <= 0.35)
                || (itvl.inf() >= 0.55 && itvl.sup() <= 0.81);
    };

    dyadic_searcher search(predicate, 3);
    auto found = search(interval(0, 1));

    ASSERT_EQ(found.size(), 2);
    ASSERT_EQ(found[0].inf(), 0.125);
    ASSERT_EQ(found[0].sup(), 0.25);
    ASSERT_EQ(found[1].inf(), 0.625);
    ASSERT_EQ(found[1].sup(), 0.75);
}

TEST(dyadic_search_tests, check_multiple_small_intervals_high_depth)
{
    std::unordered_map<interval, int, interval_hash> hits;
    auto predicate = [&hits](const interval& itvl) {
        hits[itvl] += 1;

        return (itvl.inf() >= 0.0234 && itvl.sup() <= 0.09523)
                || (itvl.inf() >= 0.1042 && itvl.sup() <= 0.1093)
                || (itvl.inf() >= 0.1252 && itvl.sup() <= 0.1301)
                || (itvl.inf() >= 0.1354 && itvl.sup() <= 0.2252)
                || (itvl.inf() >= 0.2852 && itvl.sup() <= 0.3401)
                || (itvl.inf() >= 0.3405 && itvl.sup() <= 0.3509)
                || (itvl.inf() >= 0.3791 && itvl.sup() <= 0.4411)
                || (itvl.inf() >= 0.4925 && itvl.sup() <= 0.5995)
                || (itvl.inf() >= 0.6013 && itvl.sup() <= 0.6521)
                || (itvl.inf() >= 0.6525 && itvl.sup() <= 0.6599)
                || (itvl.inf() >= 0.7354 && itvl.sup() <= 0.8023)
                || (itvl.inf() >= 0.8154 && itvl.sup() <= 0.8832)
                || (itvl.inf() >= 0.9021 && itvl.sup() <= 0.9411)
                ;
    };

    dyadic_searcher search(predicate, 10);
    auto found = search(interval(0, 1));
    ASSERT_EQ(found.size(), 13);

    for (const auto& counts : hits) {
        EXPECT_LE(counts.second, 2) << counts.first;
    }

    for (const auto& it : found) {
        ASSERT_TRUE(predicate(it));
    }




}


TEST(dyadic_search_tests, check_single_irregular_interval_high_depth)
{
    std::unordered_map<interval, int, interval_hash> hits;
    auto predicate = [&hits](const interval& itvl) {
        hits[itvl] += 1;

        return (itvl.inf() >= 0.4523 && itvl.sup() <= 0.6923);
    };

    dyadic_searcher search(predicate, 9);
    auto found = search(interval(0, 1));


    ASSERT_EQ(found.size(), 1);
}


TEST(dyadic_search_tests, check_single_interval_scaled_low_depth)
{
    auto predicate = [](const segments::interval& arg) {
        return 3.14159265358979323846 <= arg.inf() && arg.sup() <= 2*3.1415926535897932384;
    };


    dyadic_searcher search(predicate, 3);
    auto found = search(interval(0.0, 10.0));


    ASSERT_EQ(found.size(), 1);
}

TEST(dyadic_search_tests, check_single_interval_scaled_high_depth)
{
    auto predicate = [](const segments::interval& arg) {
        return 3.14159265358979323846 <= arg.inf() && arg.sup() <= 2*3.1415926535897932384;
    };


    dyadic_searcher search(predicate, 10);
    interval base(0.0, 10.0);
    auto found = search(interval(0.0, 10.0));


    ASSERT_EQ(found.size(), 1);
    EXPECT_NEAR(found[0].inf(), 3.14159265358979323846, std::ldexp(base.sup() - base.inf(), -10));
    EXPECT_NEAR(found[0].sup(), 2*3.14159265358979323846, std::ldexp(base.sup() - base.inf(), -10));
}


TEST(dyadic_search_tests, check_multiple_intervals_scaled_low_depth)
{
    auto predicate = [](const segments::interval& arg) {
        return (arg.inf() >= 0.234 && arg.sup() <= 0.9523)
                || (arg.inf() >= 1.042 && arg.sup() <= 1.093)
                || (arg.inf() >= 1.252 && arg.sup() <= 1.301)
                || (arg.inf() >= 1.354 && arg.sup() <= 2.252)
                || (arg.inf() >= 2.852 && arg.sup() <= 3.401)
                || (arg.inf() >= 3.405 && arg.sup() <= 3.509)
                || (arg.inf() >= 3.791 && arg.sup() <= 4.411)
                || (arg.inf() >= 4.925 && arg.sup() <= 5.995)
                || (arg.inf() >= 6.013 && arg.sup() <= 6.521)
                || (arg.inf() >= 6.525 && arg.sup() <= 6.599)
                || (arg.inf() >= 7.354 && arg.sup() <= 8.023)
                || (arg.inf() >= 8.154 && arg.sup() <= 8.832)
                || (arg.inf() >= 9.021 && arg.sup() <= 9.411)
                ;
    };


    dyadic_searcher search(predicate, 2);
    auto found = search(interval(0.0, 10.0));

    EXPECT_LE(found.size(), 13);
}

TEST(dyadic_search_tests, check_multiple_intervals_scaled_high_depth)
{
    auto predicate = [](const segments::interval& arg) {
        return (arg.inf() >= 0.234 && arg.sup() <= 0.9523)
                || (arg.inf() >= 1.042 && arg.sup() <= 1.093)
                || (arg.inf() >= 1.252 && arg.sup() <= 1.301)
                || (arg.inf() >= 1.354 && arg.sup() <= 2.252)
                || (arg.inf() >= 2.852 && arg.sup() <= 3.401)
                || (arg.inf() >= 3.405 && arg.sup() <= 3.509)
                || (arg.inf() >= 3.791 && arg.sup() <= 4.411)
                || (arg.inf() >= 4.925 && arg.sup() <= 5.995)
                || (arg.inf() >= 6.013 && arg.sup() <= 6.521)
                || (arg.inf() >= 6.525 && arg.sup() <= 6.599)
                || (arg.inf() >= 7.354 && arg.sup() <= 8.023)
                || (arg.inf() >= 8.154 && arg.sup() <= 8.832)
                || (arg.inf() >= 9.021 && arg.sup() <= 9.411)
                ;
    };


    dyadic_searcher search(predicate, 10);
    auto found = search(interval(0.0, 10.0));

    EXPECT_LE(found.size(), 13);
}
