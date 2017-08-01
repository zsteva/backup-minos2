HAMLIBDIR = $$absolute_path(../../hamlib-w32-3.1)

win32: INCLUDEPATH += $$HAMLIBDIR/include

win32-g++: LIBS += -L$$HAMLIBDIR/lib/gcc/ -llibhamlib
msvc: LIBS += -L$$HAMLIBDIR/lib/msvc/ -llibhamlib-2
unix: LIBS += -lhamlib
