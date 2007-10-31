SUBDIRS = gamegui minizip

noinst_HEADERS = engine.h gamegui.h main_sdlthd.h pipe_mem.h pipe_unix.h \
	pipe_win32.h san.h svn_version.h gamegui_dialogs.h theme.h options.h \
	system_config.h audio.h

.PHONY: svn_version.h

svn_version.h:
	if [ -d .svn ]; then \
		( \
		echo -n '#define SVN_VERSION "'; \
		svnversion -n ../..; \
		echo '"'; \
		) > svn_version.h; \
	fi
