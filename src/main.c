#include <gtk/gtk.h>
#include <vte/vte.h>
#include <sys/prctl.h>
#include "terminal.h"
#include "settings.h"

static void on_child_exited(VteTerminal *term, gpointer data) {
    GtkWidget *window = GTK_WIDGET(data);
    gtk_window_close(GTK_WINDOW(window));
}

static gboolean on_terminal_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->button == GDK_BUTTON_SECONDARY) {
        GtkWidget *menu = GTK_WIDGET(data);
        gtk_menu_popup_at_pointer(GTK_MENU(menu), NULL);
        return TRUE;
    }
    return FALSE;
}

int main(int argc, char *argv[]) {
    prctl(PR_SET_NAME, "gv-terminal", 0, 0, 0);

    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "gv-terminal");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);

    GtkWidget *term = create_terminal();

    const char *shell = getenv("SHELL");
    if (!shell) shell = "/usr/bin/sh";
    const char *cmd[] = { shell, NULL };
    vte_terminal_spawn_async(VTE_TERMINAL(term),
                             VTE_PTY_DEFAULT,
                             g_get_home_dir(),
                             (char **)cmd,
                             NULL,
                             G_SPAWN_DEFAULT,
                             NULL,
                             NULL,
                             NULL,
                             -1,
                             NULL,
                             NULL,
                             NULL);

    gtk_container_add(GTK_CONTAINER(window), term);
    g_signal_connect(window, "destroy", gtk_main_quit, NULL);
    g_signal_connect(term, "child-exited", G_CALLBACK(on_child_exited), window);

    GtkWidget *menu = create_context_menu(term);
    g_signal_connect(term, "button-press-event", G_CALLBACK(on_terminal_button_press), menu);

    GtkAccelGroup *accel = gtk_accel_group_new();
gtk_window_add_accel_group(GTK_WINDOW(window), accel);

gtk_widget_add_accelerator(term, "copy-clipboard", accel,
                           GDK_KEY_c, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
gtk_widget_add_accelerator(term, "paste-clipboard", accel,
                           GDK_KEY_v, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
