install(CODE "include(BundleUtilities)")

if(MSVC)
	set(CMAKE_INSTALL_UCRT_LIBRARIES 1)
	set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP 1)
	include(InstallRequiredSystemLibraries)
	set(SYSTEM_LIBS_RELEASE ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})

	# Include the api-ms-win-* DLLs in the Debug build
	foreach(LIB ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
		if(NOT ${LIB} MATCHES "api-ms-win-")
			list(REMOVE_ITEM CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${LIB})
		endif()
	endforeach()

	set(CMAKE_INSTALL_DEBUG_LIBRARIES 1)
	set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY 1)
	include(InstallRequiredSystemLibraries)
	set(SYSTEM_LIBS_DEBUG ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})

	install(CODE "
		string(TOLOWER \"\${CMAKE_INSTALL_CONFIG_NAME}\" CONFIG_NAME_LOWER)
		if(\"\${CONFIG_NAME_LOWER}\" STREQUAL \"debug\")
			set(SYSTEM_LIBS \"${SYSTEM_LIBS_DEBUG}\")
		else()
			set(SYSTEM_LIBS \"${SYSTEM_LIBS_RELEASE}\")
		endif()
		foreach(LIB \${SYSTEM_LIBS})
			file(INSTALL \${LIB} DESTINATION \${CMAKE_INSTALL_PREFIX})
		endforeach()
	")
endif()

# Skip fixup_bundle for vcpkg as the DLLs are already in place
if(WIN32 AND NOT VCPKG_TOOLCHAIN)
    if(BUILD_DREAMCHESS)
        install(CODE "fixup_bundle(\${CMAKE_INSTALL_PREFIX}/dreamchess.exe \"\" \"\")")
    elseif(BUILD_DREAMER)
        install(CODE "fixup_bundle(\${CMAKE_INSTALL_PREFIX}/dreamer.exe \"\" \"\")")
    endif()
endif()
