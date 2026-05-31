#ifndef SETTINGS_H
#define SETTINGS_H

#include <gtk/gtk.h>

void show_settings_dialog(GtkWidget *window, GtkWidget *term);
GtkWidget *create_context_menu(GtkWidget *term);

#endif
