
QT       += core gui\
            opengl\
	    multimedia

TEMPLATE = app
TARGET = ASPLOSHOOTER
INCLUDEPATH += .

QMAKE_CFLAGS+= -std=c++11 -O0 -Wunused-variable -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-sign-compare

QMAKE_CXXFLAGS+=-std=c++11 -O0 -Wno-unused-variable -Wno-unused-parameter -Wno-maybe-uninitialized -Wno-sign-compare

HEADERS += asploshooter.h\

SOURCES +=asploshooter.cpp main.cpp 

LIBS += -lGLEW 
