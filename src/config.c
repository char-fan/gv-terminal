#include "config.h"
#include <glib.h>
#include <stdio.h>

#define CONFIG_DIR  "gv-console"
#define CONFIG_FILE "config.ini"

static GKeyFile *load_config() {
    GKeyFile *keyfile = g_key_file_new();
    gchar *path = g_build_filename(g_get_user_config_dir(), CONFIG_DIR, CONFIG_FILE, NULL);
    
    if (g_file_test(path, G_FILE_TEST_EXISTS)) {
        g_key_file_load_from_file(keyfile, path, G_KEY_FILE_NONE, NULL);
    }
    g_free(path);
    return keyfile;
}

static void save_config(GKeyFile *keyfile) {
    gchar *path = g_build_filename(g_get_user_config_dir(), CONFIG_DIR, NULL);
    g_mkdir_with_parents(path, 0755);
    g_free(path);
    
    path = g_build_filename(g_get_user_config_dir(), CONFIG_DIR, CONFIG_FILE, NULL);
    g_key_file_save_to_file(keyfile, path, NULL);
    g_free(path);
}

char *get_font_setting() {
    GKeyFile *keyfile = load_config();
    char *font = g_key_file_get_string(keyfile, "terminal", "font", NULL);
    if (!font) {
        font = NULL;
    }
    g_key_file_free(keyfile);
    return font;
}

void set_font_setting(const char *font) {
    GKeyFile *keyfile = load_config();
    g_key_file_set_string(keyfile, "terminal", "font", font);
    save_config(keyfile);
    g_key_file_free(keyfile);
}

char *get_config_path() {
    return g_build_filename(g_get_user_config_dir(), CONFIG_DIR, CONFIG_FILE, NULL);
}

GdkRGBA *get_theme_color(const char *key) {
    GKeyFile *keyfile = load_config();
    char *color_str = g_key_file_get_string(keyfile, "theme", key, NULL);
    g_key_file_free(keyfile);

    if (!color_str) {
        // 默认颜色
        GdkRGBA *default_color = g_new(GdkRGBA, 1);
        if (strcmp(key, "background") == 0) {
            gdk_rgba_parse(default_color, "#1e1e1e"); // 深色背景
        } else if (strcmp(key, "foreground") == 0) {
            gdk_rgba_parse(default_color, "#ffffff"); // 白色前景
        } else if (strcmp(key, "cursor") == 0) {
            gdk_rgba_parse(default_color, "#ffffff"); // 白色光标
        } else if (strcmp(key, "selection") == 0) {
            gdk_rgba_parse(default_color, "#333333"); // 深色选中
        } else {
            gdk_rgba_parse(default_color, "#000000");
        }
        return default_color;
    }

    GdkRGBA *color = g_new(GdkRGBA, 1);
    gdk_rgba_parse(color, color_str);
    g_free(color_str);
    return color;
}

void set_theme_color(const char *key, const GdkRGBA *color) {
    GKeyFile *keyfile = load_config();
    char *color_str = gdk_rgba_to_string(color);
    g_key_file_set_string(keyfile, "theme", key, color_str);
    g_free(color_str);
    save_config(keyfile);
    g_key_file_free(keyfile);
}
