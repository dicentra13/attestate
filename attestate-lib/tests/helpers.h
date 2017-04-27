#pragma once

#include "../src/helpers.h"

#include <attestate/common.h>

#include <boost/optional.hpp>
#include <boost/test/unit_test.hpp>

#include <iostream>

#include <QApplication>

struct AppFixture {
    AppFixture()
        : app(
              boost::unit_test::framework::master_test_suite().argc,
              boost::unit_test::framework::master_test_suite().argv)
    {}

    QApplication app;
};


// for adl

namespace std {

using attestate::operator <<;

template <class T>
std::ostream& operator << (std::ostream& o, const boost::optional<T>& v)
{
    if (v) {
        o << *v;
    } else {
        o << "none";
    }
    return o;
}

} // namespace std;

template <class T>
bool equals(const boost::optional<T>& l, const boost::optional<T>& r)
{
    return (!l && !r) || (l &&r && *l == *r);
}

