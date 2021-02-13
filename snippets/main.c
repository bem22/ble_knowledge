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




}