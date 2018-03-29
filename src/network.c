#include "network.h"
#include "util.h"

#include <libvirt/libvirt.h>

static virtDBusGDBusPropertyTable virtDBusNetworkPropertyTable[] = {
    { 0 }
};

static virtDBusGDBusMethodTable virtDBusNetworkMethodTable[] = {
    { 0 }
};

static gchar **
virtDBusNetworkEnumerate(gpointer userData)
{
    virtDBusConnect *connect = userData;
    g_autoptr(virNetworkPtr) networks = NULL;
    gint num = 0;
    gchar **ret = NULL;

    if (!virtDBusConnectOpen(connect, NULL))
        return NULL;

    num = virConnectListAllNetworks(connect->connection, &networks, 0);
    if (num < 0)
        return NULL;

    if (num == 0)
        return NULL;

    ret = g_new0(gchar *, num + 1);

    for (gint i = 0; i < num; i++) {
        ret[i] = virtDBusUtilBusPathForVirNetwork(networks[i],
                                                  connect->networkPath);
    }

    return ret;
}

static GDBusInterfaceInfo *interfaceInfo = NULL;

void
virtDBusNetworkRegister(virtDBusConnect *connect,
                        GError **error)
{
    connect->networkPath = g_strdup_printf("%s/network", connect->connectPath);

    if (!interfaceInfo) {
        interfaceInfo = virtDBusGDBusLoadIntrospectData(VIRT_DBUS_NETWORK_INTERFACE,
                                                        error);
        if (!interfaceInfo)
            return;
    }

    virtDBusGDBusRegisterSubtree(connect->bus,
                                 connect->networkPath,
                                 interfaceInfo,
                                 virtDBusNetworkEnumerate,
                                 virtDBusNetworkMethodTable,
                                 virtDBusNetworkPropertyTable,
                                 connect);
}