#include "StreamReceiver.h"
#include <tiny_log.h>

#define TAG "receiver"

static void packet_handler(const char *packet, int len)
{
    LOG_D(TAG, "packet_handler: %d", len);
}

int main()
{
    StreamReceiver * thiz = StreamReceiver_New();
    const char *key = "28A/qaC4dPj8MVE6v30t5egxwxpfk6NIzH5Fdk1D";
    const char *ip = "192.168.31.119";
    int port = 9999;
    uint32_t ssrc = 1;

    StreamReceiver_Start(thiz, ip, port, key, ssrc, packet_handler);

    // StreamReceiver_Stop(thiz);

    while (1)
    {
        sleep(10);
    }

    return 0;
}
