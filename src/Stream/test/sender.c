#include "StreamSender.h"

//#define file                "./demo.h264"
#define PACKET_SIZE         1375
#define USEC_RATE           (5e5)

int main(int argc, char *argv[])
{
    const char *key = "7MB1gHv/o96kMuO1cqT0Z0xOoslqwq7nw/2lpuSO";
    const char *ip = "192.168.31.135";
    int port = 9999;
    uint32_t ssrc = 1;
    FILE * fp = NULL;
    StreamSender * thiz = StreamSender_New();

    if (argc != 2)
    {
        printf("usage: %s [filename]\n", argv[0]);
        return 0;
    }

    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("open file failed: %s\n", argv[1]);
        return 1;
    }

    if (RET_FAILED(StreamSender_Initialize(thiz, ip, port, key, ssrc)))
    {
        printf("StreamSender_Initialize failed\n");
        return 2;
    }

    printf("Sending ...\n");

    while (1)
    {
        TinyRet ret = TINY_RET_OK;
        char buf[PACKET_SIZE];
        size_t size = 0;

        memset(buf, 0, PACKET_SIZE);
        size = fread(buf, 1, PACKET_SIZE, fp);
        if (size <= 0)
        {
            break;
        }

//        printf("PACKET_SIZE: %d\n", PACKET_SIZE);
//        printf("size: %ld\n", size);

        ret = StreamSender_Sendto(thiz, buf, size);

        printf("StreamSender_Sendto: %lld\n", ret);

        usleep(USEC_RATE);
    }

    fclose(fp);

    StreamSender_Finalize(thiz);
    StreamSender_Delete(thiz);

    return 0;
}