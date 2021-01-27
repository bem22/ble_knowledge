#include <glib.h>
#include <gio/gio.h>

int main(void) {
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
        g_variant_get_child_value(result, 0);
        GVariantIter iter;
        g_variant_iter_init(&iter, result);

        int i=0;
        while(g_variant_iter_next(&iter, "a{oa{sa{sv}}}", NULL)) {
            i++;
        }

        g_print("%i", i);
    }



}