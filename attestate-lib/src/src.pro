include(../defaults.pri)

#QT       += sql

TARGET = attestate
TEMPLATE = lib

DEFINES += ATTESTATELIB_LIBRARY

SOURCES += \
    class.cpp \
    subjects.cpp \
    common.cpp \
    student.cpp \
    grades.cpp \
    serialize.cpp \
    generate.cpp \
    validate.cpp

HEADERS += \
    include/attestate/class.h \
    include/attestate/common.h \
    include/attestate/grades.h \
    include/attestate/student.h \
    include/attestate/subjects.h \
    include/attestate/exception.h \
    include/attestate/serialize.h \
    include/attestate/generate.h \
    include/attestate/validate.h \
    diff.h \
    magic_strings.h \
    helpers.h \
    unique_vector.h

OTHER_FILES += \
    todo.txt

