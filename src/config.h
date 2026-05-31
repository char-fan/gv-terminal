#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>
#include <gtk/gtk.h>

char *get_font_setting(void);
void set_font_setting(const char *font);
char *get_config_path(void);
GdkRGBA *get_theme_color(const char *key);
void set_theme_color(const char *key, const GdkRGBA *color);

#endif
