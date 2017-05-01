set (FMOD_DIR "" CACHE PATH "Location of the FMOD Programmer's API SDK")

set (_oldPath ${FMOD_FMOD_H})

find_file (FMOD_FMOD_H api/lowlevel/inc/fmod.h
    PATHS "${FMOD_DIR}"
    HINTS ENV DENG_DEPEND_PATH
    PATH_SUFFIXES "FMOD" "FMOD Programmers API"
    NO_DEFAULT_PATH
)
mark_as_advanced (FMOD_FMOD_H)

if (NOT _oldPath STREQUAL FMOD_FMOD_H)
    if (FMOD_FMOD_H)
        message (STATUS "Looking for FMOD Low Level Programmer API - found")
    else ()
        message (STATUS "Looking for FMOD Low Level Programmer API - not found (set the FMOD_DIR variable)")
    endif ()
endif ()

if (FMOD_FMOD_H AND NOT TARGET fmodex)
    get_filename_component (fmodInc "${FMOD_FMOD_H}" DIRECTORY)
    get_filename_component (fmodApi "${fmodInc}" DIRECTORY)

    add_library (fmodex INTERFACE)
    target_include_directories (fmodex INTERFACE ${fmodInc})
    if (APPLE)
        set (fmodLib "${fmodApi}/lib/libfmod.dylib")
        set (fmodInstLib ${fmodLib})
    elseif (MSVC)
        if (ARCH_BITS EQUAL 64)
            set (fmodLib "${fmodApi}/lib/fmod64_vc.lib")
            set (fmodInstLib "${fmodApi}/fmod64.dll")
        else ()
            set (fmodLib "${fmodApi}/lib/fmod_vc.lib")
            set (fmodInstLib "${fmodApi}/fmod.dll")
        endif ()
    elseif (UNIX)
        if (ARCH_BITS EQUAL 64)
            set (fmodLib ${fmodApi}/lib/libfmod64.so)
        else ()
            set (fmodLib ${fmodApi}/lib/libfmod.so)
        endif ()
        set (fmodInstLib ${fmodLib})
    endif ()
    target_link_libraries (fmodex INTERFACE ${fmodLib})
    deng_install_library (${fmodInstLib})
endif ()
