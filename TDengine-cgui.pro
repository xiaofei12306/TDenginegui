QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    addfilterwidget.cpp \
    customheaderview.cpp \
    customitemdelegate.cpp \
    customtabbar.cpp \
    customtabwidget.cpp \
    filterwidget.cpp \
    leftmargindelegate.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    querysqlwidget.cpp \
    sqlclient.cpp \
    sqlhighlighter.cpp \
    tabletabwidget.cpp \
    taosexecuteresult.cpp \
    taosfield.cpp \
    tdclient.cpp \
    tooltipwidget.cpp \
    treeitemmodel.cpp \
    treeviewwithmenu.cpp

HEADERS += \
    about.h \
    addfilterwidget.h \
    customheaderview.h \
    customitemdelegate.h \
    customtabbar.h \
    customtabwidget.h \
    filterwidget.h \
    leftmargindelegate.h \
    login.h \
    mainwindow.h \
    querysqlwidget.h \
    sqlclient.h \
    sqlhighlighter.h \
    tabletabwidget.h \
    taosexecuteresult.h \
    taosfield.h \
    tdclient.h \
    tooltipwidget.h \
    treeitemmodel.h \
    treeviewwithmenu.h

FORMS += \
    about.ui \
    login.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/driver/ -ltaos

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

RESOURCES += \
    res.qrc

RC_ICONS = icon\app.ico
