noinst_HEADERS = ui_sdlgl_3d.h ui_sdlgl.h
noinst_LIBRARIES = libui_sdlgl.a
AM_CFLAGS = $(CFLAGS) @DATADIR@ @SDL_CFLAGS@
INCLUDES = -I$(top_srcdir)/src/include -I..
libui_sdlgl_a_SOURCES = ui_sdlgl.c ui_sdlgl_3d.c texture.c draw_shape.c text.c colours.c \
system.c vkeyboard.c ingame_ui.c draw_scene.c get_move.c theme.c transitions.c credits.c \
load_data.c xmlsavefile.c quit_dialog.c ingame_dialog.c message_dialog.c promote_dialog.c \
gamegui_driver.c victory_dialog.c system_dialog.c saveload_dialog.c title_dialog.c \
saveload_dc.c
