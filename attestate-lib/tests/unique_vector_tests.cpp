#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include "../src/unique_vector.h"

#include <string>
#include <memory>
#include <initializer_list>

using namespace attestate;

// tests of helper class UniqueVector

// with copyable type

BOOST_AUTO_TEST_SUITE(unique_vector_tests_copyable)

typedef UniqueVector<std::string> StringVector;

void checkVector(const StringVector& v, std::initializer_list<std::string> exp)
{
    BOOST_REQUIRE_MESSAGE(
        v.size() == exp.size(),
        "Size mismatch, expected " << exp.size() << ", received " << v.size());

    BOOST_CHECK(v.empty() == (exp.size() == 0));

    size_t at = 0;
    for (auto i : exp) {
        BOOST_CHECK_MESSAGE(
            v.at(at) == i,
            "Value mismatch at " << at << ", expected " << i << ", received " << v.at(at));
        ++at;
        BOOST_CHECK_MESSAGE(v.contains(i), "Value " << i << " is expected");
    }
}

BOOST_AUTO_TEST_CASE(test_create_empty)
{
    StringVector v;
    checkVector(v, {});
}

BOOST_AUTO_TEST_CASE(test_create_container)
{
    std::vector<std::string> c = {"3", "-3", "0", "4"};
    StringVector v(c);
    checkVector(v, {"3", "-3", "0", "4"});
}

BOOST_AUTO_TEST_CASE(test_create_duplicate_key)
{
    std::vector<std::string> c = {"3", "3", "0", "4"};
    std::unique_ptr<StringVector> v;
    BOOST_CHECK_THROW(v.reset(new StringVector(c)), Exception);
}

BOOST_AUTO_TEST_CASE(test_copyable)
{
    std::vector<std::string> c = {"3", "-3", "0", "4"};
    std::unique_ptr<StringVector> v0(new StringVector(c));

    // construct
    StringVector v(*v0);
    v0.reset();
    checkVector(v, {"3", "-3", "0", "4"});

    // assign
    std::vector<std::string> c2 = {"0", "1", "2"};
    std::unique_ptr<StringVector> v1(new StringVector(c2));
    v = *v1;
    v1.reset();
    checkVector(v, {"0", "1", "2"});
}

BOOST_AUTO_TEST_CASE(test_append)
{
    std::vector<std::string> c = {"0", "1", "2"};
    StringVector v0(c);
    v0.append("3");
    checkVector(v0, {"0", "1", "2", "3"});

    StringVector v;
    v.append("0");
    v.append("4");
    v.append("5");
    checkVector(v, {"0", "4", "5"});
}

BOOST_AUTO_TEST_CASE(test_append_error)
{
    std::vector<std::string> c = {"0", "1", "2"};
    StringVector v(c);
    BOOST_CHECK_THROW(v.append("2"), Exception); // dup key
    checkVector(v, {"0", "1", "2"});
}

BOOST_AUTO_TEST_CASE(test_insert)
{
    std::vector<std::string> c = {"1"};
    StringVector v(c);
    v.insert("0", 0);
    v.insert("4", 2);
    v.insert("3", 2);
    checkVector(v, {"0", "1", "3", "4"});

    std::vector<std::string> c1 = {"1", "4", "5"};
    StringVector v1(c1);
    std::map<StringVector::Index, std::string> values = {
        {0, "0"}, {2, "2"}, {3, "3"}, {6, "6"}, {7, "7"}
    };
    v1.insert(values);
    checkVector(v1, {"0", "1", "2", "3", "4", "5", "6", "7"});
}

BOOST_AUTO_TEST_CASE(test_insert_error)
{
    {
        StringVector v;
        BOOST_CHECK_THROW(v.insert("2", 1), Exception); // invalid index
        checkVector(v, {});
    }
    {
        StringVector v(std::vector<std::string>{"1", "-2"});
        BOOST_CHECK_THROW(v.insert("-2", 0), Exception); // duplicate key
        checkVector(v, {"1", "-2"});
    }
    {
        StringVector v(std::vector<std::string>{"1", "2"});
        BOOST_CHECK_THROW(v.insert("-2", 3), Exception); // invalid index
        checkVector(v, {"1", "2"});
    }
    {
        StringVector v;
        BOOST_CHECK_THROW(v.insert({{0, "0"}, {1, "0"}}), Exception); // dup key
        checkVector(v, {});
    }
    {
        std::vector<std::string> c = {"1", "4", "5"};
        StringVector v(c);
        BOOST_CHECK_THROW(v.insert({{0, "0"}, {2, "1"}}), Exception); // dup key
        checkVector(v, {"1", "4", "5"});
    }
    {
        StringVector v;
        BOOST_CHECK_THROW(v.insert({{0, "2"}, {2, "0"}}), Exception); // invalid index
        checkVector(v, {});
    }
    {
        std::vector<std::string> c = {"1", "4", "5"};
        StringVector v(c);
        BOOST_CHECK_THROW(v.insert({{0, "0"}, {8, "2"}}), Exception); // invalid index
        checkVector(v, {"1", "4", "5"});
    }
}

void checkRemoveResult(
    const std::map<StringVector::Index, std::string>& exp,
    const std::map<StringVector::Index, std::string>& recv)
{
    BOOST_REQUIRE(recv.size() == exp.size());
    auto itr = recv.begin();
    auto ite = exp.begin();
    for ( ; itr != recv.end(); ++itr, ++ite) {
        BOOST_CHECK(itr->first == ite->first);
        BOOST_CHECK(itr->second == ite->second);
    }
}

BOOST_AUTO_TEST_CASE(test_remove)
{
    std::vector<std::string> c = {"1", "2", "3", "4"};
    StringVector v(c);
    BOOST_CHECK(v.remove(0) == "1");
    checkVector(v, {"2", "3", "4"});
    BOOST_CHECK(v.remove(1) == "3");
    checkVector(v, {"2", "4"});
    BOOST_CHECK(v.remove(1) == "4");
    checkVector(v, {"2"});
    BOOST_CHECK(v.remove(0) == "2");
    checkVector(v, {});

    std::vector<std::string> c1 = {"0", "1", "2", "3", "4", "5", "6"};
    StringVector v1(c1);
    std::set<StringVector::Index> at = {0, 2, 3, 6};
    auto recv = v1.remove(at);
    checkVector(v1, {"1", "4", "5"});
    checkRemoveResult({{0, "0"}, {2, "2"}, {3, "3"}, {6, "6"}}, recv);
    std::set<StringVector::Index> at2 = {0, 1, 2};
    auto recv2 = v1.remove(at2);
    checkVector(v1, {});
    checkRemoveResult({{0, "1"}, {1, "4"}, {2, "5"}}, recv2);
}

BOOST_AUTO_TEST_CASE(test_remove_error)
{
    {
        StringVector v;
        BOOST_CHECK_THROW(v.remove(0), Exception);
    }
    {
        std::vector<std::string> c = {"1", "2", "3", "4"};
        StringVector v(c);
        BOOST_CHECK_THROW(v.remove(4), Exception);
        checkVector(v, {"1", "2", "3", "4"});
    }
    {
        StringVector v;
        BOOST_CHECK_THROW(v.remove({0, 1, 2}), Exception);
    }
    {
        std::vector<std::string> c = {"1", "2", "3", "4"};
        StringVector v(c);
        BOOST_CHECK_THROW(v.remove({0, 1, 8}), Exception);
        checkVector(v, {"1", "2", "3", "4"});
    }
}


BOOST_AUTO_TEST_CASE(test_move)
{
    std::vector<std::string> c = {"0", "1", "2", "3", "4", "5", "6"};
    StringVector v(c);

    v.move(0, 2);
    checkVector(v, {"1", "2", "0", "3", "4", "5", "6"});
    v.move(2, 0);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(3, 6);
    checkVector(v, {"0", "1", "2", "4", "5", "6", "3"});
    v.move(6, 3);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(0, 6);
    checkVector(v, {"1", "2", "3", "4", "5", "6", "0"});
    v.move(6, 0);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(3, 4);
    checkVector(v, {"0", "1", "2", "4", "3", "5", "6"});
    v.move(4, 3);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});
}

BOOST_AUTO_TEST_CASE(test_move_error)
{
    {
        StringVector v;
        BOOST_CHECK_THROW(v.move(0, 1), Exception);
        checkVector(v, {});
    }
    {
        std::vector<std::string> c = {"0", "1", "2"};
        StringVector v(c);
        BOOST_CHECK_THROW(v.move(0, 3), Exception);
        checkVector(v, {"0", "1", "2"});
        BOOST_CHECK_THROW(v.move(3, 0), Exception);
        checkVector(v, {"0", "1", "2"});
    }
}

BOOST_AUTO_TEST_SUITE_END()

// K, V with movable type

BOOST_AUTO_TEST_SUITE(unique_vector_tests_movable)

typedef std::unique_ptr<std::string> StringPtr;

class Key {
public:
    explicit Key(const StringPtr& p) : k_(*p) {}

    const std::string& operator () () const { return k_; }

    bool operator < (const Key& o) const { return k_ < o.k_; }

private:
    std::string k_;
};

std::ostream& operator << (std::ostream& s, const Key& k)
{
    s << k();
    return s;
}

typedef UniqueVector<StringPtr, Key> StringPtrVector;

StringPtrVector createVector(std::initializer_list<std::string> l)
{
    std::vector<StringPtr> c;
    for (auto i : l) {
        c.push_back(StringPtr(new std::string(i)));
    }
    return StringPtrVector(std::move(c));
}

void checkVector(const StringPtrVector& v, std::initializer_list<std::string> exp)
{
    BOOST_REQUIRE_MESSAGE(
        v.size() == exp.size(),
        "Size mismatch, expected " << exp.size() << ", received " << v.size());

    BOOST_CHECK(v.empty() == (exp.size() == 0));

    size_t at = 0;
    for (auto i : exp) {
        BOOST_CHECK_MESSAGE(
            *v.at(at) == i,
            "Value mismatch at " << at << ", expected " << i << ", received " << *v.at(at));
        ++at;
    }
}

BOOST_AUTO_TEST_CASE(test_create_empty)
{
    StringPtrVector v;
    checkVector(v, {});
}

BOOST_AUTO_TEST_CASE(test_create_container)
{
    std::vector<StringPtr> c;
    for (auto i : {"3", "-3", "0", "4"}) {
        c.push_back(StringPtr(new std::string(i)));
    }
    StringPtrVector v(std::move(c));
    checkVector(v, {"3", "-3", "0", "4"});
}

BOOST_AUTO_TEST_CASE(test_create_duplicate_key)
{
    std::vector<StringPtr> c;
    for (auto i : {"3", "3", "0", "4"}) {
        c.push_back(StringPtr(new std::string(i)));
    }
    std::unique_ptr<StringPtrVector> v;
    BOOST_CHECK_THROW(v.reset(new StringPtrVector(std::move(c))), Exception);
}

BOOST_AUTO_TEST_CASE(test_movable)
{
    // construct
    StringPtrVector v0 = createVector({"3", "-3", "0", "4"});
    StringPtrVector v(std::move(v0));
    checkVector(v, {"3", "-3", "0", "4"});

    // assign
    StringPtrVector v1 = createVector({"0", "1", "2"});
    v = std::move(v1);
    checkVector(v, {"0", "1", "2"});
}

BOOST_AUTO_TEST_CASE(test_append)
{
    StringPtrVector v0 = createVector({"0", "1", "2"});
    v0.append(StringPtr(new std::string("3")));
    checkVector(v0, {"0", "1", "2", "3"});

    StringPtrVector v;
    v.append(StringPtr(new std::string("0")));
    v.append(StringPtr(new std::string("4")));
    v.append(StringPtr(new std::string("5")));
    checkVector(v, {"0", "4", "5"});
}

BOOST_AUTO_TEST_CASE(test_append_error)
{
    StringPtrVector v = createVector({"0", "1", "2"});
    BOOST_CHECK_THROW(v.append(StringPtr(new std::string("2"))), Exception); // dup key
    checkVector(v, {"0", "1", "2"});
}


BOOST_AUTO_TEST_CASE(test_insert)
{
    StringPtrVector v = createVector({"1"});
    v.insert(StringPtr(new std::string("0")), 0);
    v.insert(StringPtr(new std::string("4")), 2);
    v.insert(StringPtr(new std::string("3")), 2);
    checkVector(v, {"0", "1", "3", "4"});

    StringPtrVector v1 = createVector({"1", "4", "5"});
    typedef std::map<StringPtrVector::Index, StringPtr>::value_type Pair;
    std::map<StringPtrVector::Index, StringPtr> values;
    { Pair p ={0, std::move(StringPtr(new std::string("0")))}; values.insert(std::move(p)); }
    { Pair p ={2, std::move(StringPtr(new std::string("2")))}; values.insert(std::move(p)); }
    { Pair p ={3, std::move(StringPtr(new std::string("3")))}; values.insert(std::move(p)); }
    { Pair p ={6, std::move(StringPtr(new std::string("6")))}; values.insert(std::move(p)); }
    v1.insert(std::move(values));
    checkVector(v1, {"0", "1", "2", "3", "4", "5", "6"});
}

BOOST_AUTO_TEST_CASE(test_insert_error)
{
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.insert(StringPtr(new std::string("2")), 1), Exception); // invalid index
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"1", "-2"});
        BOOST_CHECK_THROW(v.insert(StringPtr(new std::string("-2")), 0), Exception); // duplicate key
        checkVector(v, {"1", "-2"});
    }
    {
        StringPtrVector v = createVector({"1", "2"});
        BOOST_CHECK_THROW(v.insert(StringPtr(new std::string("-2")), 3), Exception); // invalid index
        checkVector(v, {"1", "2"});
    }
    {
        StringPtrVector v;
        typedef std::map<StringPtrVector::Index, StringPtr> M;
        M m;
        { M::value_type p = {0, StringPtr(new std::string("0"))}; m.insert(std::move(p)); }
        { M::value_type p = {1, StringPtr(new std::string("0"))}; m.insert(std::move(p)); }
        BOOST_CHECK_THROW(v.insert(std::move(m)), Exception); // dup key
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"1", "4", "5"});
        typedef std::map<StringPtrVector::Index, StringPtr> M;
        M m;
        { M::value_type p = {0, StringPtr(new std::string("0"))}; m.insert(std::move(p)); }
        { M::value_type p = {2, StringPtr(new std::string("1"))}; m.insert(std::move(p)); }
        BOOST_CHECK_THROW(v.insert(std::move(m)), Exception); // dup key
        checkVector(v, {"1", "4", "5"});
    }
    {
        StringPtrVector v;
        typedef std::map<StringPtrVector::Index, StringPtr> M;
        M m;
        { M::value_type p = {0, StringPtr(new std::string("2"))}; m.insert(std::move(p)); }
        { M::value_type p = {2, StringPtr(new std::string("0"))}; m.insert(std::move(p)); }
        BOOST_CHECK_THROW(v.insert(std::move(m)), Exception); // invalid index
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"1", "4", "5"});
        typedef std::map<StringPtrVector::Index, StringPtr> M;
        M m;
        { M::value_type p = {0, StringPtr(new std::string("0"))}; m.insert(std::move(p)); }
        { M::value_type p = {8, StringPtr(new std::string("2"))}; m.insert(std::move(p)); }
        BOOST_CHECK_THROW(v.insert(std::move(m)), Exception); // invalid index
        checkVector(v, {"1", "4", "5"});
    }
}

void checkRemoveResult(
    const std::map<size_t, std::string>& exp,
    const std::map<StringPtrVector::Index, StringPtr>& recv)
{
    BOOST_REQUIRE(recv.size() == exp.size());
    auto itr = recv.begin();
    auto ite = exp.begin();
    for ( ; itr != recv.end(); ++itr, ++ite) {
        BOOST_CHECK(itr->first == ite->first);
        BOOST_CHECK(*itr->second == ite->second);
    }
}

BOOST_AUTO_TEST_CASE(test_remove)
{
    StringPtrVector v = createVector({"1", "2", "3", "4"});
    BOOST_CHECK(*v.remove(0) == "1");
    checkVector(v, {"2", "3", "4"});
    BOOST_CHECK(*v.remove(1) == "3");
    checkVector(v, {"2", "4"});
    BOOST_CHECK(*v.remove(1) == "4");
    checkVector(v, {"2"});
    BOOST_CHECK(*v.remove(0) == "2");
    checkVector(v, {});

    StringPtrVector v1 = createVector({"0", "1", "2", "3", "4", "5", "6"});
    std::set<StringPtrVector::Index> at = {0, 2, 3, 6};
    auto recv = v1.remove(at);
    checkVector(v1, {"1", "4", "5"});
    checkRemoveResult({{0, "0"}, {2, "2"}, {3, "3"}, {6, "6"}}, recv);
    std::set<StringPtrVector::Index> at2 = {0, 1, 2};
    auto recv2 = v1.remove(at2);
    checkVector(v1, {});
    checkRemoveResult({{0, "1"}, {1, "4"}, {2, "5"}}, recv2);
}

BOOST_AUTO_TEST_CASE(test_remove_error)
{
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.remove(0), Exception);
    }
    {
        StringPtrVector v = createVector({"1", "2", "3", "4"});
        BOOST_CHECK_THROW(v.remove(4), Exception);
        checkVector(v, {"1", "2", "3", "4"});
    }
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.remove({0, 1, 2}), Exception);
    }
    {
        StringPtrVector v = createVector({"1", "2", "3", "4"});
        BOOST_CHECK_THROW(v.remove({0, 1, 8}), Exception);
        checkVector(v, {"1", "2", "3", "4"});
    }
}

BOOST_AUTO_TEST_CASE(test_move)
{
    StringPtrVector v = createVector({"0", "1", "2", "3", "4", "5", "6"});

    v.move(0, 2);
    checkVector(v, {"1", "2", "0", "3", "4", "5", "6"});
    v.move(2, 0);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(3, 6);
    checkVector(v, {"0", "1", "2", "4", "5", "6", "3"});
    v.move(6, 3);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(0, 6);
    checkVector(v, {"1", "2", "3", "4", "5", "6", "0"});
    v.move(6, 0);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(3, 4);
    checkVector(v, {"0", "1", "2", "4", "3", "5", "6"});
    v.move(4, 3);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});
}

BOOST_AUTO_TEST_CASE(test_move_error)
{
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.move(0, 1), Exception);
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"0", "1", "2"});
        BOOST_CHECK_THROW(v.move(0, 3), Exception);
        checkVector(v, {"0", "1", "2"});
        BOOST_CHECK_THROW(v.move(3, 0), Exception);
        checkVector(v, {"0", "1", "2"});
    }
}

BOOST_AUTO_TEST_SUITE_END()

// K, V with copyable type

BOOST_AUTO_TEST_SUITE(unique_vector_tests_map_copyable)

typedef std::shared_ptr<std::string> StringPtr;

class Key {
public:
    explicit Key(const StringPtr& p) : k_(*p) {}

    const std::string& operator () () const { return k_; }

    bool operator < (const Key& o) const { return k_ < o.k_; }

private:
    std::string k_;
};

std::ostream& operator << (std::ostream& s, const Key& k)
{
    s << k();
    return s;
}

typedef UniqueVector<StringPtr, Key> StringPtrVector;

StringPtrVector createVector(std::initializer_list<std::string> l)
{
    std::vector<StringPtr> c;
    for (auto i : l) {
        c.push_back(std::make_shared<std::string>(i));
    }
    return StringPtrVector(c);
}

void checkVector(const StringPtrVector& v, std::initializer_list<std::string> exp)
{
    BOOST_REQUIRE_MESSAGE(
        v.size() == exp.size(),
        "Size mismatch, expected " << exp.size() << ", received " << v.size());

    BOOST_CHECK(v.empty() == (exp.size() == 0));

    size_t at = 0;
    for (auto i : exp) {
        BOOST_CHECK_MESSAGE(
            *v.at(at) == i,
            "Value mismatch at " << at << ", expected " << i << ", received " << *v.at(at));
        ++at;
    }
}

BOOST_AUTO_TEST_CASE(test_create_empty)
{
    StringPtrVector v;
    checkVector(v, {});
}

BOOST_AUTO_TEST_CASE(test_create_container)
{
    std::vector<StringPtr> c;
    for (auto i : {"3", "-3", "0", "4"}) {
        c.push_back(std::make_shared<std::string>(i));
    }
    StringPtrVector v(c);
    checkVector(v, {"3", "-3", "0", "4"});
}

BOOST_AUTO_TEST_CASE(test_create_duplicate_key)
{
    std::vector<StringPtr> c;
    for (auto i : {"3", "3", "0", "4"}) {
        c.push_back(std::make_shared<std::string>(i));
    }
    std::unique_ptr<StringPtrVector> v;
    BOOST_CHECK_THROW(v.reset(new StringPtrVector(c)), Exception);
}

BOOST_AUTO_TEST_CASE(test_copyable)
{
    // construct
    StringPtrVector v0 = createVector({"3", "-3", "0", "4"});
    StringPtrVector v(v0);
    checkVector(v, {"3", "-3", "0", "4"});

    // assign
    StringPtrVector v1 = createVector({"0", "1", "2"});
    v = v1;
    checkVector(v, {"0", "1", "2"});
}

BOOST_AUTO_TEST_CASE(test_append)
{
    StringPtrVector v0 = createVector({"0", "1", "2"});
    v0.append(std::make_shared<std::string>("3"));
    checkVector(v0, {"0", "1", "2", "3"});

    StringPtrVector v;
    v.append(std::make_shared<std::string>("0"));
    v.append(std::make_shared<std::string>("4"));
    v.append(std::make_shared<std::string>("5"));
    checkVector(v, {"0", "4", "5"});
}

BOOST_AUTO_TEST_CASE(test_append_error)
{
    StringPtrVector v = createVector({"0", "1", "2"});
    BOOST_CHECK_THROW(v.append(std::make_shared<std::string>("2")), Exception); // dup key
    checkVector(v, {"0", "1", "2"});
}

BOOST_AUTO_TEST_CASE(test_insert)
{
    StringPtrVector v = createVector({"1"});
    v.insert(std::make_shared<std::string>("0"), 0);
    v.insert(std::make_shared<std::string>("4"), 2);
    v.insert(std::make_shared<std::string>("3"), 2);
    checkVector(v, {"0", "1", "3", "4"});

    StringPtrVector v1 = createVector({"1", "4", "5"});
    std::map<StringPtrVector::Index, StringPtr> values = {
        {0, std::make_shared<std::string>("0")},
        {2, std::make_shared<std::string>("2")},
        {3, std::make_shared<std::string>("3")},
        {6, std::make_shared<std::string>("6")}
    };
    v1.insert(values);
    checkVector(v1, {"0", "1", "2", "3", "4", "5", "6"});
}

BOOST_AUTO_TEST_CASE(test_insert_error)
{
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.insert(std::make_shared<std::string>("2"), 1), Exception); // invalid index
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"1", "-2"});
        BOOST_CHECK_THROW(v.insert(std::make_shared<std::string>("-2"), 0), Exception); // duplicate key
        checkVector(v, {"1", "-2"});
    }
    {
        StringPtrVector v = createVector({"1", "2"});
        BOOST_CHECK_THROW(v.insert(std::make_shared<std::string>("-2"), 3), Exception); // invalid index
        checkVector(v, {"1", "2"});
    }
    {
        StringPtrVector v;
        std::map<StringPtrVector::Index, StringPtr> m = {
            {0, std::make_shared<std::string>("0")},
            {1, std::make_shared<std::string>("0")}
        };
        BOOST_CHECK_THROW(v.insert(m), Exception); // dup key
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"1", "4", "5"});
        std::map<StringPtrVector::Index, StringPtr> m = {
            {0, std::make_shared<std::string>("0")},
            {2, std::make_shared<std::string>("1")}
        };
        BOOST_CHECK_THROW(v.insert(m), Exception); // dup key
        checkVector(v, {"1", "4", "5"});
    }
    {
        StringPtrVector v;
        std::map<StringPtrVector::Index, StringPtr> m = {
            {0, std::make_shared<std::string>("2")},
            {2, std::make_shared<std::string>("0")}
        };
        BOOST_CHECK_THROW(v.insert(m), Exception); // invalid index
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"1", "4", "5"});
        std::map<StringPtrVector::Index, StringPtr> m = {
            {0, std::make_shared<std::string>("0")},
            {8, std::make_shared<std::string>("2")}
        };
        BOOST_CHECK_THROW(v.insert(m), Exception); // invalid index
        checkVector(v, {"1", "4", "5"});
    }
}

void checkRemoveResult(
    const std::map<size_t, std::string>& exp,
    const std::map<StringPtrVector::Index, StringPtr>& recv)
{
    BOOST_REQUIRE(recv.size() == exp.size());
    auto itr = recv.begin();
    auto ite = exp.begin();
    for ( ; itr != recv.end(); ++itr, ++ite) {
        BOOST_CHECK(itr->first == ite->first);
        BOOST_CHECK(*itr->second == ite->second);
    }
}

BOOST_AUTO_TEST_CASE(test_remove)
{
    StringPtrVector v = createVector({"1", "2", "3", "4"});
    BOOST_CHECK(*v.remove(0) == "1");
    checkVector(v, {"2", "3", "4"});
    BOOST_CHECK(*v.remove(1) == "3");
    checkVector(v, {"2", "4"});
    BOOST_CHECK(*v.remove(1) == "4");
    checkVector(v, {"2"});
    BOOST_CHECK(*v.remove(0) == "2");
    checkVector(v, {});

    StringPtrVector v1 = createVector({"0", "1", "2", "3", "4", "5", "6"});
    std::set<StringPtrVector::Index> at = {0, 2, 3, 6};
    auto recv = v1.remove(at);
    checkVector(v1, {"1", "4", "5"});
    checkRemoveResult({{0, "0"}, {2, "2"}, {3, "3"}, {6, "6"}}, recv);
    std::set<StringPtrVector::Index> at2 = {0, 1, 2};
    auto recv2 = v1.remove(at2);
    checkVector(v1, {});
    checkRemoveResult({{0, "1"}, {1, "4"}, {2, "5"}}, recv2);
}

BOOST_AUTO_TEST_CASE(test_remove_error)
{
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.remove(0), Exception);
    }
    {
        StringPtrVector v = createVector({"1", "2", "3", "4"});
        BOOST_CHECK_THROW(v.remove(4), Exception);
        checkVector(v, {"1", "2", "3", "4"});
    }
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.remove({0, 1, 2}), Exception);
    }
    {
        StringPtrVector v = createVector({"1", "2", "3", "4"});
        BOOST_CHECK_THROW(v.remove({0, 1, 8}), Exception);
        checkVector(v, {"1", "2", "3", "4"});
    }
}

BOOST_AUTO_TEST_CASE(test_move)
{
    StringPtrVector v = createVector({"0", "1", "2", "3", "4", "5", "6"});

    v.move(0, 2);
    checkVector(v, {"1", "2", "0", "3", "4", "5", "6"});
    v.move(2, 0);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(3, 6);
    checkVector(v, {"0", "1", "2", "4", "5", "6", "3"});
    v.move(6, 3);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(0, 6);
    checkVector(v, {"1", "2", "3", "4", "5", "6", "0"});
    v.move(6, 0);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});

    v.move(3, 4);
    checkVector(v, {"0", "1", "2", "4", "3", "5", "6"});
    v.move(4, 3);
    checkVector(v, {"0", "1", "2", "3", "4", "5", "6"});
}

BOOST_AUTO_TEST_CASE(test_move_error)
{
    {
        StringPtrVector v;
        BOOST_CHECK_THROW(v.move(0, 1), Exception);
        checkVector(v, {});
    }
    {
        StringPtrVector v = createVector({"0", "1", "2"});
        BOOST_CHECK_THROW(v.move(0, 3), Exception);
        checkVector(v, {"0", "1", "2"});
        BOOST_CHECK_THROW(v.move(3, 0), Exception);
        checkVector(v, {"0", "1", "2"});
    }
}

BOOST_AUTO_TEST_SUITE_END()
