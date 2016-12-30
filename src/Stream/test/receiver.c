#include "StreamReceiver.h"
#include <tiny_log.h>

#define TAG "receiver"

static FILE * fp = NULL;

static void packet_handler(const char *packet, int len)
{
    LOG_D(TAG, "packet_handler: %d", len);

    fwrite(packet, (size_t)len, 1, fp);
}

int main(int argc, char *argv[])
{
    StreamReceiver * thiz = StreamReceiver_New();
    const char *key = "7MB1gHv/o96kMuO1cqT0Z0xOoslqwq7nw/2lpuSO";
    const char *ip = "192.168.31.119";  // ubuntu
//    const char *ip = "192.168.31.135";
    int port = 9999;
    uint32_t ssrc = 1;

    if (argc != 2)
    {
        printf("usage: %s [filename]\n", argv[0]);
        return 0;
    }

    fp = fopen(argv[1], "wb");
    if (fp == NULL) {
        printf("open file failed: %s\n", argv[1]);
        return 1;
    }

    StreamReceiver_Start(thiz, ip, port, key, ssrc, packet_handler);

    // StreamReceiver_Stop(thiz);

    int c = getchar();

//    while (1)
//    {
//        sleep(10);
//    }

    fclose(fp);

    return 0;
}
