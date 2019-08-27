if(WIN32)
	set(LOCALEDIR locale)
elseif(APPLE)
	set(LOCALEDIR DreamChess.app/Contents/SharedSupport/locale)
else()
	set(LOCALEDIR ${CMAKE_INSTALL_LOCALEDIR})
endif()

GettextTranslate()
