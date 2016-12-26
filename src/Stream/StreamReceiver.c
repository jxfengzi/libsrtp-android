/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   StreamReceiver.c
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "StreamReceiver.h"
#include "util.h"
#include <tiny_log.h>
#include <tiny_memory.h>
#include <TinyThread.h>
#include <config.h>
#include <rtp.h>
#include <arpa/inet.h>

#define TAG                 "StreamReceiver"
#define PACKET_LEN          1378
#define MAX_KEY_LEN         96

static void loop(void *param);

struct _StreamReceiver
{
    int                     sock;
    srtp_policy_t           policy;
    rtp_receiver_t          receiver;
    TinyThread              thread;
    StreamPacketHandler     handler;
    bool                    running;
};

StreamReceiver * StreamReceiver_New(void)
{
    StreamReceiver *thiz = NULL;

    do
    {
        thiz = (StreamReceiver *)tiny_malloc(sizeof(StreamReceiver));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED.");
            break;
        }

        if (RET_FAILED(StreamReceiver_Construct(thiz)))
        {
            StreamReceiver_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TinyRet StreamReceiver_Construct(StreamReceiver *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        srtp_err_status_t status;

        memset(thiz, 0, sizeof(StreamReceiver));
        thiz->handler = NULL;
        thiz->running = false;

        memset(&thiz->policy, 0, sizeof(srtp_policy_t));

        TinyThread_Construct(&thiz->thread);
        TinyThread_Initialize(&thiz->thread, loop, thiz, "StreamReceiver");

        status = srtp_init();
        if (status != 0)
        {
            LOG_E(TAG, "srtp_init failed: %d", status);
            ret = TINY_RET_E_INTERNAL;
            break;
        }
    } while (false);

    return ret;
}

void StreamReceiver_Dispose(StreamReceiver *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamReceiver_Stop(thiz);

    srtp_shutdown();

    memset(thiz, 0, sizeof(StreamReceiver));
}

void StreamReceiver_Delete(StreamReceiver *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamReceiver_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet StreamReceiver_Start(StreamReceiver *thiz,
                          const char *ip,
                          int port,
                          const char *keyBase64,
                          uint32_t ssrc,
                          StreamPacketHandler handler)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ip, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(port, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(keyBase64, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ssrc, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    thiz->handler = handler;

    do
    {
        srtp_err_status_t status;
        char key[MAX_KEY_LEN];
        srtp_sec_serv_t sec_servs = sec_serv_none;
        struct in_addr address;
        struct sockaddr_in name;
        int expected_len = 0;
        int len = 0;
        int pad = 0;

        if (thiz->running)
        {
            LOG_E(TAG, "already started");
            ret = TINY_RET_E_STARTED;
            break;
        }

        if (0 == inet_aton(ip, &address)) {
            LOG_E(TAG, "cannot parse IP v4 address %s", ip);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        if (address.s_addr == INADDR_NONE) {
            LOG_E(TAG, "address error: %s", ip);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        thiz->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (thiz->sock < 0)
        {
            LOG_E(TAG, "couldn't open socket: %d\n", errno);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        name.sin_addr   = address;
        name.sin_family = PF_INET;
        name.sin_port   = htons(port);

        /**
         * 128 bit key
         */
        sec_servs |= sec_serv_conf;             // -e
        // sec_servs |= sec_serv_auth;          // -a

        srtp_crypto_policy_set_rtp_default(&thiz->policy.rtp);
        srtp_crypto_policy_set_rtcp_default(&thiz->policy.rtcp);

        thiz->policy.ssrc.type  = ssrc_any_inbound;
        thiz->policy.ssrc.value = ssrc;
        thiz->policy.key  = (uint8_t *) key;
        thiz->policy.ekt  = NULL;
        thiz->policy.next = NULL;
        thiz->policy.window_size = 128;
        thiz->policy.allow_repeat_tx = 0;
        thiz->policy.rtp.sec_serv = sec_servs;
        thiz->policy.rtcp.sec_serv = sec_serv_none;  /* we don't do RTCP anyway */

        /*
         * read key from hexadecimal or base64 on command line into an octet string
         */
        pad = 0;
        expected_len = (thiz->policy.rtp.cipher_key_len * 4)/3;
        len = base64_string_to_octet_string(key, &pad, keyBase64, expected_len);
        if (pad != 0)
        {
            LOG_E(TAG, "error : padding in base64 unexpected");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        /* check that hex string is the right length */
        if (len < expected_len)
        {
            LOG_E(TAG, "error: too few digits in key/salt (should be %d digits, found %d)", expected_len, len);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        if ((int) strlen(keyBase64) > thiz->policy.rtp.cipher_key_len * 2)
        {
            LOG_E(TAG, "error: too many digits in key/salt (should be %d hexadecimal digits, found %u)", thiz->policy.rtp.cipher_key_len * 2, (unsigned)strlen(keyBase64));
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        printf("set master key/salt to %s/", octet_string_hex_string(key, 16));
        printf("%s\n", octet_string_hex_string(key + 16, 14));

        if (bind(thiz->sock, (struct sockaddr *)&name, sizeof(name)) < 0)
        {
            close(thiz->sock);
            LOG_E(TAG, "socket bind error");
            ret = TINY_RET_E_SOCKET_BIND;
            break;
        }

        thiz->receiver = rtp_receiver_alloc();
        if (thiz->receiver == NULL) {
            LOG_E(TAG, "error: malloc() failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        rtp_receiver_init(thiz->receiver, thiz->sock, name, ssrc);

        status = (srtp_err_status_t) rtp_receiver_init_srtp(thiz->receiver, &(thiz->policy));
        if (status != srtp_err_status_ok) {
            LOG_E(TAG, "error: srtp_create() failed with code %d", status);
            ret = TINY_RET_E_INTERNAL;
            break;
        }

        TinyThread_Start(&thiz->thread);
    } while (false);

    return ret;
}

TinyRet StreamReceiver_Stop(StreamReceiver *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->running)
    {
        close(thiz->sock);
        thiz->running = false;

        TinyThread_Join(&thiz->thread);

        rtp_receiver_deinit_srtp(thiz->receiver);
        rtp_receiver_dealloc(thiz->receiver);
    }

    return TINY_RET_OK;
}

static void loop(void *param)
{
    StreamReceiver *thiz = (StreamReceiver *)param;

    LOG_D(TAG, "start receiving ...");

    thiz->running = true;

    while (thiz->running)
    {
        char packet[PACKET_LEN];
        int packet_len = PACKET_LEN;
        int size = 0;

        size = rtp_recvfrom(thiz->receiver, packet, &packet_len);
        if (size < 0)
        {
            LOG_D(TAG, "recvfrom < 0");
            break;
        }

        thiz->handler(packet, size);
    }

    LOG_D(TAG, "stop receiving");
}