#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <attestate/validate.h>

#include "../src/helpers.h"

#include <boost/optional.hpp>

#include <initializer_list>
#include <vector>
#include <tuple>
#include <functional>

using namespace attestate;
using namespace attestate::validation;

namespace std {

std::ostream& operator << (std::ostream& o, ValueError er)
{
    std::map<ValueError, std::string> v = {
        {ValueError::Empty, "Empty"},
        {ValueError::Invalid, "Invalid"}
    };
    o << v.at(er);
    return o;
}

} // namespace std

BOOST_AUTO_TEST_SUITE(validation_tests)

typedef boost::optional<ValueError> OptionalError;

template <class PropertyType>
struct PropertyData {
    struct D {
        PropertyType value;
        OptionalError error;
    };

    std::vector<D> data;
    QString name;
    std::function<void(Student&, const PropertyType&)> valueSetter;
};

constexpr const size_t PROPERTIES_COUNT = 7;

auto PROPERTIES_DATA = std::make_tuple(
    PropertyData<QString>{
        {
            {"TEST_FNAME", boost::none},
            {"", ValueError::Empty}
        },
        cfg::tags::property::FAMILY_NAME,
        [] (Student& s, const QString& v) { s.setFamilyName(v); }
    },
    PropertyData<QString>{
        {
            {"TEST_NAME", boost::none},
            {"", ValueError::Empty}
        },
        cfg::tags::property::NAME,
        [] (Student& s, const QString& v) { s.setName(v); }
    },
    PropertyData<QString>{
        {
            {"TEST_PNAME", boost::none},
            {"", ValueError::Empty}
        },
        cfg::tags::property::PARENTAL_NAME,
        [] (Student& s, const QString& v) { s.setParentalName(v); }
    },
    PropertyData<QString>{
        {
            {"TEST_ATTESTATE_ID", boost::none},
            {"", ValueError::Empty}
        },
        cfg::tags::property::ATTESTATE_ID,
        [] (Student& s, const QString& v) { s.setAttestateId(v); }
    },
    PropertyData<QDate>{
        {
            {QDate::currentDate(), boost::none},
            {QDate(), ValueError::Invalid}
        },
        cfg::tags::property::BIRTH_DATE,
        [] (Student& s, const QDate& v) { s.setBirthDate(v); }
    },
    PropertyData<OptionalDate>{
        {
            {QDate::currentDate(), boost::none},
            {QDate(), ValueError::Invalid},
            {boost::none, ValueError::Invalid}
        },
        cfg::tags::property::ISSUE_DATE,
        [] (Student& s, const OptionalDate& v) { s.setIssueDate(v); }
    },
    PropertyData<OptionalYear>{
        {
            {QDate::currentDate().year(), boost::none},
            {boost::none, ValueError::Empty}
        },
        cfg::tags::property::GRADUATION_YEAR,
        [] (Student& s, const OptionalYear& v) { s.setGraduationYear(v); }
    }
);

std::vector<size_t> CURRENT(PROPERTIES_COUNT, 0);

#define INCREMENT_TUPLE(index) \
    static_assert(index < PROPERTIES_COUNT, "Index too big"); \
    if (CURRENT[index] == std::get<index>(PROPERTIES_DATA).data.size() - 1) { \
        CURRENT[index] = 0; \
    } else { \
        ++CURRENT[index]; \
        return true; \
    } \
// INCREMENT_TUPLE

bool getNext()
{
    INCREMENT_TUPLE(0)
    INCREMENT_TUPLE(1)
    INCREMENT_TUPLE(2)
    INCREMENT_TUPLE(3)
    INCREMENT_TUPLE(4)
    INCREMENT_TUPLE(5)
    INCREMENT_TUPLE(6)
    return false;
}

#undef INCREMENT_TUPLE

std::string currentState()
{
    std::ostringstream os;
    for (size_t i = 0; i < PROPERTIES_COUNT; ++i) {
        os << (i == 0 ? "" : ":") << CURRENT[i];
    }
    return os.str();
}

BOOST_AUTO_TEST_CASE(test_student_personal)
{
    StudentErrors errors;
    std::vector<Class::StudentPtr> ss;
    auto sp = Class::StudentPtr(new Student(ID::gen()));
    sp->save();
    ss.push_back(std::move(sp));
    Class c(ID::gen(), "", boost::none, boost::none, std::move(ss), nullptr);

    auto& s = c.student(0);

#define UPDATE_VALUE(index) \
    const auto& v##index = std::get<index>(PROPERTIES_DATA); \
    v##index.valueSetter(s, v##index.data[CURRENT[index]].value); \
    const auto& e##index = v##index.data[CURRENT[index]].error; \
    if (!e##index) { \
        errors.propertyErrors.erase(v##index.name); \
    } else { \
        errors.propertyErrors[v##index.name] = *e##index; \
    } \
// UPDATE_VALUE

    do {
        UPDATE_VALUE(0)
        UPDATE_VALUE(1)
        UPDATE_VALUE(2)
        UPDATE_VALUE(3)
        UPDATE_VALUE(4)
        UPDATE_VALUE(5)
        UPDATE_VALUE(6)

        auto res = validate(c, 0);
        if (errors.propertyErrors.empty()) {
            BOOST_CHECK(!res);
        } else {
            BOOST_REQUIRE_MESSAGE(
                res, "Empty validation res, properties state: " << currentState());

            BOOST_CHECK(res->gradeErrors.empty());
            BOOST_REQUIRE(res->propertyErrors.size() == errors.propertyErrors.size());

            auto ite = errors.propertyErrors.begin();
            auto itr = res->propertyErrors.begin();
            for ( ; ite != errors.propertyErrors.end(); ++ite, ++itr) {
                BOOST_CHECK_MESSAGE(
                    itr->first == ite->first,
                    "Expected property: " << ite->first.toStdString()
                        << ", received: " << itr->first.toStdString());
                BOOST_CHECK_MESSAGE(
                    itr->second == ite->second,
                    "Property: " << ite->first.toStdString()
                        << ", expected error: " << ite->second
                        << ", received: " << itr->second);
            }
        }
    } while (getNext());

#undef UPDATE_VALUE
}

BOOST_AUTO_TEST_SUITE_END()
