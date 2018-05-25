install(CODE "include(BundleUtilities)")

if(MINGW)
    if(BUILD_DREAMCHESS)
        install(CODE "fixup_bundle(\${CMAKE_INSTALL_PREFIX}/dreamchess.exe \"\" \"\")")
    elseif(BUILD_DREAMER)
        install(CODE "fixup_bundle(\${CMAKE_INSTALL_PREFIX}/dreamer.exe \"\" \"\")")
    endif()
endif()
