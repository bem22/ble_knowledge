//
// Created by bem22 on 10/03/2021.
//
#include "utils/list_utils.h"
#include "panoramalabs-service.h"
#include "panoramalabs-agent.h"
#include <stdio.h>


int agentFilterFlag= FILTER_WHITELIST;

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

    /**g_dbus_proxy_call_sync(dev_proxy,
                           "Set",
                           g_variant_new("(ssv)", "org.bluez.Device1", "Trusted", g_variant_new_boolean(TRUE)),
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if (!err) { g_print("Trusted!\n"); }
    else { g_print("%s", err->message); }
    **/

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
    g_print("Device path : %s\n", path);
    set_trusted(path);

    panorama_org_bluez_agent1_complete_request_authorization(agent, invocation);

    return TRUE;
}

static void pair_async_cb() {
    g_print("ready to pair");
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
    Node* device_node;
    GError *err = NULL;

    // Get all new interfaces
    g_variant_get(params, "(&oa{sa{sv}})", &object, &interfaces);

    if(agentFilterFlag == FILTER_WHITELIST) {
        for (int i = 0; i < l.length; i++) {
            device_node = get_from_index(&l, i);
            if (strstr(object, device_node->mac_addr) && strlen(object) < 38 && !device_node->paired) {
                g_print("Found: %s whitelisted\n", object);

                // Upon discovery create device proxies and reference to the list
                device_node->device_proxy = g_dbus_proxy_new_sync(conn,
                                                             G_DBUS_PROXY_FLAGS_NONE,
                                                             NULL,
                                                             "org.bluez",
                                                             object,
                                                             "org.bluez.Device1",
                                                             NULL,
                                                             &err);
                if(err) {
                    g_print("%s\n", err->message);
                    return;
                }

                device_node->paired = 1;

                // If PROPERTY(Paired+Trusted) == FALSE -> Call Pair

                g_dbus_proxy_call_sync(device_node->device_proxy, "Pair",
                                       NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &err);

            }
        }
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


    g_print("%s", "Interface removed\n");
}

int whitelist_init() {

    /**
    * Open file whitelist.conf
    **/
    FILE *fp;
    char * line = NULL;
    size_t len = 0;

    init_list(&l);

    fp = fopen("../whitelist.conf", "r");

    if (fp == NULL) {
        g_print("Missing file \n");
        return 0;
    }


    /**
     * 1. Read line by line
     * 2. Allocate mem resources for nodes and strings
     * 3. Check for errors and transform ':' into '_'
     * 4. Push nodes to list
    **/

     while ((getline(&line, &len, fp)) != -1) {

        // Initialize node
        Node *n = (Node*) malloc(sizeof(Node));
        if(!n) { printf("%s", "Failed to malloc"); return -1; }

        // Initialize string inside node
        n->mac_addr = malloc(18);
        if(!n->mac_addr) { printf("%s", "Failed to malloc!"); return -1; }

        // Set final character
        n->mac_addr[17] = '\0';

        // Set string string_length (including \0)
        n->string_length = 18;
        n->paired = 0;


        // Replaces MAC address separators with '_' to match bluez spec
            // 95 is '_' in ASCII
        g_strcanon(line, "ABCDEF0123456789_", 95);

        // Copy the line
        memcpy(n->mac_addr, line, n->string_length - 1);

        // Finally, push the node to the list
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

    // Stop discovery
    g_dbus_proxy_call_sync(adapter_proxy,
                           "StopDiscovery",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);

    if(err) {
        // TODO: Improve error handling here

        g_print("%s\n", err->message);
    } else { g_print("StopDiscovery\n");}

    err = NULL;
    int error_no = 0;


    for(int i=0; i<l.length; i++) {
        g_print("%s\n", g_strconcat("/org/bluez/hci0/dev_", get_from_index(&l, i)->mac_addr, NULL));

        g_dbus_proxy_call_sync(adapter_proxy,
                               "RemoveDevice",
                               g_variant_new("(o)", g_strconcat("/org/bluez/hci0/dev_", get_from_index(&l, i)->mac_addr, NULL)),
                               G_DBUS_CALL_FLAGS_NONE,
                               -1,
                               NULL,
                               &err);

        if(err) {
            g_print("%s\n", err->message);
            // TODO: Improve error handling here
        }
        err = NULL;


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
     * 2: Remove all whitelisted devices from bluetooth's list
     * 3: Subscribe to interface added signal with connection_signal_subscribe()
     * 4: Start discovery
     * TODO: React to added interfaces signals through callback function
     *     a) Check if added interface is device
     *     TODO: Check if filters are set (e.g whitelist/all/none etc)
     *     c) (If yes -> Check if the device is in whitelist)
     * TODO: Pair
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
