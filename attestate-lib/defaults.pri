QT += gui widgets printsupport xml xmlpatterns

CONFIG += c++11

INCLUDEPATH += \
    ../src/include \
    ../../doctpl-lib/src/include

QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    ../unique_vector.h

LIBS += \
    -L/home/dicentra/projects/qt/attestate/doctpl-lib/build-debug/src -ldoctpl
