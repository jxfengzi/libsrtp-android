/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   StreamSender.c
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "StreamSender.h"
#include <tiny_log.h>
#include <tiny_memory.h>
#include <config.h>
#include <rtp.h>
#include <arpa/inet.h>
#include "util.h"

#define TAG                 "StreamSender"
#define MAX_KEY_LEN         96

struct _StreamSender
{
    int                     sock;
    srtp_policy_t           policy;
    rtp_sender_t            sender;
    bool                    initialized;
};

StreamSender * StreamSender_New(void)
{
    StreamSender *thiz = NULL;

    do
    {
        thiz = (StreamSender *)tiny_malloc(sizeof(StreamSender));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED.");
            break;
        }

        if (RET_FAILED(StreamSender_Construct(thiz)))
        {
            StreamSender_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TinyRet StreamSender_Construct(StreamSender *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        srtp_err_status_t status;

        memset(thiz, 0, sizeof(StreamSender));
        thiz->initialized = false;

        memset(&thiz->policy, 0, sizeof(srtp_policy_t));

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

void StreamSender_Dispose(StreamSender *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamSender_Finalize(thiz);

    srtp_shutdown();

    memset(thiz, 0, sizeof(StreamSender));
}

void StreamSender_Delete(StreamSender *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamSender_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet StreamSender_Initialize(StreamSender *thiz, const char *ip, int port, const char *keyBase64, uint32_t ssrc)
{
    TinyRet ret = TINY_RET_OK;

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

        if (thiz->initialized)
        {
            LOG_E(TAG, "already initialized");
            ret = TINY_RET_E_INITIALIZED;
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

        thiz->policy.ssrc.type  = ssrc_any_outbound;
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

        /**
         * 这里应该可以指定本地发送端口
         */

        /* bind to local socket (to match crypto policy, if need be) */
        {
            struct sockaddr_in local;
            memset(&local, 0, sizeof(struct sockaddr_in));
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            local.sin_port = htons(port);
            ret = bind(thiz->sock, (struct sockaddr *) &local, sizeof(struct sockaddr_in));
            if (ret < 0)
            {
                LOG_E(TAG, "bind failed: %d", port);
                ret = TINY_RET_E_ARG_INVALID;
                break;
            }
        }

        thiz->sender = rtp_sender_alloc();
        if (thiz->sender == NULL)
        {
            LOG_E(TAG, "rtp_sender_alloc failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        rtp_sender_init(thiz->sender, thiz->sock, name, ssrc);

        status = (srtp_err_status_t) rtp_sender_init_srtp(thiz->sender, &thiz->policy);
        if (status != srtp_err_status_ok) {
            LOG_E(TAG, "error: srtp_create() failed with code %d", status);
            ret = TINY_RET_E_INTERNAL;
            break;
        }

        thiz->initialized = true;
    } while (false);

    return ret;
}

TinyRet StreamSender_Sendto(StreamSender *thiz, char buf[1375], size_t size)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        if (! thiz->initialized) {
            LOG_E(TAG, "not initialized");
            ret = TINY_RET_E_NOT_INITIALIZED;
            break;
        }

        rtp_sendto(thiz->sender, buf, size);

    } while (false);

    return ret;
}

void StreamSender_Finalize(StreamSender *thiz)
{
    if (! thiz->initialized) {
        return;
    }

    rtp_sender_deinit_srtp(thiz->sender);
    rtp_sender_dealloc(thiz->sender);
    close(thiz->sock);

    thiz->initialized = false;
}
