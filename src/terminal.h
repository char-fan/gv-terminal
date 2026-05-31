#ifndef TERMINAL_H
#define TERMINAL_H

#include <gtk/gtk.h>
#include <vte/vte.h>

extern GtkWidget *g_term;

GtkWidget *create_terminal(void);
void apply_font_to_terminal(GtkWidget *term, const char *font);
void apply_theme_to_terminal(GtkWidget *term);

#endif
