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
    const char *key = "7MB1gHv/o96kMuO1cqT0Z0xOoslqwq7nw/2lpuSO";
//    const char *ip = "192.168.31.119";
    const char *ip = "192.168.31.135";
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
