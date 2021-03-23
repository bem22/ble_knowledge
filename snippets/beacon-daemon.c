#include "panoramalabs-service.h"

/* ---------------------------------------------------------------------------------------------------- */

static GDBusObjectManagerServer *manager = NULL;
GDBusConnection *conn;

gboolean handle_request_authorization() {
    return TRUE;
}
void set_trusted(gchar* path) {
    GError *err = NULL;
    GDBusProxy *dev_proxy = g_dbus_proxy_new_sync(conn,
                                                 G_DBUS_PROXY_FLAGS_NONE,
                                                 NULL,
                                                 "org.bluez",
                                                 "/org/bluez/hci0/dev_DC_3F_32_42_62_B0",
                                                 "org.freedesktop.DBus.Properties",
                                                 NULL,
                                                 &err);
    if(!err) { g_print("proxy\n"); }

    g_dbus_proxy_call_sync(dev_proxy,
                           "Set",
                           g_variant_new("(ssv)", "org.bluez.Device1", "Trusted", g_variant_new_boolean(TRUE)),
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &err);
    if(!err) { g_print("Trusted!\n"); }
    else { g_print("%s", err->message);}

    g_object_unref(dev_proxy);
}


int on_request_confirmation(PanoramaOrgBluezAgent1 *agent,
        GDBusMethodInvocation *invocation,
        gchar* path,
        guint32 passkey,
        gpointer userdata) {
    g_print("Hello WORLD!");
    set_trusted(path);
    return TRUE;
}

int on_request_authorization(PanoramaOrgBluezAgent1 *agent,
                             GDBusMethodInvocation *invocation,
                             gchar* path) {
    g_print("Device path : %s", path);
    set_trusted(path);

    return TRUE;
}

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
    conn = connection;
    PanoramaObjectSkeleton *object;
    PanoramaOrgBluezAgent1 *agent;
    guint n;

    g_print ("Acquired a message bus connection\n");


    manager = g_dbus_object_manager_server_new ("/");

    gchar *s = "/com/panoramalabs/Agent1";
    object = panorama_object_skeleton_new(s);

    agent = panorama_org_bluez_agent1_skeleton_new();

    g_signal_connect(agent, "handle-request-confirmation",
                     G_CALLBACK (on_request_confirmation), NULL);
    g_signal_connect(agent, "handle-request-authorization",
                     G_CALLBACK (on_request_authorization), NULL);


    panorama_object_skeleton_set_org_bluez_agent1(object, agent);

    g_object_unref(agent);


    /* Export the object (@manager takes its own reference to @object) */
    g_dbus_object_manager_server_export (manager, G_DBUS_OBJECT_SKELETON (object));

    g_object_unref (object);

    /* Export all objects */
    g_dbus_object_manager_server_set_connection (manager, connection);


    GError *err = NULL;
    GDBusProxy *manager_proxy = g_dbus_proxy_new_sync(connection, G_DBUS_PROXY_FLAGS_NONE, NULL,
                     "org.bluez", "/org/bluez", "org.bluez.AgentManager1",
                     NULL, &err);

    if(err) {
        g_print("fail");
    }

    g_dbus_proxy_call_sync(manager_proxy,
                           "RegisterAgent",
                           g_variant_new("(os)", "/com/panoramalabs/Agent1", "KeyboardDisplay"),
                           G_DBUS_CALL_FLAGS_NONE, -1, NULL, &err);
    if(err) {
        g_print("fail");
    }

    g_dbus_proxy_call_sync(manager_proxy,
                           "RequestDefaultAgent",
                           g_variant_new("(o)", "/com/panoramalabs/Agent1"),
                           G_DBUS_CALL_FLAGS_NONE, -1, NULL, &err);
    if(err) {
        g_print("fail");
    }

}

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
    g_print ("Acquired the name %s\n", name);
}

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
    g_print ("Lost the name %s\n", name);
}


gint
main (gint argc, gchar *argv[])
{
    GMainLoop *loop;
    guint id;

    loop = g_main_loop_new (NULL, FALSE);

    id = g_bus_own_name (G_BUS_TYPE_SYSTEM,
                         "com.panoramalabs",
                         G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT |
                         G_BUS_NAME_OWNER_FLAGS_REPLACE,
                         on_bus_acquired,
                         on_name_acquired,
                         on_name_lost,
                         loop,
                         NULL);

    g_main_loop_run (loop);

    g_bus_unown_name (id);


    g_main_loop_unref (loop);

    return 0;
}
/**

gdbus-codegen --interface-prefix com.panoramalabs. \
              --generate-c-code panoramalabs-service                        \
              --c-namespace Panorama                       \
              --c-generate-object-manager                 \
              --generate-docbook panoramalabs-service-generated-docs                       \
              panoramalabs-service.xml

**/