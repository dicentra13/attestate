#include <attestate/common.h>

namespace attestate {

namespace {

const OID EMPTY_OID = 0;
const DBID EMPTY_DBID = 0;

} // namespace

ID::ID() : id_(EMPTY_OID), dbid_(EMPTY_DBID) {}

ID::ID(DBID dbid) : id_(EMPTY_OID), dbid_(dbid) {}

ID::ID(const OID& oid, const DBID& dbid) : id_(oid), dbid_(dbid) {}

ID ID::setDBID(const ID& id, const DBID& dbid) { return ID{id.oid(), dbid}; }

const OID& ID::oid() const { return id_; }
const DBID& ID::dbid() const { return dbid_; }

bool ID::operator < (const ID& id) const { return id_ < id.id_; }
bool ID::operator == (const ID& id) const { return id_ == id.id_; }
bool ID::operator != (const ID& id) const { return !(*this == id); }

const ID& ID::emptyID()
{
    static const ID s_emptyID = ID();
    return s_emptyID;
}

ID ID::gen()
{
    static OID s_gen = 0;

    return ID(++s_gen, EMPTY_DBID);
}

std::ostream& operator << (std::ostream& os, const ID& id)
{
    os << id.oid() << ":" << id.dbid();
    return os;
}

} // namespace attestate
