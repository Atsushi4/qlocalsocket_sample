QT += core network
QT -= gui

CONFIG += c++11

TARGET = localsocket
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

DISTFILES += \
    readme.md
