#pragma once

#include <QDate>
#include <QString>

#include <boost/optional.hpp>

#include <string>
#include <map>
#include <set>
#include <memory>
#include <cstdint>

namespace attestate {

typedef uint16_t Year;
typedef boost::optional<Year> OptionalYear;

typedef boost::optional<QDate> OptionalDate;

typedef uint32_t OID;
typedef uint32_t DBID;

typedef QString DataString;

// TODO maybe be able to construct complex ids like dd AA ddddd
typedef DataString AttestateId;

class ID {
public:
    explicit ID(DBID dbid);

    static ID setDBID(const ID& id, const DBID& dbid);

    const OID& oid() const;
    const DBID& dbid() const;

    bool operator < (const ID& id) const;
    bool operator == (const ID& id) const;
    bool operator != (const ID& id) const;

    static const ID& emptyID();

    static ID gen();

private:
    ID(); // empty
    ID(const OID& oid, const DBID& dbid);

    OID id_;
    DBID dbid_;
};

typedef std::set<ID> IDSet;

std::ostream& operator << (std::ostream& os, const ID& id);


enum class State {New, Existing, Modified, Deleted};

} // namespace attestate

