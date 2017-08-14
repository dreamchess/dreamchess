set(GAMEGUI_SRCS
    action.c
    align.c
    bin.c
    box.c
    clipping.c
    container.c
    dialog.c
    edit.c
    entry.c
    frame.c
    hbox.c
    image.c
    label.c
    option.c
    scrollbarv.c
    select.c
    seperatorh.c
    seperatorv.c
    signal.c
    system.c
    vbox.c
    viewport.c
    widget.c
)

add_library(gamegui STATIC ${GAMEGUI_SRCS})
