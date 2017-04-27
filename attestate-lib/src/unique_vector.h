#pragma once

#include <attestate/exception.h>

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <type_traits>

namespace attestate {

template <class V, class K, bool> class UniqueVectorImpl {};

template <class V, class K>
class UniqueVectorImpl<V, K, false>
{
public:
    typedef uint8_t Index;

    UniqueVectorImpl() : values_(new ValuesMap) {}

    template <class Container>
    UniqueVectorImpl(
            const Container& c,
            const typename std::enable_if<
                !std::is_same<Container, UniqueVectorImpl<V, K, false>>::value,
                Container
            >::type* = nullptr)
        : values_(new ValuesMap)
    {
        vector_.reserve(c.size());
        for (const auto& v : c) {
            append(v);
        }
    }

    template <class Container>
    UniqueVectorImpl(
            Container&& c,
            const typename std::enable_if<
                !std::is_same<Container, UniqueVectorImpl<V, K, false>>::value,
                Container
            >::type* = nullptr)
        : values_(new ValuesMap)
    {
        vector_.reserve(c.size());
        for (auto&& v : c) {
            append(std::move(v));
        }
    }

    UniqueVectorImpl(const UniqueVectorImpl<V, K, false>& o)
        : values_(new ValuesMap)
    { *this = o; }

    UniqueVectorImpl<V, K, false>& operator = (const UniqueVectorImpl<V, K, false>& o)
    {
        values_->clear();
        vector_.clear();
        vector_.reserve(o.values_->size());
        for (auto it : o.vector_) {
            append(it->second);
        }
        return *this;
    }

    UniqueVectorImpl(UniqueVectorImpl<V, K, false>&& o)
        : values_(new ValuesMap)
    { *this = std::move(o); }

    UniqueVectorImpl<V, K, false>& operator = (UniqueVectorImpl<V, K, false>&& o)
    {
        values_ = std::move(o.values_);
        vector_ = std::move(o.vector_);
        return *this;
    }

    const V& at(Index at) const
    {
        checkIndexIsValid(at);
        return vector_.at(at)->second;
    }

    bool contains(const K& key) const { return values_->find(key) != values_->end(); }

    void insert(const V& v, Index at) { insertImpl(values_->insert({K(v), v}), at); }
    void insert(V&& v, Index at) { insertImpl(values_->emplace(K(v), std::move(v)), at); }

    void insert(const std::map<Index, V>& v)
    {
        auto insWOCheck = [&] (const V& v, Index at)
        {
            auto res = values_->insert({K(v), v});
            auto it = vector_.begin() + at;
            vector_.insert(it, res.first);
        };

        checkInsertData(v);
        vector_.reserve(vector_.size() + v.size());
        for (const auto& p : v) {
            insWOCheck(p.second, p.first);
        }
    }
    void insert(std::map<Index, V>&& v)
    {
        auto insWOCheck = [&] (V&& v, Index at)
        {
            auto res = values_->insert(typename ValuesMap::value_type{K(v), std::move(v)});
            auto it = vector_.begin() + at;
            vector_.insert(it, res.first);
        };

        checkInsertData(v);
        vector_.reserve(vector_.size() + v.size());
        for (auto&& p : v) {
            insWOCheck(std::move(p.second), p.first);
        }
    }

    void append(const V& v) { insertImpl(values_->emplace(K(v), v), vector_.size()); }
    void append(V&& v) { insertImpl(values_->emplace(K(v), std::move(v)), vector_.size()); }

    V remove(Index at) { return removeImpl(at, /* check = */ true); }

    // index -> v before removal
    std::map<Index, V> remove(const std::set<Index>& at)
    {
        size_t rc = 0;
        for (auto i : at) {
            checkIndexIsValid(i - rc++);
        }

        std::map<Index, V> res;
        size_t deleted = 0;
        for (auto i : at) {
            typename std::map<Index, V>::value_type p =
                {i, std::move(removeImpl(i - deleted++, /* check = */ false))};
            res.insert(std::move(p));
        }
        return res;
    }

    void move(Index from, Index to)
    {
        checkIndexIsValid(from);
        checkIndexIsValid(to);
        auto fromIt = vector_.begin() + from;
        auto toIt = vector_.begin() + to;
        int d = to > from ? 1 : -1;
        auto fromSetIt = *fromIt;
        for (auto i = fromIt; i != toIt; i += d) {
            *i = *(i + d);
        }
        *toIt = fromSetIt;
    }

    void reserve(size_t size) { vector_.reserve(size); }

    bool empty() const { return vector_.empty(); }
    size_t size() const { return vector_.size(); }

private:
    typedef std::map<K, V> ValuesMap;

    void insertImpl(const std::pair<typename ValuesMap::iterator, bool>& res, Index at)
    {
        ATT_REQUIRE(res.second, "Key " << res.first->first << " is already present");
        ATT_REQUIRE(at <= vector_.size(), "Index " << at << " is out of range");
        auto it = vector_.begin() + at;
        vector_.insert(it, res.first);
    }

    void checkIndexIsValid(Index at) const
    {
        ATT_REQUIRE(at < vector_.size(), "Index " << at << " is out of range");
    }
    void checkInsertData(const std::map<Index, V>& v) const
    {
        size_t prevCnt = 0;
        std::set<K> ks;
        for (const auto& p : v) {
            ATT_REQUIRE(p.first <= vector_.size() + prevCnt++, "Invalid index " << p.first);
            K key(p.second);
            ATT_REQUIRE(values_->find(key) == values_->end(), "Duplicate key " << key);
            ATT_REQUIRE(ks.insert(key).second, "Duplicate key " << key);
        }
    }

    V removeImpl(Index at, bool check)
    {
        if (check) {
            checkIndexIsValid(at);
        }
        auto it = vector_.begin() + at;
        auto mapIt = *it;
        V v = std::move(mapIt->second);
        vector_.erase(it);
        values_->erase(mapIt);
        return std::move(v);
    }

    std::unique_ptr<ValuesMap> values_;
    std::vector<typename ValuesMap::iterator> vector_;
};

template <class V>
class UniqueVectorImpl<V, V, true> {
public:
    typedef uint8_t Index;

    UniqueVectorImpl() : set_(new Set) {}

    template <class Container>
    UniqueVectorImpl(
            const Container& c,
            const typename std::enable_if<
                !std::is_same<Container, UniqueVectorImpl<V, V, true>>::value,
                Container
            >::type* = nullptr)
        : set_(new Set)
    {
        vector_.reserve(c.size());
        for (const auto& v : c) {
            append(v);
        }
    }

    template <class Container>
    UniqueVectorImpl(
            Container&& c,
            const typename std::enable_if<
                !std::is_same<Container, UniqueVectorImpl<V, V, true>>::value,
                Container
            >::type* = nullptr)
        : set_(new Set)
    {
        vector_.reserve(c.size());
        for (auto&& v : c) {
            append(std::move(v));
        }
    }

    UniqueVectorImpl(const UniqueVectorImpl<V, V, true>& o)
        : set_(new Set)
    { *this = o; }

    UniqueVectorImpl<V, V, true>& operator = (const UniqueVectorImpl<V, V, true>& o)
    {
        set_->clear();
        vector_.clear();
        vector_.reserve(o.set_->size());
        for (auto it : o.vector_) {
            append(*it);
        }
        return *this;
    }

    UniqueVectorImpl(UniqueVectorImpl<V, V, true>&& o)
        : set_(new Set)
    { *this = std::move(o); }

    UniqueVectorImpl<V, V, true>& operator = (UniqueVectorImpl<V, V, true>&& o)
    {
        set_ = std::move(o.set_);
        vector_ = std::move(o.vector_);
        return *this;
    }

    const V& at(Index at) const
    {
        checkIndexIsValid(at);
        return *vector_.at(at);
    }

    bool contains(const V& key) const { return set_->find(key) != set_->end(); }

    void insert(const V& v, Index at) { insertImpl(set_->insert(v), at); }
    void insert(V&& v, Index at) { insertImpl(set_->insert(std::move(v)), at); }

    void insert(const std::map<Index, V>& v)
    {
        auto insWOCheck = [&] (const V&v, Index at)
        {
            auto res = set_->insert(v);
            auto it = vector_.begin() + at;
            vector_.insert(it, res.first);
        };

        checkInsertData(v);
        vector_.reserve(vector_.size() + v.size());
        for (const auto& p : v) {
            insWOCheck(p.second, p.first);
        }
    }
    void insert(std::map<Index, V>&& v)
    {
        auto insWOCheck = [&] (V&& v, Index at)
        {
            auto res = set_->insert(std::move(v));
            auto it = vector_.begin() + at;
            vector_.insert(it, res.first);
        };

        checkInsertData(v);
        vector_.reserve(vector_.size() + v.size());
        for (auto&& p : v) {
            insWOCheck(std::move(p.second), p.first);
        }
    }

    void append(const V& v) { insertImpl(set_->insert(v), vector_.size()); }
    void append(V&& v) { insertImpl(set_->insert(std::move(v)), vector_.size()); }

    V remove(Index at) { return removeImpl(at, /* check = */ true); }

    // index -> v before removal
    std::map<Index, V> remove(const std::set<Index>& at)
    {
        size_t rc = 0;
        for (auto i : at) {
            checkIndexIsValid(i - rc++);
        }

        std::map<Index, V> res;
        size_t deleted = 0;
        for (auto i : at) {
            res.insert({i, removeImpl(i - deleted++, /* check = */ false)});
        }
        return res;
    }

    void move(Index from, Index to)
    {
        checkIndexIsValid(from);
        checkIndexIsValid(to);
        auto fromIt = vector_.begin() + from;
        auto toIt = vector_.begin() + to;
        int d = to > from ? 1 : -1;
        auto fromSetIt = *fromIt;
        for (auto i = fromIt; i != toIt; i += d) {
            *i = *(i + d);
        }
        *toIt = fromSetIt;
    }

    void reserve(size_t size) { vector_.reserve(size); }

    bool empty() const { return vector_.empty(); }
    size_t size() const { return vector_.size(); }

private:
    typedef std::set<V> Set;

    void insertImpl(const std::pair<typename Set::iterator, bool>& res, Index at)
    {
        ATT_REQUIRE(res.second, "Key " << *(res.first) << " is already present");
        ATT_REQUIRE(at <= vector_.size(), "Index " << at << " is out of range");
        auto it = vector_.begin() + at;
        vector_.insert(it, res.first);
    }

    void checkIndexIsValid(Index at) const
    {
        ATT_REQUIRE(at < vector_.size(), "Index " << at << " is out of range");
    }

    void checkInsertData(const std::map<Index, V>& v) const
    {
        size_t prevCnt = 0;
        Set vs;
        for (const auto& p : v) {
            ATT_REQUIRE(p.first <= vector_.size() + prevCnt++, "Invalid index " << p.first);
            ATT_REQUIRE(set_->find(p.second) == set_->end(), "Duplicate key " << p.second);
            ATT_REQUIRE(vs.insert(p.second).second, "Duplicate key " << p.second);
        }
    }

    V removeImpl(Index at, bool check)
    {
        if (check) {
            checkIndexIsValid(at);
        }
        auto it = vector_.begin() + at;
        auto setIt = *it;
        V v = *setIt;
        vector_.erase(it);
        set_->erase(setIt);
        return v;
    }

    std::unique_ptr<Set> set_;
    std::vector<typename Set::iterator> vector_;
};

template <class V, class K = V>
class UniqueVector {
private:
    typedef UniqueVectorImpl<V, K, std::is_same<V, K>::value> Impl;
public:
    typedef uint8_t Index;

    UniqueVector() {}

    template <class Container>
    UniqueVector(
            const Container& c,
            const typename std::enable_if<
                !std::is_same<Container, UniqueVector<V, K>>::value,
                Container
            >::type* = nullptr)
        : impl_(c)
    {}

    template <class Container>
    UniqueVector(
            Container&& c,
            const typename std::enable_if<
                !std::is_same<Container, UniqueVector<V, K>>::value,
                Container
            >::type* = nullptr)
        : impl_(std::move(c))
    {}

    UniqueVector(const UniqueVector<V, K>& o) { *this = o; }
    UniqueVector<V, K>& operator = (const UniqueVector<V, K>& o)
    {
        impl_ = o.impl_;
        return *this;
    }

    UniqueVector(UniqueVector<V, K>&& o) { *this = std::move(o); }
    UniqueVector<V, K>& operator = (UniqueVector<V, K>&& o)
    {
        impl_ = std::move(o.impl_);
        return *this;
    }

    const V& at(Index at) const { return impl_.at(at); }
    bool contains(const K& key) const { return impl_.contains(key); }

    void insert(const V& v, Index at) { impl_.insert(v, at); }
    void insert(V&& v, Index at) { impl_.insert(std::move(v), at); }
    void insert(const std::map<Index, V>& v) { impl_.insert(v); }
    void insert(std::map<Index, V>&& v) { impl_.insert(std::move(v)); }

    void append(const V& v) { impl_.append(v); }
    void append(V&& v) { impl_.append(std::move(v)); }

    V remove(Index at) { return impl_.remove(at); }

    // index -> v before removal
    std::map<Index, V> remove(const std::set<Index>& at) { return impl_.remove(at); }

    void move(Index from, Index to) { impl_.move(from, to); }

    void reserve(size_t size) { impl_.reserve(size); }

    bool empty() const { return impl_.empty(); }
    size_t size() const { return impl_.size(); }

private:
    Impl impl_;
};

} // namespace attestate
