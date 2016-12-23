#include "StreamReceiver.h"


static void packet_handler(const char *packet, int len)
{
}

int main()
{
    StreamReceiver * thiz = StreamReceiver_New();
    const char *key = "28A/qaC4dPj8MVE6v30t5egxwxpfk6NIzH5Fdk1D";
    const char *ip = "192.168.31.119";
    int port = 9999;
    uint32_t ssrc = 1;

    StreamReceiver_Start(thiz, ip, port, key, ssrc, packet_handler);

    StreamReceiver_Stop(thiz);

    return 0;
}
