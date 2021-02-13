#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

void hello_world() {
    g_print("Hello world");
}

GDBusConnection *conn;

void *handle_ble_connection(void *result2) {

    // Extract argument
    GVariant *result = result2;


    int fd, mtu;
    g_variant_get_child(result, 0, "h", &fd);
    g_variant_get_child(result, 1, "q", &mtu);

    struct pollfd fds[1];
    int flags = fcntl(fd, F_GETFL);

    if (flags < 0)
        err(1, "error when getting flags");

    flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) < 0)
        err(1, "error when setting flags");

    fds[0].fd = fd;
    fds[0].events = POLLIN | POLLPRI | POLLERR;

    char* buff =(char *)malloc((sizeof(char)+1)*mtu);

    for (;;) {
        if (read(fd, buff, mtu) < 0) {

            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                poll(fds, 1, 5); continue; }
            else err(1, "error when reading");
        }
        printf("%s", buff);
    }


}



int main(void) {
    int *keepalive = malloc(sizeof(int));
    *keepalive = 1;

    pthread_t *ble_conn_watcher = (pthread_t*) malloc(sizeof(pthread_t));


    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);



    GError *erro = NULL;
    conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &erro);
    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while attempting to establish a connection with DBUS", erro->message);
        return 1;
    } else {
        g_print("\n%s\n", "Connection to DBUS successful!");
    }

    GDBusProxy *proxy = g_dbus_proxy_new_sync(conn,
                                              G_DBUS_PROXY_FLAGS_NONE,
                                              NULL,
                                              "org.bluez",
                                              "/org/bluez/hci0",
                                              "org.bluez.Adapter1",
                                              NULL,
                                              &erro);

    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create a proxy", erro->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy created successfully!");
    }


    g_dbus_proxy_call_sync(proxy,
                           "StartDiscovery",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &erro);

    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while calling a method through proxy", erro->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy method call, successful!");
    }


    g_dbus_proxy_call_sync(proxy,
                           "StopDiscovery",
                           NULL,
                           G_DBUS_CALL_FLAGS_NONE,
                           -1,
                           NULL,
                           &erro);

    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while calling a method through proxy", erro->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy method call, successful!");
    }

    sleep(4);

    GDBusProxy *beacon_proxy = g_dbus_proxy_new_sync(conn,
                                                     G_DBUS_PROXY_FLAGS_NONE,
                                                     NULL,
                                                     "org.bluez",
                                                     "/org/bluez/hci0/dev_DC_3F_32_42_62_B0",
                                                     "org.bluez.Device1",
                                                     NULL,
                                                     &erro);

    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create a proxy", erro->message);
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
                           &erro);
    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while calling a method through proxy123", erro->message);
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
                                                   &erro);
    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create a proxy", erro->message);
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
                               &erro);
        if(erro != NULL) {
            g_print("%s\n%s\n", "Error while calling a method through proxy", erro->message);
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
                                        &erro);
    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while attempting to create a proxy", erro->message);
        return 1;
    } else {
        g_print("\n%s\n", "Proxy created successfully!");
    }

    GVariantBuilder builder2;
    g_variant_builder_init (&builder2, G_VARIANT_TYPE("a{sv}"));
    GVariant *argument2 = g_variant_builder_end(&builder2);
    GVariant *result2 = g_dbus_proxy_call_sync(gatt_proxy2,
                                              "AcquireNotify",
                                              g_variant_new("(@a{sv})",argument2),
                                              G_DBUS_CALL_FLAGS_NONE,
                                              -1,
                                              NULL,
                                              &erro);
    if(erro != NULL) {
        g_print("%s\n%s\n", "Error while attempting to call method", erro->message);
        return 1;
    } else {
        g_print("\n%s\n", "AcquireNotify!");
    }

    // Start the thread that watches over interfaces
    pthread_create(ble_conn_watcher, NULL, &handle_ble_connection, result2);
    g_main_loop_run(loop);


}
