#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>

void *handle_ble_connection() {
    while(1) {
        printf("%s", "Hello!!!!!\n");
        sleep(1);

    }

}

int main(void) {
    pthread_t *ble_conn_watcher = (pthread_t*) malloc(sizeof(pthread_t));

    // Start the thread that watches over interfaces
    pthread_create(ble_conn_watcher, NULL, &handle_ble_connection, NULL);

    pthread_join(*ble_conn_watcher, NULL);

    GError *err = NULL;
    GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);

    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to establish a connection with DBUS", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Connection to DBUS successful!");
    }

    GDBusProxy *proxy = g_dbus_proxy_new_sync(conn,
                                              G_DBUS_PROXY_FLAGS_NONE,
                                              NULL,
                                              "org.bluez",
                                              "/",
                                              "org.freedesktop.DBus.ObjectManager",
                                              NULL,
                                              &err);

    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create a proxy", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy created successfully!");
    }

    GVariant *result = g_dbus_proxy_call_sync(proxy,
                                              "GetManagedObjects",
                                              NULL,
                                              G_DBUS_CALL_FLAGS_NONE,
                                              -1,
                                              NULL,
                                              &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while fetching data", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Data fetched successfully");
    }

    if(result) {
        result = g_variant_get_child_value(result, 0);

        GVariantIter iter;
        g_variant_iter_init(&iter, result);
        gchar *obj_path;
        GVariant *if_prop;
        int i=0;
        while(g_variant_iter_next(&iter, "{&o@a{sa{sv}}}", &obj_path, &if_prop)) {
            g_print("%s\n", obj_path);
            const gchar *iface_name;
            GVariant *properties;
            GVariantIter iter2;
            g_variant_iter_init(&iter2, if_prop);
            while(g_variant_iter_next(&iter2, "{&s@a{sv}}", &iface_name, &properties)) {
                g_print("%s\n", iface_name);

                GVariantIter iter3;
                g_variant_iter_init(&iter3, properties);

                gchar *prop_name;
                GVariant *property;
                while(g_variant_iter_next(&iter3, "{&s@v}", &prop_name, &property)) {
                    g_print("%s\n", prop_name);
                }
                g_print("\n");
            }
            g_print("\n");
        }

        g_print("%i", i);
    }

}