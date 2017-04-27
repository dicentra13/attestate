#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include "../src/diff.h"
#include "helpers.h"

#include <map>
#include <string>
#include <initializer_list>

using namespace attestate;

// tests of helper class Diff

BOOST_AUTO_TEST_SUITE(diff_tests)

typedef Diff<std::string, int> DiffT;

void checkDiff(const DiffT::DiffType& rdiff,const DiffT::DiffType& ediff)
{
    BOOST_REQUIRE(rdiff.size() == ediff.size());
    auto ite = ediff.begin();
    auto itr = rdiff.begin();
    for ( ; ite != ediff.end(); ++ite, ++itr) {
        BOOST_CHECK(ite->first == itr->first);
        BOOST_CHECK(equals(ite->second.first, itr->second.first));
        BOOST_CHECK(equals(ite->second.second, itr->second.second));
    }
}

void checkValues(const DiffT::ValuesType& rv,const DiffT::ValuesType& ev)
{
    BOOST_REQUIRE(rv.size() == ev.size());
    auto ite = ev.begin();
    auto itr = rv.begin();
    for ( ; ite != ev.end(); ++ite, ++itr) {
        BOOST_CHECK(ite->first == itr->first);
        BOOST_CHECK(ite->second == itr->second);
    }
}

BOOST_AUTO_TEST_CASE(test_diff_1)
{
    DiffT::ValuesType v1 = {{"a", 0}};
    DiffT::ValuesType v2 = {};
    DiffT::ValuesType v3 = v1;

    checkDiff(DiffT::compute(v1, v1), {});

    auto diff = DiffT::compute(v1, v2);
    checkDiff(diff, DiffT::DiffType{{"a", {0, boost::none}}});
    auto rdiff2 = DiffT::compute(v2, v1);

    DiffT::apply(v1, diff);
    checkDiff(DiffT::compute(v1, v2), {});
    checkValues(v1, v2);

    auto rdiff = DiffT::reverse(diff);
    checkDiff(rdiff, DiffT::DiffType{{"a", {boost::none, 0}}});
    checkDiff(rdiff, rdiff2);

    DiffT::apply(v1, rdiff);
    checkDiff(DiffT::compute(v1, v3), {});
    checkValues(v1, v3);

    v2["a"] = 1;

    diff = DiffT::compute(v1, v2);
    checkDiff(diff, DiffT::DiffType{{"a", {0, 1}}});
    rdiff2 = DiffT::compute(v2, v1);

    DiffT::apply(v1, diff);
    checkDiff(DiffT::compute(v1, v2), {});
    checkValues(v1, v2);

    rdiff = DiffT::reverse(diff);
    checkDiff(rdiff, DiffT::DiffType{{"a", {1, 0}}});
    checkDiff(rdiff, rdiff2);

    DiffT::apply(v1, rdiff);
    checkDiff(DiffT::compute(v1, v3), {});
    checkValues(v1, v3);
}

BOOST_AUTO_TEST_CASE(test_diff_2)
{
    DiffT::ValuesType v1 = {
        {"a", 0},
        {"b", 1},
        {"d", 2},
        {"f", 4},
        {"g", 5}
    };
    DiffT::ValuesType v2 = {
        {"a", 0},
        {"b", 0},
        {"c", 2},
        {"e", 3},
        {"g", 6}
    };
    DiffT::ValuesType v3 = v1;

    checkDiff(DiffT::compute(v1, v1), {});
    checkDiff(DiffT::compute(v2, v2), {});

    auto diff = DiffT::compute(v1, v2);
    checkDiff(diff, DiffT::DiffType{
        {"b", {1, 0}},
        {"c", {boost::none, 2}},
        {"d", {2, boost::none}},
        {"e", {boost::none, 3}},
        {"f", {4, boost::none}},
        {"g", {5, 6}}
    });

    DiffT::apply(v1, diff);
    checkDiff(DiffT::compute(v1, v2), {});
    checkValues(v1, v2);

    auto rdiff = DiffT::reverse(diff);
    checkDiff(rdiff, DiffT::DiffType{
        {"b", {0, 1}},
        {"c", {2, boost::none}},
        {"d", {boost::none, 2}},
        {"e", {3, boost::none}},
        {"f", {boost::none, 4}},
        {"g", {6, 5}}
    });

    DiffT::apply(v1, rdiff);
    checkDiff(DiffT::compute(v1, v3), {});
    checkValues(v1, v3);
}

BOOST_AUTO_TEST_CASE(test_error)
{
    {
        DiffT::ValuesType v = {};
        DiffT::DiffType d = {{"a", {0, 1}}};
        BOOST_CHECK_THROW(DiffT::apply(v, d), Exception);
        checkValues(v, {});
    }
    {
        DiffT::ValuesType v = {{"a", 0}};
        DiffT::DiffType d = {{"a", {1, 0}}};
        BOOST_CHECK_THROW(DiffT::apply(v, d), Exception);
        checkValues(v, {{"a", 0}});
    }
    {
        DiffT::ValuesType v = {{"a", 0}};
        DiffT::DiffType d = {{"a", {0, 0}}};
        BOOST_CHECK_THROW(DiffT::apply(v, d), Exception);
        checkValues(v, {{"a", 0}});
    }
    {
        DiffT::ValuesType v = {{"a", 0}};
        DiffT::DiffType d = {{"a", {boost::none, 0}}};
        BOOST_CHECK_THROW(DiffT::apply(v, d), Exception);
        checkValues(v, {{"a", 0}});
    }
    {
        DiffT::ValuesType v = {};
        DiffT::DiffType d = {{"a", {boost::none, boost::none}}};
        BOOST_CHECK_THROW(DiffT::apply(v, d), Exception);
        checkValues(v, {});
    }
}

BOOST_AUTO_TEST_SUITE_END()
