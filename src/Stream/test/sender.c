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
        printf("usage: %s [filename]", argv[0]);
        return 0;
    }

    fp = fopen(argv[1], "rb");

    StreamSender_Initialize(thiz, ip, port, key, ssrc);

    while (1)
    {
        char buf[PACKET_SIZE];
        size_t size = 0;

        memset(buf, 0, PACKET_SIZE);
        size = fread(buf, PACKET_SIZE, 1, fp);
        if (size <= 0)
        {
            break;
        }

        StreamSender_Sendto(thiz, buf, size);

        usleep(USEC_RATE);
    }

    fclose(fp);

    StreamSender_Finalize(thiz);
    StreamSender_Delete(thiz);

    return 0;
}