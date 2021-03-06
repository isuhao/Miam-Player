QT += sql multimedia widgets

TEMPLATE = lib

DEFINES += MIAMLIBRARY_LIBRARY

SOURCES += albumitem.cpp \
    artistitem.cpp \
    discitem.cpp \
    libraryfilterlineedit.cpp \
    libraryfilterproxymodel.cpp \
    libraryheader.cpp \
    libraryitemdelegate.cpp \
    libraryitemmodel.cpp \
    libraryorderdialog.cpp \
    libraryscrollbar.cpp \
    librarytreeview.cpp \
    miamitemdelegate.cpp \
    miamitemmodel.cpp \
    separatoritem.cpp \
    trackitem.cpp \
    yearitem.cpp

HEADERS += albumitem.h \
    artistitem.h \
    discitem.h \
    libraryfilterlineedit.h \
    libraryfilterproxymodel.h \
    libraryheader.h \
    libraryitemdelegate.h \
    libraryitemmodel.h \
    libraryorderdialog.h \
    libraryscrollbar.h \
    librarytreeview.h \
    miamitemdelegate.h \
    miamitemmodel.h \
    miamlibrary_global.hpp \
    separatoritem.h \
    trackitem.h \
    yearitem.h

FORMS += \
    libraryorderdialog.ui

CONFIG(debug, debug|release) {
    win32: LIBS += -L$$OUT_PWD/../Core/debug/ -lCore
    OBJECTS_DIR = debug/.obj
    MOC_DIR = debug/.moc
    RCC_DIR = debug/.rcc
    UI_DIR = $$PWD
}

CONFIG += c++11
CONFIG(release, debug|release) {
    win32: LIBS += -L$$OUT_PWD/../Core/release/ -lCore
    OBJECTS_DIR = release/.obj
    MOC_DIR = release/.moc
    RCC_DIR = release/.rcc
    UI_DIR = $$PWD
}
win32 {
    TARGET = Library
}
unix {
    LIBS += -L$$OUT_PWD/../Core/ -lmiam-core
    TARGET = miam-library
}
unix:!macx {
    target.path = /usr/lib$$LIB_SUFFIX/
    INSTALLS += target
}
macx {
    QMAKE_SONAME_PREFIX = @executable_path/../Frameworks
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
}

INCLUDEPATH += $$PWD/../Core/
DEPENDPATH += $$PWD/../Core

TRANSLATIONS = translations/library_ar.ts \
    translations/library_cs.ts \
    translations/library_de.ts \
    translations/library_el.ts \
    translations/library_en.ts \
    translations/library_es.ts \
    translations/library_fr.ts \
    translations/library_in.ts \
    translations/library_it.ts \
    translations/library_ja.ts \
    translations/library_kr.ts \
    translations/library_pt.ts \
    translations/library_ru.ts \
    translations/library_th.ts \
    translations/library_vn.ts \
    translations/library_zh.ts

RESOURCES += library.qrc
