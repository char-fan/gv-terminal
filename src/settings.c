#include "settings.h"
#include "config.h"
#include "terminal.h"
#include <glib.h>
#include <pango/pango.h>
#include <pango/pangofc-fontmap.h>

static void on_refresh_theme_clicked(GtkButton *button, gpointer data) {
    apply_theme_to_terminal(g_term);
}

static void on_color_button_clicked(GtkColorButton *button, gpointer data) {
    const char *key = (const char *)data;
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(button), &color);
    set_theme_color(key, &color);
}

static void on_copy_clicked(GtkMenuItem *item, gpointer data) {
    GtkWidget *term = GTK_WIDGET(data);
    vte_terminal_copy_clipboard_format(VTE_TERMINAL(term), VTE_FORMAT_TEXT);
}

static void on_paste_clicked(GtkMenuItem *item, gpointer data) {
    GtkWidget *term = GTK_WIDGET(data);
    vte_terminal_paste_clipboard(VTE_TERMINAL(term));
}

static void on_apply_font_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    GtkWidget *term = GTK_WIDGET(data);
    
    GtkWidget *font_combo = g_object_get_data(G_OBJECT(dialog), "font_combo");
    GtkWidget *size_spin = g_object_get_data(G_OBJECT(dialog), "size_spin");
    
    const char *font_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(font_combo));
    int font_size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(size_spin));
    
    if (font_name) {
        char *font_string = g_strdup_printf("%s %d", font_name, font_size);
        set_font_setting(font_string);
        apply_font_to_terminal(term, font_string);
        g_free(font_string);
    }
}

void show_settings_dialog(GtkWidget *window, GtkWidget *term) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "设置 - gv-console", GTK_WINDOW(window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "关闭", GTK_RESPONSE_CLOSE,
        NULL);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 10);
    gtk_box_set_spacing(GTK_BOX(content), 10);

    GtkWidget *font_label = gtk_label_new("字体：");
    gtk_container_add(GTK_CONTAINER(content), font_label);

    GtkWidget *font_combo = gtk_combo_box_text_new();
    
    PangoFontMap *fontmap = pango_cairo_font_map_get_default();
    PangoFontFamily **families;
    int n_families;
    pango_font_map_list_families(fontmap, &families, &n_families);
    
    for (int i = 0; i < n_families; i++) {
        const char *family_name = pango_font_family_get_name(families[i]);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(font_combo), family_name, family_name);
    }
    g_free(families);

    char *current_font = get_font_setting();
    char *font_name = NULL;
    int font_size = 12;
    if (current_font) {
        char *space = strrchr(current_font, ' ');
        if (space) {
            *space = '\0';
            font_name = current_font;
            font_size = atoi(space + 1);
        }
    }
    
    if (font_name) {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(font_combo));
        GtkTreeIter iter;
        gboolean found = FALSE;
        if (gtk_tree_model_get_iter_first(model, &iter)) {
            do {
                char *name;
                gtk_tree_model_get(model, &iter, 0, &name, -1);
                if (g_strcmp0(name, font_name) == 0) {
                    gtk_combo_box_set_active_iter(GTK_COMBO_BOX(font_combo), &iter);
                    found = TRUE;
                }
                g_free(name);
            } while (!found && gtk_tree_model_iter_next(model, &iter));
        }
        if (!found) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(font_combo), 0);
        }
    } else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(font_combo), 0);
    }
    g_free(current_font);

    gtk_container_add(GTK_CONTAINER(content), font_combo);

    GtkWidget *size_label = gtk_label_new("字号：");
    gtk_container_add(GTK_CONTAINER(content), size_label);

    GtkWidget *size_spin = gtk_spin_button_new_with_range(8, 48, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(size_spin), font_size);
    gtk_container_add(GTK_CONTAINER(content), size_spin);

    GtkWidget *font_apply_btn = gtk_button_new_with_label("应用字体");
    g_signal_connect(font_apply_btn, "clicked", G_CALLBACK(on_apply_font_clicked), term);
    gtk_container_add(GTK_CONTAINER(content), font_apply_btn);

    GtkWidget *theme_label = gtk_label_new("主题颜色：");
    gtk_container_add(GTK_CONTAINER(content), theme_label);

    GtkWidget *bg_label = gtk_label_new("背景：");
    gtk_container_add(GTK_CONTAINER(content), bg_label);

    GdkRGBA *bg_color = get_theme_color("background");
    GtkWidget *bg_button = gtk_color_button_new_with_rgba(bg_color);
    g_signal_connect(bg_button, "color-set", G_CALLBACK(on_color_button_clicked), "background");
    gtk_container_add(GTK_CONTAINER(content), bg_button);
    g_free(bg_color);

    GtkWidget *fg_label = gtk_label_new("前景：");
    gtk_container_add(GTK_CONTAINER(content), fg_label);
    
    GdkRGBA *fg_color = get_theme_color("foreground");
    GtkWidget *fg_button = gtk_color_button_new_with_rgba(fg_color);
    g_signal_connect(fg_button, "color-set", G_CALLBACK(on_color_button_clicked), "foreground");
    gtk_container_add(GTK_CONTAINER(content), fg_button);
    g_free(fg_color);

    GtkWidget *cursor_label = gtk_label_new("光标：");
    gtk_container_add(GTK_CONTAINER(content), cursor_label);

    GdkRGBA *cursor_color = get_theme_color("cursor");
    GtkWidget *cursor_button = gtk_color_button_new_with_rgba(cursor_color);
    g_signal_connect(cursor_button, "color-set", G_CALLBACK(on_color_button_clicked), "cursor");
    gtk_container_add(GTK_CONTAINER(content), cursor_button);
    g_free(cursor_color);

    GtkWidget *sel_label = gtk_label_new("选中：");
    gtk_container_add(GTK_CONTAINER(content), sel_label);
    
    GdkRGBA *sel_color = get_theme_color("selection");
    GtkWidget *sel_button = gtk_color_button_new_with_rgba(sel_color);
    g_signal_connect(sel_button, "color-set", G_CALLBACK(on_color_button_clicked), "selection");
    gtk_container_add(GTK_CONTAINER(content), sel_button);
    g_free(sel_color);

    GtkWidget *refresh_btn = gtk_button_new_with_label("刷新主题");
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_refresh_theme_clicked), NULL);
    gtk_container_add(GTK_CONTAINER(content), refresh_btn);

    g_object_set_data(G_OBJECT(dialog), "font_combo", font_combo);
    g_object_set_data(G_OBJECT(dialog), "size_spin", size_spin);

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

GtkWidget *create_context_menu(GtkWidget *term) {
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *copy_item = gtk_menu_item_new_with_label("复制");
    g_signal_connect(copy_item, "activate", G_CALLBACK(on_copy_clicked), term);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), copy_item);
    GtkWidget *paste_item = gtk_menu_item_new_with_label("粘贴");
    g_signal_connect(paste_item, "activate", G_CALLBACK(on_paste_clicked), term);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), paste_item);
    GtkWidget *settings_item = gtk_menu_item_new_with_label("设置");
    g_signal_connect(settings_item, "activate", G_CALLBACK(show_settings_dialog), term);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), settings_item);
    gtk_widget_show_all(menu);
    return menu;
}
