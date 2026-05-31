#include "terminal.h"
#include "config.h"
#include <pango/pango.h>

GtkWidget *g_term = NULL;

GtkWidget *create_terminal() {
    GtkWidget *term = vte_terminal_new();
    g_term = term;
    char *font = get_font_setting();
    apply_font_to_terminal(term, font);
    g_free(font);
    apply_theme_to_terminal(term);
    return term;
}

void apply_font_to_terminal(GtkWidget *term, const char *font) {
    if (font) {
        PangoFontDescription *font_desc = pango_font_description_from_string(font);
        vte_terminal_set_font(VTE_TERMINAL(term), font_desc);
        pango_font_description_free(font_desc);
    }
}

void apply_theme_to_terminal(GtkWidget *term) {
    if (!term || !VTE_IS_TERMINAL(term)) {
        g_warning("apply_theme_to_terminal: term 无效，跳过");
        return;
    }
    GdkRGBA *bg = get_theme_color("background");
    GdkRGBA *fg = get_theme_color("foreground");
    GdkRGBA *cursor = get_theme_color("cursor");
    GdkRGBA *selection = get_theme_color("selection");
    
    vte_terminal_set_colors(VTE_TERMINAL(term), fg, bg, NULL, 0);
    vte_terminal_set_color_cursor(VTE_TERMINAL(term), cursor);
    vte_terminal_set_color_highlight(VTE_TERMINAL(term), selection);
    
    g_free(bg);
    g_free(fg);
    g_free(cursor);
    g_free(selection);
}
