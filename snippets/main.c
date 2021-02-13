#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include <pthread.h>

void *handle_ble_connection() {
    while(1) {
        printf("%s", "Hello!!!!!\n");
        sleep(1);

    }

}

static void  hello_world(GDBusProxy *proxy, GAsyncResult *res) {
    GVariant *result = NULL;
    GError *err = NULL;
    result = g_dbus_proxy_call_finish(proxy, res, &err);

    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to establish a connection with DBUS", err->message);

    } else {
        g_print("\n%s %s\n", "Connection to DBUS successful: ", g_variant_get_type_string(result));
    }

}

int main(void) {
    GError *err = NULL;
    GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);

    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to establish a connection with DBUS", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Connection to DBUS successful!");
    }

    GDBusProxy *beacon_proxy = g_dbus_proxy_new_sync(conn,
                                                     G_DBUS_PROXY_FLAGS_NONE,
                                                     NULL,
                                                     "org.bluez",
                                                     "/org/bluez/hci0/dev_DC_3F_32_42_62_B0",
                                                     "org.bluez.Device1",
                                                     NULL,
                                                     &err);

    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create a proxy", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy created successfully!");
    }

    g_dbus_proxy_call_sync(beacon_proxy,
                           "Connect",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while calling a method through proxy123", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy method call, successful!");
    }

    sleep(3);


    GDBusProxy *gatt_proxy = g_dbus_proxy_new_sync(conn,
                                                   G_DBUS_PROXY_FLAGS_NONE,
                                                   NULL,
                                                   "org.bluez",
                                                   "/org/bluez/hci0/dev_DC_3F_32_42_62_B0/service0035/char0036",
                                                   "org.bluez.GattCharacteristic1",
                                                   NULL,
                                                   &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create a proxy", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy created successfully!");
    }

    for(int i=0; i<15; i++) {
        GVariantBuilder builder;
        g_variant_builder_init (&builder, G_VARIANT_TYPE("a{sv}"));
        GVariant *argument = g_variant_builder_end(&builder);

        char* data = "1";
        if(i%2==0) {
            data = "";
        } else {data = "1";}
        GVariant *char_value = g_variant_new_from_data(G_VARIANT_TYPE("ay"), data, 1, TRUE, NULL, NULL);


        GVariant *result = g_dbus_proxy_call_sync(gatt_proxy,
                                                  "WriteValue",
                                                  g_variant_new("(@ay@a{sv})", char_value, argument),
                                                  G_DBUS_CALL_FLAGS_NONE,
                                                  -1,
                                                  NULL,
                                                  &err);
        if(err != NULL) {
            g_print("%s\n%s\n", "Error while calling a method through proxy", err->message);
            return 1;
        } else {
            g_print("\n%s\n", "Proxy method call, successful!");
        }

        usleep(100);
    }

    GDBusProxy *gatt_proxy2 = g_dbus_proxy_new_sync(conn,
                                                   G_DBUS_PROXY_FLAGS_NONE,
                                                   NULL,
                                                   "org.bluez",
                                                   "/org/bluez/hci0/dev_DC_3F_32_42_62_B0/service0018/char0022",
                                                   "org.bluez.GattCharacteristic1",
                                                   NULL,
                                                   &err);

    g_dbus_proxy_call(gatt_proxy2,
                           "StartNotify",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           hello_world,
                           NULL);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while starting notify", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Notify started!");
    }



    g_main_loop_run(loop);
    g_object_unref(conn);


}