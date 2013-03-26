noinst_HEADERS = ui_sdlgl_3d.h ui_sdlgl.h
if UI_SDLGL
noinst_LIBRARIES = libui_sdlgl.a
endif
AM_CFLAGS = $(CFLAGS) @DATADIR@ @SDL_CFLAGS@
AM_CPPFLAGS = -I$(top_builddir)/src/include -I$(top_srcdir)/src/include -I$(top_srcdir)/src
libui_sdlgl_a_SOURCES = ui_sdlgl.c ui_sdlgl_3d.c texture.c draw_shape.c text.c colours.c \
system.c ingame_ui.c draw_scene.c get_move.c theme.c transitions.c credits.c \
load_data.c xmlsavefile.c gamegui_driver.c saveload_dialog.c title_dialog.c \
victory_dialog.c promote_dialog.c vkeyboard.c