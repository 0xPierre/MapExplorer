/*
 * Copyright (C) 2021 James Westman <james@jwestman.net>
 *
 * Copyright 2024 Adrien
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config.h"
#include "shumate-test-tile-source.h"
#include "tp-2a-s4-window.h"
#include "DataPipeline.h"
#include "SearchTrie.h"
#include "SearchEngine.h"
#include "CommandLine.h"
#include "ShortestPath.h"


struct _Tp2aS4Window {
    AdwApplicationWindow parent_instance;

    // UI components
    GtkButton *addButton;
    GtkButton *generateTourBtn;
    GtkDropDown *layers_dropdown;
    GtkEntry *entry_completionAddress;
    GtkListView *list_view;
    ShumateSimpleMap *map;
    ShumateMapSourceRegistry *registry;
    ShumateMarkerLayer *marker_layer;
    ShumatePathLayer *path_layer;

    // Stores
    GtkListStore *travelStore; // Contain travel
    GtkListStore *addressStore; // Contain auto-fill address
    GtkListStore *citiesStore; // Contain auto-fill cities
    GList *markers; // Contain auto-fill cities

    Address *selectedAddress;

};

G_DEFINE_FINAL_TYPE (Tp2aS4Window, tp_2a_s4_window, ADW_TYPE_APPLICATION_WINDOW)

Tp2aS4Window *shumate_demo_window_new(AdwApplication *app) {
    return g_object_new(TP_2A_S4_TYPE_WINDOW, "application", app, NULL);
}


static void create_marker(Tp2aS4Window *self, double lat, double lng) {
    GtkWidget *image = gtk_image_new_from_icon_name("map-marker-symbolic");
    ShumateMarker *marker = shumate_marker_new();

    shumate_location_set_location(SHUMATE_LOCATION(marker), lat, lng);
    shumate_marker_set_child(marker, image);

    shumate_marker_layer_add_marker(self->marker_layer, marker);
    shumate_path_layer_add_node(self->path_layer, SHUMATE_LOCATION(marker));

    self->markers = g_list_prepend(self->markers, marker);
}

static void remove_all_markers(Tp2aS4Window *self) {
    GList *l;

    for (l = self->markers; l != NULL; l = l->next) {
        ShumateMarker *marker = SHUMATE_MARKER(l->data);
        shumate_marker_layer_remove_marker(self->marker_layer, marker);
        shumate_path_layer_remove_node(self->path_layer, SHUMATE_LOCATION(marker));
//        g_object_unref(marker);
    }

    g_list_free(self->markers);
    self->markers = NULL;
}


static void on_symbol_clicked(ShumateVectorRenderer *renderer, ShumateSymbolEvent *event, gpointer user_data) {
    const char *name = shumate_symbol_event_get_tag(event, "name");

    if (name != NULL)
        g_print("Symbol %s (%s) clicked in layer %s (%s) at (%f, %f)\n",
                shumate_symbol_event_get_feature_id(event),
                name,
                shumate_symbol_event_get_layer(event),
                shumate_symbol_event_get_source_layer(event),
                shumate_location_get_latitude(SHUMATE_LOCATION(event)),
                shumate_location_get_longitude(SHUMATE_LOCATION(event)));
    else
        g_print("Symbol %s clicked in layer %s (%s) at (%f, %f)\n",
                shumate_symbol_event_get_feature_id(event),
                shumate_symbol_event_get_layer(event),
                shumate_symbol_event_get_source_layer(event),
                shumate_location_get_latitude(SHUMATE_LOCATION(event)),
                shumate_location_get_longitude(SHUMATE_LOCATION(event)));
}


static void shumate_demo_window_dispose(GObject *object) {
    Tp2aS4Window *self = TP_2A_S4_WINDOW(object);

    g_clear_object (&self->registry);

    G_OBJECT_CLASS (tp_2a_s4_window_parent_class)->dispose(object);
}


static void tp_2a_s4_window_class_init(Tp2aS4WindowClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->dispose = shumate_demo_window_dispose;

    gtk_widget_class_set_template_from_resource(widget_class, "/com/potooFactory/eu/tp-2a-s4-window.ui");
    gtk_widget_class_bind_template_child (widget_class, Tp2aS4Window, map);
    gtk_widget_class_bind_template_child (widget_class, Tp2aS4Window, layers_dropdown);
    gtk_widget_class_bind_template_child(widget_class, Tp2aS4Window, addButton);
    gtk_widget_class_bind_template_child(widget_class, Tp2aS4Window, generateTourBtn);
    gtk_widget_class_bind_template_child(widget_class, Tp2aS4Window, list_view);
    gtk_widget_class_bind_template_child(widget_class, Tp2aS4Window, entry_completionAddress);
    gtk_widget_class_bind_template_callback (widget_class, on_symbol_clicked);
}

static gchar *get_map_source_name(ShumateMapSource *map_source) {
    return g_strdup (shumate_map_source_get_name(map_source));
}


gint get_list_store_size(GtkTreeModel *model) {
    GtkTreeIter iter;
    gboolean valid;
    gint count = 0;

    valid = gtk_tree_model_get_iter_first(model, &iter);
    while (valid) {
        count++;
        valid = gtk_tree_model_iter_next(model, &iter);
    }

    return count;
}

static void onAddAddress(GtkWidget *widget, Tp2aS4Window *self) {
    assert(self);
    GtkTreeIter iter;

    info("Added: Address: %s, City: %s Lat: %f, Long: %f\n",
         self->selectedAddress->nom_voie,
         self->selectedAddress->slug,
         self->selectedAddress->lat,
         self->selectedAddress->lon);


    gtk_list_store_append(self->travelStore, &iter);
    gtk_list_store_set(self->travelStore, &iter,
                       0, self->selectedAddress->lat,
                       1, self->selectedAddress->lon,
                       -1);

    create_marker(self, self->selectedAddress->lat, self->selectedAddress->lon);
    self->selectedAddress = NULL;
    gtk_widget_set_sensitive(GTK_WIDGET(self->addButton), FALSE);
    if (get_list_store_size(GTK_TREE_MODEL(self->travelStore)) > 1) {
        gtk_widget_set_sensitive(GTK_WIDGET(self->generateTourBtn), TRUE);
    }
}


static void onGenerationTour(GtkWidget *widget, Tp2aS4Window *self) {
    assert(self);
    GtkTreeIter iter;

    info("Your travel:\n");
    int positionCount = 0;
    double **positions = calloc(get_list_store_size(GTK_TREE_MODEL(self->travelStore)), sizeof(double *));
    for (int i = 0; i < get_list_store_size(GTK_TREE_MODEL(self->travelStore)); ++i) {
        positions[i] = calloc(2, sizeof(double));
    }
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self->travelStore), &iter);
    while (valid) {

        gdouble longitude;
        gdouble latitude;

        gtk_tree_model_get(GTK_TREE_MODEL(self->travelStore), &iter,
                           0, &latitude,
                           1, &longitude,
                           -1);
        positions[positionCount][0] = latitude;
        positions[positionCount][1] = longitude;

        debug("Latitude: %f, Longitude: %f\n", latitude, longitude);

        positionCount++;
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(self->travelStore), &iter);
    }

    printf("1\n");


    if (positionCount > 0) {
        printf("2\n");


        remove_all_markers(self);
        printf("3\n");


        Path ***paths;
        int nodes_id[positionCount];
        Path *shortestPath = GenerateTourFromMap(positions, positionCount, initMap->map, initMap->graph,
                                                 initMap->coords, &paths, nodes_id);
        printf("4\n");

        printf("30\n");

        ListIntIter *iterPath = ListIntIter_create(shortestPath->list);
        while (ListIntIter_isValid(iterPath)) {
            int id = ListIntIter_get(iterPath);
            create_marker(self, initMap->coords[nodes_id[id]][1], initMap->coords[nodes_id[id]][0]);

            ListIntIter_next(iterPath);

            if (!ListIntIter_isValid(iterPath)) {
                break;
            }

            int nextNextID = ListIntIter_get(iterPath);

            if (nextNextID == -1) break;

            printf("Id: %d NextId: %d\n", id, nextNextID);
            debug("%p\n", paths[id][nextNextID]->list);
            ListIntIter *iterP2 = ListIntIter_create(paths[id][nextNextID]->list);
            while (ListIntIter_isValid(iterP2)) {
                int nextID = ListIntIter_get(iterP2);

                ShumateMarker *marker = shumate_marker_new();

                shumate_location_set_location(SHUMATE_LOCATION(marker),
                                              initMap->coords[nextID][1],
                                              initMap->coords[nextID][0]);

                shumate_marker_layer_add_marker(self->marker_layer, marker);
                shumate_path_layer_add_node(self->path_layer, SHUMATE_LOCATION(marker));


                self->markers = g_list_prepend(self->markers, marker);
                ListIntIter_next(iterP2);
            }
        }
    }
}


void onAddressChange(GtkEntryCompletion *completion, Tp2aS4Window *self) {
    GtkTreeIter iter;
    gtk_list_store_clear(self->addressStore);

    int addressFoundLength;
    int addressFoundLengthInter;
    char *query = (char *) gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(completion)));
    Address **address_found = SearchTree_AutoCompleteAddressSearch(
            initMap->trie,
            ClearString(query),
            &addressFoundLength,
            9, 20);

    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(self->citiesStore), &iter)) {
        gchar *cityStr;

        gtk_tree_model_get(GTK_TREE_MODEL(self->citiesStore), &iter,
                           1, &cityStr,
                           -1);

        City *city = calloc(1, sizeof(City));
        strcat(city->slug, cityStr);

        Address **address_found_inter = IntersectQuery(address_found, addressFoundLength, city,
                                                       &addressFoundLengthInter);
        if (addressFoundLengthInter > 0) {
            address_found = address_found_inter;
            addressFoundLength = addressFoundLengthInter;
        }
        free(city);
    }

    if (address_found == NULL || addressFoundLength <= 0) return;

    for (int i = 0; i < addressFoundLength; ++i) {
        gtk_list_store_append(self->addressStore, &iter);

        int displayNameLength = strlen(address_found[i]->nom_voie) + strlen(address_found[i]->nom_commune) + 4;

        char displayName[displayNameLength];
        strcpy(displayName, address_found[i]->nom_voie);
        strcat(displayName, ", ");
        strcat(displayName, address_found[i]->nom_commune);

        gtk_list_store_set(self->addressStore, &iter,
                           0, address_found[i]->nom_voie,
                           1, address_found[i]->nom_commune,
                           2, address_found[i]->lat,
                           3, address_found[i]->lon,
                           4, displayName,
                           -1);
    }
}

static gboolean
always_match_func(GtkEntryCompletion *completion, const gchar *key, GtkTreeIter *iter, gpointer user_data) {
    return TRUE;
}

static gboolean
onCompletionAddressClicked(
        GtkEntryCompletion *completion,
        GtkTreeModel *model,
        GtkTreeIter *iter,
        Tp2aS4Window *self) {

    gchar *address;
    gchar *city;
    gdouble latitude;
    gdouble longitude;

    gtk_tree_model_get(model, iter,
                       0, &address,
                       1, &city,
                       2, &latitude,
                       3, &longitude,
                       -1);

    if (self->selectedAddress == NULL) {
        self->selectedAddress = (Address *) calloc(1, sizeof(Address));
    }
    strcpy(self->selectedAddress->nom_voie, address);
    strcpy(self->selectedAddress->slug, address);

    self->selectedAddress->lat = latitude;
    self->selectedAddress->lon = longitude;

    g_free(address);
    g_free(city);

    gtk_widget_set_sensitive(GTK_WIDGET(self->addButton), TRUE);

    return FALSE;
}

static void tp_2a_s4_window_init(Tp2aS4Window *self) {
    ShumateViewport *viewport;
    GtkExpression *expression;
    g_autoptr(GBytes) bytes = NULL;
    g_autoptr(GSimpleActionGroup) action_map = NULL;
    const char *style_json;
    GError *error = NULL;


    gtk_widget_init_template(GTK_WIDGET (self));

    action_map = g_simple_action_group_new();

    gtk_widget_insert_action_group(GTK_WIDGET (self), "win", G_ACTION_GROUP (action_map));

    self->registry = shumate_map_source_registry_new_with_defaults();
    shumate_map_source_registry_add(self->registry, SHUMATE_MAP_SOURCE(shumate_test_tile_source_new()));
    expression = gtk_cclosure_expression_new(G_TYPE_STRING, NULL, 0, NULL,
                                             (GCallback) get_map_source_name, NULL, NULL);
    gtk_drop_down_set_expression(self->layers_dropdown, expression);
    gtk_drop_down_set_model(self->layers_dropdown, G_LIST_MODEL(self->registry));


    GtkEntryCompletion *addressCompletion = gtk_entry_completion_new();
    GtkEntryCompletion *cityCompletion = gtk_entry_completion_new();

    // Store init
    self->citiesStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    self->addressStore = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE, G_TYPE_DOUBLE,
                                            G_TYPE_STRING);
    self->travelStore = gtk_list_store_new(2, G_TYPE_DOUBLE, G_TYPE_DOUBLE);

    self->markers = NULL; // Set list of markers to null

    // Address completion init
    gtk_entry_completion_set_model(addressCompletion, GTK_TREE_MODEL(self->addressStore));
    gtk_entry_completion_set_text_column(addressCompletion, 4);
    gtk_entry_completion_set_match_func(addressCompletion, always_match_func, NULL, NULL);
    gtk_entry_set_completion(GTK_ENTRY(self->entry_completionAddress), addressCompletion);

    gtk_widget_set_sensitive(GTK_WIDGET(self->addButton), FALSE); // Disable add button
    gtk_widget_set_sensitive(GTK_WIDGET(self->generateTourBtn), FALSE); // Disable add button

    if (shumate_vector_renderer_is_supported()) {
        bytes = g_resources_lookup_data("/com/potooFactory/eu/osm-liberty/style.json", G_RESOURCE_LOOKUP_FLAGS_NONE,
                                        NULL);
        style_json = g_bytes_get_data(bytes, NULL);

        ShumateVectorRenderer *renderer = shumate_vector_renderer_new(
                "vector-tiles",
                style_json,
                &error
        );

        if (error) {
            g_warning ("Failed to create vector map style: %s", error->message);
            g_clear_error(&error);
        } else {
            g_autoptr(GdkTexture) sprites_texture = NULL;
            g_autoptr(GBytes) sprites_json = NULL;
            g_autoptr(GdkTexture) sprites_2x_texture = NULL;
            g_autoptr(GBytes) sprites_2x_json = NULL;
            ShumateVectorSpriteSheet *sprites = NULL;

            sprites_json = g_resources_lookup_data("/com/potooFactory/eu/osm-liberty/sprites.json",
                                                   G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
            sprites_texture = gdk_texture_new_from_resource("/com/potooFactory/eu/osm-liberty/sprites.png");
            sprites_2x_json = g_resources_lookup_data("/com/potooFactory/eu/osm-liberty/sprites@2x.json",
                                                      G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
            sprites_2x_texture = gdk_texture_new_from_resource("/com/potooFactory/eu/osm-liberty/sprites@2x.png");

            sprites = shumate_vector_renderer_get_sprite_sheet(renderer);
            shumate_vector_sprite_sheet_add_page(sprites, sprites_texture, g_bytes_get_data(sprites_json, NULL), 1,
                                                 &error);
            if (error) {
                g_warning ("Failed to create spritesheet for vector map style: %s", error->message);
                g_clear_error(&error);
            }

            shumate_vector_sprite_sheet_add_page(sprites, sprites_2x_texture, g_bytes_get_data(sprites_2x_json, NULL),
                                                 2, &error);
            if (error) {
                g_warning ("Failed to create spritesheet for vector map style: %s", error->message);
                g_clear_error(&error);
            }

            shumate_map_source_set_license(SHUMATE_MAP_SOURCE(renderer), "© OpenMapTiles © OpenStreetMap contributors");
            shumate_map_source_registry_add(self->registry, SHUMATE_MAP_SOURCE(renderer));
            shumate_map_go_to_full(SHUMATE_MAP(shumate_simple_map_get_map(self->map)), 48.07552120827601,
                                   -0.7705708840901749, 9); // Go to Laval
        }
    }

    viewport = shumate_simple_map_get_viewport(self->map);

    /* Add the marker layers */
    self->marker_layer = shumate_marker_layer_new(viewport);
    shumate_simple_map_add_overlay_layer(self->map, SHUMATE_LAYER(self->marker_layer));
    self->path_layer = shumate_path_layer_new(viewport);
    shumate_simple_map_add_overlay_layer(self->map, SHUMATE_LAYER(self->path_layer));

    // Init signals
    g_signal_connect_data(self->addButton, "clicked", G_CALLBACK(onAddAddress), self, NULL, 0);
    g_signal_connect_data(self->entry_completionAddress, "changed", G_CALLBACK(onAddressChange), self, NULL, 0);
    g_signal_connect_data(addressCompletion, "match-selected", G_CALLBACK(onCompletionAddressClicked), self, NULL, 0);
    g_signal_connect_data(self->generateTourBtn, "clicked", G_CALLBACK(onGenerationTour), self, NULL, 0);
}
