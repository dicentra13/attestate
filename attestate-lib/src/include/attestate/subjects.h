#pragma once

#include <attestate/common.h>
#include <attestate/exception.h>

#include <vector>
#include <set>
#include <map>

namespace attestate {

class Subject {
public:
    // new
    explicit Subject(const ID& id);

    // existing
    Subject(
        const ID& id,
        const DataString& name,
        const DataString& shortenedName = "");

    Subject(Subject&&);
    Subject& operator = (Subject&&);

    ~Subject();

    const ID& id() const;
    void setDBID(const DBID& dbid); // if saved to DB

    State state() const;
    void setDeleted(bool isDeleted);

    // data

    const DataString& name() const;
    void setName(const DataString& name);
    bool isNameModified() const;

    const DataString& shortenedName() const;
    void setShortenedName(const DataString& name);
    bool isShortenedNameModified() const;

    // set current state as original and discard cached changes
    void save();

private:
    class Impl;

    std::unique_ptr<Impl> impl_;
};
// class Subject

typedef std::shared_ptr<Subject> SubjectPtr;
typedef std::vector<SubjectPtr> SubjectPtrVector;


// subjects plan

class SubjectsPlan {
public:
    typedef std::vector<ID> SubjectIdVector;

    // new
    explicit SubjectsPlan(const ID& id);

    // existing
    SubjectsPlan(
        const ID& id,
        const DataString& name,
        SubjectPtrVector subjects);

    SubjectsPlan(SubjectsPlan&&);
    SubjectsPlan& operator = (SubjectsPlan&&);

    ~SubjectsPlan();

    const ID& id() const;
    void setDBID(const DBID& dbid); // if saved to DB

    State state() const;
    void setDeleted(bool isDeleted);

    // data

    const DataString& name() const;
    void setName(const DataString& name);
    bool isNameModified() const;

    typedef uint8_t Index;

    void insert(const SubjectPtr& subject, Index at);
    void append(const SubjectPtr& subject);

    // id of removed subject
    SubjectPtr erase(Index at);
    // {index before erase -> subject id}
    std::map<Index, SubjectPtr> erase(const std::set<Index>& at);

    void move(Index from, Index to);

    bool areSubjectsModified() const;
    bool isModified() const;

    // diff

    struct SubjectPtrCompare {
        bool operator () (const SubjectPtr& p1, const SubjectPtr& p2) const;
    };

    typedef boost::optional<Index> OptionalIndex;
    typedef std::map<
        SubjectPtr,
        std::pair<OptionalIndex, OptionalIndex>,
        SubjectPtrCompare
    > Diff;

    Diff diff(const SubjectsPlan& o) const;
    void applyDiff(const Diff& diff); // check that it is valid diff of current plan

    static SubjectsPlan::Diff reverseDiff(const SubjectsPlan::Diff& diff);

    // RO access

    const Subject& at(Index at) const;
    bool hasSubject(const ID& id) const;

    size_t subjectsCount() const;

    SubjectIdVector subjectIds() const;

    // by id
    bool operator == (const SubjectsPlan& other) const;

    // set current state as original and discard cached changes
    void save();

private:
    class Impl;

    std::unique_ptr<Impl> impl_;
}; // class SubjectsPlan

typedef std::shared_ptr<SubjectsPlan> SubjectsPlanPtr;

} // namespace attestate

