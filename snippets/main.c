#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#define AGENT_PATH "/org/bluez/AutoPinAgent"
static void  hello_world(GDBusProxy          *proxy,
                         GVariant            *changed_properties,
                         const gchar* const  *invalidated_properties,
                         gpointer             user_data) {
    changed_properties = g_variant_get_child_value(changed_properties, 0);
    GVariant *string = g_variant_get_child_value(changed_properties, 0);
    GVariant *variant = g_variant_get_child_value(changed_properties, 1);
    variant = g_variant_get_child_value(variant, 0);
    if (g_variant_n_children (changed_properties) > 0) {

        g_print("[1]\n");
    }
}


static void  hello_world2(GDBusProxy          *proxy,
                         GVariant            *changed_properties,
                         const gchar* const  *invalidated_properties,
                         gpointer             user_data) {
    changed_properties = g_variant_get_child_value(changed_properties, 0);
    GVariant *string = g_variant_get_child_value(changed_properties, 0);
    GVariant *variant = g_variant_get_child_value(changed_properties, 1);
    variant = g_variant_get_child_value(variant, 0);
    if (g_variant_n_children (changed_properties) > 0) {

        g_print("[2]\n");
    }
}


int main(void) {
    GError *err = NULL;
    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);

    GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);


    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to establish a connection with DBUS", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Connection to DBUS successful!");
    }

    GDBusProxy *agent_manager_proxy = g_dbus_proxy_new_sync(conn,
                                                    G_DBUS_PROXY_FLAGS_NONE,
                                                    NULL,
                                                    "org.bluez",
                                                    "/org/bluez",
                                                    "org.bluez.AgentManager1",
                                                    NULL,
                                                    &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create agent proxy", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Agent proxy created!");
    }

    g_dbus_proxy_call_sync(agent_manager_proxy,
                          "RegisterAgent",
                          g_variant_new("(os)", AGENT_PATH, "NoInputNoOutput"),
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          NULL,
                          &err);

    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to register agent", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Agent registered.");
    }

    g_dbus_proxy_call_sync(agent_manager_proxy,
                           "RequestDefaultAgent",
                           g_variant_new("(o)", AGENT_PATH),
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);

    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to register agent", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Agent now default.");
    }


    GDBusProxy *adapter_proxy = g_dbus_proxy_new_sync(conn,
                                                G_DBUS_PROXY_FLAGS_NONE,
                                                NULL,
                                                "org.bluez",
                                                "/org/bluez/hci0",
                                                "org.bluez.Adapter1",
                                                NULL,
                                                &err);

    if(err != NULL) {
        g_print("%s\n%s\n", "Adapter proxy creation failed.", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Adapter proxy created");
    }

    g_dbus_proxy_call_sync(adapter_proxy,
                           "StartDiscovery",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Start discovery failed", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Start discovery");
    }

    sleep(10);

    GDBusProxy *device_properties_proxy = g_dbus_proxy_new_sync(conn,
                                                   G_DBUS_PROXY_FLAGS_NONE,
                                                   NULL,
                                                   "org.bluez",
                                                   "/org/bluez/hci0/dev_DC_3F_32_42_62_B0",
                                                   "org.freedesktop.DBus.Properties",
                                                   NULL,
                                                   &err);

    g_dbus_proxy_call_sync(device_properties_proxy,
                           "Set",
                           g_variant_new("(ssv)", "org.bluez.Device1", "Trusted", g_variant_new_boolean(TRUE)),
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Not so trusted", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Trusted");
    }

    GDBusProxy *device_proxy = g_dbus_proxy_new_sync(conn,
                                                     G_DBUS_PROXY_FLAGS_NONE,
                                                     NULL,
                                                     "org.bluez",
                                                     "/org/bluez/hci0/dev_DC_3F_32_42_62_B0",
                                                     "org.bluez.Device1",
                                                     NULL,
                                                     &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create device proxy", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Device proxy created!");
    }




    g_dbus_proxy_call_sync(device_proxy,
                           "Pair",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Pair error", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Device paired!");
    }



    /*
    g_dbus_proxy_call_sync(device_proxy,
                           "Connect",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create device proxy", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Device connected!");
    }

    sleep(3);



    GDBusProxy *gatt_proxy5 = g_dbus_proxy_new_sync(conn,
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
        g_print("\n%s\n", "GATT Proxy created successfully!");
    }

    /*for(int i=0; i<15; i++) {
        GVariantBuilder builder;
        g_variant_builder_init (&builder, G_VARIANT_TYPE("a{sv}"));
        GVariant *argument = g_variant_builder_end(&builder);

        char* data = "1";
        if(i%2==0) {
            data = "";
        } else {data = "1";}
        GVariant *char_value = g_variant_new_from_data(G_VARIANT_TYPE("ay"), data, 1, TRUE, NULL, NULL);


        GVariant *result = g_dbus_proxy_call_sync(gatt_proxy5,
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
                                                   "/org/bluez/hci0/dev_DC_3F_32_42_62_B0/service0018/char0019",
                                                   "org.bluez.GattCharacteristic1",
                                                   NULL,
                                                   &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while starting notify", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Notify started!");
    }

    g_dbus_proxy_call_sync(gatt_proxy2,
                           "StartNotify",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if(err != NULL) {
        g_print("%s\n%s\n", "Error while starting notify", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Notify started!");
    }


    g_signal_connect(gatt_proxy2,
                     "g-properties-changed",
                     G_CALLBACK(hello_world),
                     NULL);


    if(err != NULL) {
        g_print("%s\n%s\n", "Error while starting notify", err->message);
        return 1;
    } else {
        g_print("\n%s\n", "Notify started!");
    }

    */

    g_main_loop_run(loop);
    g_object_unref(conn);
}