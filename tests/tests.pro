QT += core widgets gui network testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../src
SOURCES += tst_utils.cpp\
    ../src/helpers.cpp\
    ../src/chain_elements/chain_element.cpp\
    ../src/chain_elements/source_filter.cpp

HEADERS += \
    test_sink.h
