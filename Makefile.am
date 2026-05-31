bin_PROGRAMS = gv-console
gv_console_SOURCES = \
    src/main.c \
    src/config.c \
    src/terminal.c \
    src/settings.c

gv_console_CFLAGS = $(GTK_CFLAGS) $(VTE_CFLAGS) -I$(top_srcdir)/src
gv_console_LDADD = $(GTK_LIBS) $(VTE_LIBS)
