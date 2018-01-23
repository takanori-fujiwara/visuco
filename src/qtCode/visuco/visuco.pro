#-------------------------------------------------
#
# Project created by QtCreator 2016-06-30T09:12:34
#
#-------------------------------------------------

QT       += core gui opengl concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visuco
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Vertex.cpp \
    Edge.cpp \
    Route.cpp \
    Graph.cpp \
    GraphView.cpp \
    Color.cpp \
    MatrixView.cpp \
    AdjMatrix.cpp \
    RouteGuide.cpp \
    StatView.cpp \
    FormatConverter.cpp \
    MappingGuide.cpp \
    DataDirectorySelectionDialog.cpp \
    SettingsDialog.cpp \
    input.cpp \
    memetic.cpp \
    localSearch.cpp \
    MappingGuideSa.cpp

HEADERS  += MainWindow.h \
    Vertex.h \
    Edge.h \
    Route.h \
    Graph.h \
    GraphView.h \
    Color.h \
    MatrixView.h \
    AdjMatrix.h \
    RouteGuide.h \
    StatView.h \
    FormatConverter.h \
    MappingGuide.h \
    combination.h \
    DataDirectorySelectionDialog.h \
    SettingsDialog.h \
    input.h \
    memetic.h \
    common.h \
    localSearch.h \
    MappingGuideSa.h

FORMS    += MainWindow.ui

CONFIG += c++11
#INCLUDEPATH += /usr/local/include/viennacl/
INCLUDEPATH += /usr/local/include/eigen3/
QMAKE_CXXFLAGS_RELEASE += -O3 # to optimize the compiled result
