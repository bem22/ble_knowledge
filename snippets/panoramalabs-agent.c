//
// Created by bem22 on 10/03/2021.
//
#include "panoramalabs-service.h"
#include "panoramalabs-agent.h"

int agentFilterFlag= FILTER_NO_FILTER;

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

int autopair_init() {

    // TODO: Subscribe to added interfaces
    // TODO: Start discovery
    // TODO: Check if added interface is device
    // TODO: Check if filters are set
        // TODO (If yes -> Check if the device is in the filter)
            // TODO (If yes -> Pair)
    return 0;
}

