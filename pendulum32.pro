QT += charts widgets

CONFIG += c++11

HEADERS += \
    dialog.h \
    mainwindow.h \
    rkn.h \
    widget.h

SOURCES += \
    dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    rkn.cpp \
    widget.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    dialog.ui
