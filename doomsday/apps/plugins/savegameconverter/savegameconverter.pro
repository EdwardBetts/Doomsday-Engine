# The Doomsday Engine Project
# Copyright (c) 2011-2014 Jaakko Keränen <jaakko.keranen@iki.fi>
# Copyright (c) 2011-2014 Daniel Swanson <danij@dengine.net>

# This plugin uses the full libcore C++ API.
CONFIG += dengplugin_libcore_full

include(../config_plugin.pri)

TEMPLATE = lib
TARGET   = savegameconverter
VERSION  = $$SAVEGAMECONVERTER_VERSION

INCLUDEPATH += include

HEADERS += \
    include/savegameconverter.h \
    include/version.h

SOURCES += \
    src/savegameconverter.cpp

win32 {
    deng_msvc:  QMAKE_LFLAGS += /DEF:\"$$PWD/api/dpsavegameconverter.def\"
    deng_mingw: QMAKE_LFLAGS += --def \"$$PWD/api/dpsavegameconverter.def\"

    OTHER_FILES += api/dpsavegameconverter.def

    RC_FILE = res/savegameconverter.rc
}

macx {
    fixPluginInstallId($$TARGET, 1)
    linkToBundledLibcore($$TARGET)
    linkToBundledLiblegacy($$TARGET)
}