#-------------------------------------------------
#
# Project created by QtCreator 2016-05-29T18:24:36
#
#-------------------------------------------------

QT       += gui widgets core printsupport xml xmlpatterns

QMAKE_CXXFLAGS += -std=c++11

TARGET = attestate-app
TEMPLATE = app

INCLUDEPATH += \
    ../attestate-lib/src/include \
    ../doctpl-lib/src/include

SOURCES += \
    main.cpp \
    class/class_model.cpp \
    class/class_header_data.cpp \
    class/header_view.cpp \
    class/horizontal_header.cpp \
    class/vertical_header.cpp \
    class/class_editor.cpp \
    class/class_common_widget.cpp \
    class/colored_cell_delegate.cpp \
    main_window.cpp

HEADERS  += \
    main_window.h \
    class/class_editor.h \
    class/class_model.h \
    class/class_header_data.h \
    class/class_view.h \
    class/header_view.h \
    class/horizontal_header.h \
    class/vertical_header.h \
    class/class_common_widget.h \
    class/colored_cell_delegate.h \
    class/class_widget.h

LIBS += \
    -L/home/dicentra/projects/qt/attestate/attestate-lib/build-debug/src -lattestate \
    -L/home/dicentra/projects/qt/attestate/doctpl-lib/build-debug/src -ldoctpl

OTHER_FILES += \
    todo.txt
