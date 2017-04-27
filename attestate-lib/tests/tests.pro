include(../defaults.pri)

TEMPLATE = app

SOURCES += \
    tests.cpp \
    subjects_tests.cpp \
    student_tests.cpp \
    helpers.cpp \
    grades_tests.cpp \
    unique_vector_tests.cpp \
    diff_tests.cpp \
    class_tests.cpp \
    serialize_tests.cpp \
    generate_tests.cpp \
    validation_tests.cpp

LIBS += \
    -L../src -lattestate -lboost_unit_test_framework

HEADERS += \
    helpers.h
