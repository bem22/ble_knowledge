//
// Created by bem22 on 10/03/2021.
//
#include "utils/list_utils.h"
#include "panoramalabs-service.h"
#include "panoramalabs-agent.h"
#include <stdio.h>


int agentFilterFlag= FILTER_NO_FILTER;

GDBusProxy *adapter_proxy;

int set_agent_filter(PanoramaAgentFilterFlags flag) {
    if (flag == FILTER_NO_FILTER || flag == FILTER_SINGLE_DEVICE || flag == FILTER_WHITELIST) {
        agentFilterFlag = flag;
        return 0;
    } else return 1;
}

void set_trusted(gchar *path) {
    GError *err = NULL;
    GDBusProxy *dev_proxy = g_dbus_proxy_new_sync(conn,
                                                  G_DBUS_PROXY_FLAGS_NONE,
                                                  NULL,
                                                  "org.bluez",
                                                  "/org/bluez/hci0/dev_DC_3F_32_42_62_B0",
                                                  "org.freedesktop.DBus.Properties",
                                                  NULL,
                                                  &err);
    if (!err) { g_print("proxy\n"); }

    g_dbus_proxy_call_sync(dev_proxy,
                           "Set",
                           g_variant_new("(ssv)", "org.bluez.Device1", "Trusted", g_variant_new_boolean(TRUE)),
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if (!err) { g_print("Trusted!\n"); }
    else { g_print("%s", err->message); }

    g_object_unref(dev_proxy);
}

int on_request_confirmation(PanoramaOrgBluezAgent1 *agent,
                            GDBusMethodInvocation *invocation,
                            gchar *path,
                            guint32 passkey,
                            gpointer userdata) {
    g_print("Hello WORLD!");
    set_trusted(path);
    return TRUE;
}

int on_request_authorization(PanoramaOrgBluezAgent1 *agent,
                             GDBusMethodInvocation *invocation,
                             gchar *path) {
    g_print("Device path : %s", path);
    set_trusted(path);

    return TRUE;
}
int check_mac_whitelist(gchar* mac_address) {
    return 1;
}

// TODO: Write callback function for verifying added interface
static void interface_added_callback(GDBusConnection *con,
                              const gchar *sender,
                              const gchar *path,
                              const gchar *interface,
                              const gchar *signal,
                              GVariant *params,
                              void *userdata) {
    (void)conn;
    (void)sender;
    (void)path;
    (void)interface;

    static int exit = 0;


    GVariantIter *interfaces;
    const char *object;
    const gchar *interface_name;
    GVariant *properties;

    g_variant_get(params, "(&oa{sa{sv}})", &object, &interfaces);
    while(g_variant_iter_next(interfaces, "{&s@a{sv}}", &interface_name, &properties)) {
        if(g_strstr_len(g_ascii_strdown(interface_name, -1), -1, "device")) {
            g_print("[ %s ]\n", object);
            const gchar *property_name;
            GVariantIter i;
            GVariant *prop_val;
            g_variant_iter_init(&i, properties);
            while(g_variant_iter_next(&i, "{&sv}", &property_name, &prop_val))
                //bluez_property_value(property_name, prop_val);
            g_variant_unref(prop_val);
        }
        g_variant_unref(properties);
    }

}
static void interface_removed_callback(GDBusConnection *con,
                                       const gchar *sender,
                                       const gchar *path,
                                       const gchar *interface,
                                       const gchar *signal,
                                       GVariant *params,
                                       void *userdata) {
    (void) conn;
    (void) sender;
    (void) path;
    (void) interface;

    static int exit = 0;


    //g_print("%s", "Interface removed\n");
}

int whitelist_init() {

    /**
    * Open file whitelist.conf
    **/
    FILE *fp;
    char * line = NULL;
    size_t len = 0;

    whitelisted.count = 0;

    init_list(&l);
    l.length = 0;

    fp = fopen("../whitelist.conf", "r");

    if (fp == NULL) {
        g_print("Missing file \n");
        return 0;
    }


    // TODO: Init whitelist memory (malloc / !check for alloc errors)
    whitelisted.paths = (char**) malloc(sizeof(char**));

    if(!whitelisted.paths) {
        return 0;
    }

    // TODO: Read line by line
    // TODO: !! FIND OUT WHY THE LINE IS NOT READ CORRECTLY !! (realloc is a cause - it goes to new wiped memory)

    while ((getline(&line, &len, fp)) != -1) {

        Node *n = (Node*) malloc(sizeof(Node));

        n->string = malloc(18);
        n->string[17] = '\0';

        n->length = 18;
        memcpy(n->string, line, n->length-1);

        push(&l, n);
        free(line);
        line = NULL;
    }

    fclose(fp);

    if(line) {
        free(line);
    }

    return 0;
}

int adapter_remove_whitelisted_devices() {
    GError *err = NULL;

    int error_no = 0;

    for(int i=0; i<whitelisted.count; i++) {
        /* g_dbus_proxy_call_sync(adapter_proxy,
                               "RemoveDevice",
                // TODO: Copy path from whitelist.path to whitelist_base_path
                               g_variant_new("/org/bluez/hci0/dev_"),
                               G_DBUS_CALL_FLAGS_NONE,
                               -1,
                               NULL,
                               &err);
        if(err) {
            // TODO: Improve error handling here
            return -1;
        }
         */

    }
    for(int i=0; i<3; i++) {
        g_print("%s", pop(&l, i)->string);
    }

    return error_no;

}

int autopair_init(gpointer loop) {
    GError *err = NULL;

    whitelist_init();

    // Adapter setup
    adapter_proxy = g_dbus_proxy_new_sync(conn,
                                          G_DBUS_PROXY_FLAGS_NONE,
                                          NULL,
                                          "org.bluez",
                                          "/org/bluez/hci0",
                                          "org.bluez.Adapter1",
                                          NULL,
                                          &err);
    if(err) {
        // TODO: Improve error handling here.
        g_print("Hello");
        return -1;
    }

    /**
     * Pairing strategy for BLE auto-connect to known devices
     * 1: Stop discovery
     * TODO: Remove all whitelisted devices from bluetooth's list
     * 3: Subscribe to interface added signal with connection_signal_subscribe()
     * 4: Start discovery
     * TODO: React to added interfaces signals through callback function
     *     TODO: Check if added interface is device
     *     TODO: Check if filters are set (e.g whitelist/all/none etc)
     *     TODO: (If yes -> Check if the device is in whitelist)
     * TODO: Pair
     */

    /* Stop Discovery
    g_dbus_proxy_call_sync(adapter_proxy,
                           "StopDiscovery",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);

    if(err) {
        // TODO: Improve error handling here

        g_print("%s", err->message);
        return -1;
    } else { g_print("StopDiscovery\n");}
    */
    adapter_remove_whitelisted_devices();

    // Subscribe to interfaces added signal
    guint iface_added = g_dbus_connection_signal_subscribe(conn,
                                                     "org.bluez",
                                                     "org.freedesktop.DBus.ObjectManager",
                                                     "InterfacesAdded",
                                                     NULL,
                                                     NULL,
                                                     G_DBUS_SIGNAL_FLAGS_NONE,
                                                     interface_added_callback,
                                                     loop,
                                                     NULL);

    // Subscribe to interface removed signal
    guint iface_removed = g_dbus_connection_signal_subscribe(conn,
                                                       "org.bluez",
                                                       "org.freedesktop.DBus.ObjectManager",
                                                       "InterfacesRemoved",
                                                       NULL,
                                                       NULL,
                                                       G_DBUS_SIGNAL_FLAGS_NONE,
                                                       interface_removed_callback,
                                                       loop,
                                                       NULL);

    // Start Discovery
    g_dbus_proxy_call_sync(adapter_proxy,
                           "StartDiscovery",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);

    if(err) {
        // TODO: Improve error handling here

        g_print("%s", err->message);
        return -1;
    } else { g_print("StartDiscovery\n");}

    return 0;
}
