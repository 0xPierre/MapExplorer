// Welcome to Map Explorer by Adrien & Pierre

#include "Graph.h"
#include "Settings.h"
#include "FileParser.h"
#include "DataPipeline.h"
#include "SpatialMap.h"
#include "SearchTrie.h"
#include "TextUtils.h"
#include "Test.h"
#include "SearchEngine.h"
#include "CommandLine.h"
#include "Evolution.h"
#include "TSPAlgorithm.h"





#if UI_ENABLED
#include "config.h"
#include <glib/gi18n.h>
#include <shumate/shumate.h>

#include "tp-2a-s4-application.h"
#include "tp-2a-s4-window.h"

static void activate(AdwApplication *app, gpointer user_data) {
    Tp2aS4Window *window;

    window = shumate_demo_window_new(app);
    gtk_window_present(GTK_WINDOW (window));
}
#endif

int main(int argc, char *argv[]) {
#if UI_ENABLED
    init();
    g_autoptr(Tp2aS4Application) app = NULL;
    int ret;

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    app = tp_2a_s4_application_new("com.potooFactory.eu", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    ret = g_application_run(G_APPLICATION (app), argc, argv);
    return ret;
#endif

#if !UI_ENABLED
srand(time(NULL));

#if RUN_CLI
    MainLoop();
#endif

#if TEST_SIMPLE_ACO
    TestSimpleAco(FILENAME_SIMPLE_ACO);
#endif

#if TEST_ADDRESS_AUTOCOMPLETE
    TestAddressAndNearIntersection();
#endif

#if TEST_TSP_BRUTEFORCE
    TestBruteForceTSP(TEST_TSP_BRUTEFORCE_FILE);
#endif

    return 0;
#endif
}
