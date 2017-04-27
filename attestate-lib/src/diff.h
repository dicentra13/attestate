#pragma once

#include "helpers.h"

#include <attestate/exception.h>

#include <boost/optional.hpp>

#include <map>

namespace attestate {

template <class K, class V, class Cmp = std::less<K>>
class Diff {
public:

    typedef boost::optional<V> OptV;
    typedef std::map<K, V, Cmp> ValuesType;
    typedef std::map<K, std::pair<OptV, OptV>, Cmp> DiffType;

    static DiffType compute(const ValuesType& v1, const ValuesType& v2)
    {
        DiffType res;
        for (const auto& g : v1) {
            res.insert({g.first, {g.second, boost::none}});
        }
        for (const auto& g : v2) {
            auto r = res.insert({g.first, {boost::none, g.second}});
            if (r.second) {
                continue;
            }
            if (r.first->second.first == g.second) {
                res.erase(r.first);
            } else {
                r.first->second.second = g.second;
            }
        }
        return res;
    }

    static void apply(ValuesType& v, const DiffType& diff)
    {
        // check
        for (const auto& d : diff) {
            const auto& p = d.second;
            auto it = v.find(d.first);
            if (p.first) {
                ATT_REQUIRE(it != v.end(), "Key " << d.first << " not found");
                ATT_REQUIRE(it->second == *p.first,
                    "Diff and map values for key " << d.first << " mismatch, "
                    << " expected " << it->second << ", got " << *p.first);
                if (p.second) {
                    ATT_REQUIRE(*p.second != *p.first,
                        "Equal values " << *p.first << " in diff for key " << d.first);
                }
            } else {
                ATT_REQUIRE(it == v.end(), "Key " << d.first << " is not expected");
                ATT_REQUIRE(p.second, "Both diff values are none for key " << d.first);
            }
        }
        // apply
        for (const auto& d : diff) {
            const auto& p = d.second;
            auto it = v.find(d.first);
            if (p.first) {
                if (p.second) {
                    it->second = *p.second;
                } else {
                    v.erase(it);
                }
            } else {
                v.insert({d.first, *p.second});
            }
        }
    }

    static DiffType reverse(const DiffType& diff)
    {
        DiffType res;
            for (const auto& p : diff) {
                res.insert({p.first, {p.second.second, p.second.first}});
            }
            return res;
    }
};

} // namespace attestate
