SUBDIRS = gamegui minizip

noinst_HEADERS = engine.h gamegui.h main_sdlthd.h pipe_mem.h pipe_unix.h \
	pipe_win32.h san.h git_rev.h gamegui_dialogs.h theme.h options.h \
	system_config.h audio.h network.h news.h browser.h

.PHONY: git_rev.h

git_rev.h:
	@if [ -d "$(top_srcdir)/.git" ] && hash git 2>&-; \
	then \
		echo '#define GIT_REV "'`cd "$(top_srcdir)" ; git rev-parse --verify HEAD 2>/dev/null | cut -c 1-7`'"' > git_rev.h; \
	fi
