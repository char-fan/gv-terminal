bin_PROGRAMS = gv-terminal
gv_terminal_SOURCES = \
    src/main.c \
    src/config.c \
    src/terminal.c \
    src/settings.c

gv_terminal_CFLAGS = $(GTK_CFLAGS) $(VTE_CFLAGS) -I$(top_srcdir)/src
gv_terminal_LDADD = $(GTK_LIBS) $(VTE_LIBS)

desktopdir = $(datadir)/applications
desktop_DATA = data/gv-terminal.desktop

icondir = $(datadir)/icons/hicolor/128x128/apps
icon_DATA = data/gv-terminal.png
