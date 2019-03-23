#-------------------------------------------------
#
# Project created by QtCreator 2019-03-16T20:29:49
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = antilogviewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    antilogviewer.cpp \
    chain_elements/udp_socket.cpp \
    chain_elements/level_filter.cpp \
    chain_elements/memory_storage.cpp \
    chain_elements/terminator.cpp \
    table_model.cpp

HEADERS += \
    antilogviewer.h \
    chain_elements/chain_element.h \
    chain_elements/udp_socket.h \
    chain_elements/level_filter.h \
    chain_elements/memory_storage.h \
    log_item.h \
    chain_elements/terminator.h \
    table_model.h

RESOURCES +=

FORMS +=
